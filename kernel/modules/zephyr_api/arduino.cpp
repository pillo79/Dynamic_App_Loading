#include "Arduino.h"

#if !USERLIB

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

#include <syscalls/pinMode_mrsh.c>
#include <syscalls/digitalWrite_mrsh.c>
#include <syscalls/digitalRead_mrsh.c>
}

#else

#define __pinned_func
#include <syscalls/Common.h>

#endif
