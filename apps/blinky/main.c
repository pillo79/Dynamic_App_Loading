
#include "sysled.h"

uint8_t led_num = 0x01;
uint8_t led_status;

uint8_t dummy_function_2(){
    static int status = 0;
    status = !status;
    return status;
}

uint8_t dummy_function(){
    uint8_t led_status = dummy_function_2();
    return led_status;
}

// dummy_function, dummy_function_2 and the global variables
// are there just to verify if the relocation is actually working

static volatile int tot = 0;

int32_t k_usleep(uint32_t us);

int main() {
SetLed(0, 0);
led_num-=1;
    while (1) {
	led_status=dummy_function();
	SetLed(led_num, led_status);
	k_usleep(1000*1000);
    }
    return 0;
}
