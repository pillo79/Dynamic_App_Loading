/**
 ******************************************************************************
 * @file	main.c
 * @brief	Main file
 *			Load the app and also turn on a LED
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
#include "app_loader/app_loader.h"

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/logging/log.h>

#define LED0_NODE DT_ALIAS(led0)
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
	#if DT_PHA_HAS_CELL(LED0_NODE, gpios, flags)
		#define LED0_FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
	#else
		#define LED0_FLAGS	0
	#endif

#else
	#error "Unsupported board: led0 is not aliased in device tree."
#endif

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define RING_BUF_SIZE	(1024)

uint8_t ring_buffer[RING_BUF_SIZE];
struct ring_buf ringbuf;

static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					sizeof(buffer));

			recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len < 0) {
				LOG_ERR("Failed to read UART FIFO");
				recv_len = 0;
			};

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len) {
				LOG_ERR("Drop %u bytes", recv_len - rb_len);
			}

			LOG_DBG("tty fifo -> ringbuf %d bytes", rb_len);
			if (rb_len) {
				uart_irq_tx_enable(dev);
			}
		}

		if (uart_irq_tx_ready(dev)) {
			uint8_t buffer[64];
			int rb_len, send_len;

			rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
			if (!rb_len) {
				LOG_DBG("Ring buffer empty, disable TX IRQ");
				uart_irq_tx_disable(dev);
				continue;
			}

			send_len = uart_fifo_fill(dev, buffer, rb_len);
			if (send_len < rb_len) {
				LOG_ERR("Drop %d bytes", rb_len - send_len);
			}

			LOG_DBG("ringbuf -> tty fifo %d bytes", send_len);
		}
	}
}


#include "stm32h7xx_hal_rtc.h"

void HAL_RTCEx_BKUPWrite(RTC_HandleTypeDef * hrtc, uint32_t BackupRegister, uint32_t Data)
{
	uint32_t tmp;

	/* Check the parameters */
	assert_param(IS_RTC_BKP(BackupRegister));

	/* Point on address of first backup register */
#if defined(TAMP_BKP0R)
	tmp = (uint32_t) & (((TAMP_TypeDef *)((uint32_t)hrtc->Instance + TAMP_OFFSET))->BKP0R);
#endif /* TAMP_BKP0R */
#if defined(RTC_BKP0R)
	tmp = (uint32_t) & (hrtc->Instance->BKP0R);
#endif /* RTC_BKP0R */

	tmp += (BackupRegister * 4U);

	/* Write the specified register */
	*(__IO uint32_t *)tmp = (uint32_t)Data;
}

void on1200bpstouch(const struct device *dev, int rate) {
	RTC_HandleTypeDef RTCHandle;
	RTCHandle.Instance = RTC;
	if (rate == 1200) {
		usb_disable();
		HAL_RTCEx_BKUPWrite(&RTCHandle, RTC_BKP_DR0, 0xDF59);
		NVIC_SystemReset();
	}
}

int main(void)
{
	int ret;
	const struct device *dev;
	uint32_t baudrate, dtr = 0U;

	dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(dev)) {
		LOG_ERR("CDC ACM device not ready");
		return 0;
	}

	#if defined(CONFIG_USB_DEVICE_STACK_NEXT)
	ret = enable_usb_device_next();
	#else
	ret = usb_enable(NULL);
	#endif

	if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return 0;
	}

	ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

	cdc_acm_dte_rate_callback_set(dev, on1200bpstouch);
	uart_irq_callback_set(dev, interrupt_handler);

	/* Enable rx interrupts */
	uart_irq_rx_enable(dev);

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_INACTIVE | LED0_FLAGS);
	if (ret < 0) {
		return -1;
	}

	// Load the app directly from Flash
	const uint8_t *app = (const uint8_t *) 0x08060000;
	if (LoadApp(app)<0 ){
		while (1) {
			gpio_pin_set_dt(&led0, 1);
			k_usleep(100000);
			gpio_pin_set_dt(&led0, 0);
			k_usleep(100000);
		}
	}

	gpio_pin_set_dt(&led0, 1);
	return 0;
}
