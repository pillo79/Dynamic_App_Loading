/**
 ******************************************************************************
 * @file	app_loader.c
 * @brief	Contains function for loading the app and setting up the freertos
 *			task
 ******************************************************************************
 * @attention
 *
 * LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Rohit Gujarathi rgujju.github.io
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <zephyr/kernel.h>
#include "string.h" //TODO: Replace with a minimal libc implementation
#include "stdlib.h" //TODO: Replace with a minimal libc implementation
#include "app_loader/app_loader.h"

#include <zephyr/logging/log.h>
// TODO: Change Log module debug level from build system
LOG_MODULE_REGISTER(app_loader, 4);

// TODO Add Tests:
// 2) Check name length =0,1,15,16,and greater, Check if it is 0s after name length
// 3) Check little endian/ big endian or whatever
// 4) malloc fails
// 5) app loading fails during task creation
// 6) tinf->bin should fail

// RGREMOVE
#define DUMMY_SYS_ADDR 0xA5A5A5A5
uint32_t sys = 0xdeadbeef;
//void blinky() {
//	  while (1) {
//		  SetLed(LED0, LED_ON);
//		  k_msleep(1000);
//		  SetLed(LED0, LED_OFF);
//		  k_msleep(1000);
//	  }
//}
/////////////

#define DEFAULT_STACK_SIZE 1024
#define DEFAULT_HEAP_SIZE 4096
K_THREAD_STACK_DEFINE(app_stack, DEFAULT_STACK_SIZE);
K_THREAD_STACK_DEFINE(app_heap_buf, DEFAULT_HEAP_SIZE);

K_MEM_PARTITION_DEFINE(app_heap_partition, app_heap_buf, sizeof(app_heap_buf), K_MEM_PARTITION_P_RW_U_RW);

struct k_thread app_thread; //TODO: Dynamically allocate thread struct
struct k_heap app_heap;

int8_t LoadApp(const uint8_t* tinf_img) {

	tinf_t* tinf = (tinf_t*)tinf_img;
	// Check if it is actually an APP
	// Check the magic, ie start of the header contains 'TINF'
	uint32_t* data_base = tinf->bin+tinf->text_size;

	if (memcmp(tinf->magic, "TINF", 4) != 0)
		return APP_INVALID;

	// Valid TINF Format app
	printk("Loading app: %s %hu.%hu\n", tinf->app_name, tinf->major_version, tinf->minor_version);
	printk("App text size: %hu 32 bit word\n", tinf->text_size);
	printk("App data size: %hu 32 bit word\n", tinf->data_size);
	printk("App bss size: %hu 32 bit word\n", tinf->bss_size);
	printk("App GOT entries: %ld\n", tinf->got_entries);

	// Allocate memory for data, bss and got sections of the app
	uint32_t app_data_size = tinf->data_size + tinf->got_entries + tinf->bss_size;

	k_heap_init(&app_heap, app_heap_buf, sizeof(app_heap_buf));
	printk("Allocating app memory of %ld bytes\n", app_data_size*4);
	uint32_t* app_data_base = k_heap_alloc(&app_heap, app_data_size*4, K_NO_WAIT);
	printk("App data base: 0x%08X\n", app_data_base);

	if(tinf->data_size > 0) {
		// Copy data section from flash to the RAM we allocated above
		memcpy(app_data_base, data_base, (tinf->data_size*4));
		printk("Data at data section (flash): 0x%08X\n", *(uint32_t*)(tinf->bin+(tinf->text_size)));
		printk("Data at data section (RAM): 0x%08X\n", *(uint32_t*)(app_data_base));
	}

	// app_got_base is the value which will get loaded to r9
	// This is address of the base of GOT which will actually be used
	// by the app for global data accesses
	// This value is passed as the parameter to the app_main. The app will
	// copy the this value from r0 to r9 register
	uint32_t* app_got_base = app_data_base + tinf->data_size;

	// If there is any global data of the app then copy to RAM
	if(tinf->got_entries > 0) {
		// Copy the GOT from flash to RAM
		// app_got_base is the base of the GOT in the RAM
		// this is where GOT will be copied to
		printk("GOT base: %p\n", app_got_base);

		// got_entries_base is the base of the GOT in the flash
		// tinf->got_entries number of entries from this location
		// needs to be copied into RAM
		uint32_t* got_entries_base = data_base + tinf->data_size;

		// While copying add the base address (app_data_base) in RAM to each element of the GOT
		// TODO: Add more explaination about this
		// Need to subtract the data_offset to get the location with respect to 0
		const uint32_t data_offset = 0x10000000;
		for(uint8_t i = 0; i < tinf->got_entries; i++) {
			if(*(got_entries_base+i) >= data_offset) {
				// If the value is greater than data_offset then it is in the RAM section.
				// So add the app_data_base to it.
				*(app_got_base+i) = ((*(got_entries_base+i))-data_offset)+(uint32_t)((uintptr_t)app_data_base);
			} else {
				// else it is a relocation in the flash
				// so add the flash->bin ie the base of the app in the flash to it
				*(app_got_base+i) = (*(got_entries_base+i))+(uint32_t)((uintptr_t)tinf->bin);
			}
			printk("%2i) %08x -> %08x\n", i, *(got_entries_base+i), *(app_got_base+i));
		}
	}

	// If there is any bss in the app then set that much space to 0
	if(tinf->bss_size > 0) {
		// Set BSS section to 0
		memset(app_data_base+(tinf->data_size+tinf->got_entries), 0, tinf->bss_size*4);
		printk("Data at bss section (RAM): 0x%08X\n", *(app_data_base+(tinf->data_size+tinf->got_entries)));
	}

	// OR'ed with 1 to set the thumb bit, TODO: Check if this is handled internally by zephyr
	int (*app_main)() = (void*)(((uintptr_t)(tinf->bin))|1);
	printk("App entry point: 0x%08X\n", app_main);
	// TODO: The following line gives segfault for some reason
	printk("Data at app entry point:");
	for (int i=0; i<16; ++i)
		printk(" %02x", ((uint8_t*)tinf->bin)[i]);
	printk("\n");

	// Create thread
	k_thread_create(&app_thread,
					app_stack,
					DEFAULT_STACK_SIZE,
					(k_thread_entry_t)app_main,
					app_got_base, NULL, NULL,
					3, K_USER, K_FOREVER);

	// Allow thread access to the app heap (includes GOT, data and BSS!)
	k_thread_heap_assign(&app_thread, &app_heap);
	k_mem_domain_add_partition(&k_mem_domain_default, &app_heap_partition);

	k_thread_start(&app_thread);
	printk("Gone!\n");

	return 0;
}

