#include "Arduino.h"

#if !CONFIG_USERLIB

extern "C" {

static inline void z_vrfy_pinMode(pin_size_t pinNumber, PinMode mode)
{
	return pinMode(pinNumber, mode);
}

static inline void z_vrfy_digitalWrite(pin_size_t pinNumber, PinStatus status)
{
	return digitalWrite(pinNumber, status);
}

static inline PinStatus z_vrfy_digitalRead(pin_size_t pinNumber)
{
	return digitalRead(pinNumber);
}

static inline unsigned long z_vrfy_millis(void)
{
	return millis();
}

static inline unsigned long z_vrfy_micros(void)
{
	return micros();
}

static inline void z_vrfy_delay(unsigned long ms)
{
	delay(ms);
}

static inline void z_vrfy_delayMicroseconds(unsigned int us)
{
	delayMicroseconds(us);
}

#include <syscalls/pinMode_mrsh.c>
#include <syscalls/digitalWrite_mrsh.c>
#include <syscalls/digitalRead_mrsh.c>
#include <syscalls/millis_mrsh.c>
#include <syscalls/micros_mrsh.c>
#include <syscalls/delay_mrsh.c>
#include <syscalls/delayMicroseconds_mrsh.c>

}

#else

#include <syscalls/Common.h>

#endif
