/**
 ******************************************************************************
 * @file    zephyr_api.c
 * @brief   Contains C API trampoline function implementations
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

#if CONFIG_USERLIB

// make sure syscalls are not inlined
#include <zephyr/toolchain/common.h>
#undef __syscall
#define __syscall

#include <zephyr/cache.h>
#include <zephyr/device.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/bbram.h>
#include <zephyr/drivers/can.h>
#include <zephyr/drivers/console/uart_mux.h>
#include <zephyr/drivers/counter.h>
#include <zephyr/drivers/dac.h>
#include <zephyr/drivers/dma.h>
#include <zephyr/drivers/eeprom.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/drivers/espi.h>
#include <zephyr/drivers/flash/flash_simulator.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/flash/nrf_qspi_nor.h>
#include <zephyr/drivers/fuel_gauge.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/drivers/i3c.h>
#include <zephyr/drivers/ipm.h>
#include <zephyr/drivers/kscan.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/mbox.h>
#include <zephyr/drivers/mdio.h>
#include <zephyr/drivers/peci.h>
#include <zephyr/drivers/ps2.h>
#include <zephyr/drivers/ptp_clock.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/reset.h>
#include <zephyr/drivers/retained_mem.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/rtc/maxim_ds3231.h>
#include <zephyr/drivers/rtc/maxim_ds3231.h>
#include <zephyr/drivers/sdhc.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/sip_svc/sip_svc_driver.h>
#include <zephyr/drivers/smbus.h>
#include <zephyr/drivers/spi.h>
#include <zephyr/drivers/syscon.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/usb/usb_bc12.h>
#include <zephyr/drivers/virtualization/ivshmem.h>
#include <zephyr/drivers/w1.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/mgmt/updatehub.h>
#include <zephyr/net/ethernet.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/phy.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/socket_select.h>
#include <zephyr/posix/time.h>
#include <zephyr/random/rand32.h>
#include <zephyr/rtio/rtio.h>
#include <zephyr/sys/errno_private.h>
#include <zephyr/sys/kobject.h>
#include <zephyr/sys/libc-hooks.h>
#include <zephyr/sys/mem_manage.h>
#include <zephyr/sys/mutex.h>
#include <zephyr/sys/time_units.h>

#endif /* CONFIG_USERLIB */
