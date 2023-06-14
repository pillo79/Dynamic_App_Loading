/**
 ******************************************************************************
 * @file    main.c
 * @brief   Main file
 *          Load the app and also turn on a LED
 ******************************************************************************
 * @attention
 *
 * LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Rohit Gujarathi
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

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
// TODO: Change Log module debug level from build system
LOG_MODULE_REGISTER(main, 4);

#include "utilities.h"
#include "sys_module/sysled.h"
#include "syscalls/sysled.h"
#include "app_loader/app_loader.h"

int main ( void ) {
    initLeds();

    // Load the app directly from Flash
    const uint8_t *app = (const uint8_t *) 0x08060000;
    if(LoadApp(app)<0){
    	while(1){
    	    SetLed(LED0, LED_ON);
	    k_usleep(100000);
    	    SetLed(LED0, LED_OFF);
	    k_usleep(100000);
    	}
    }

    SetLed(LED0, LED_ON);
    return 0;
}
