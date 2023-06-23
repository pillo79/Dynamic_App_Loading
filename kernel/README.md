Steps:
=====

* `compile_kernel.sh` 
  * will produce `build/zephyr/zephyr.bin` which is the "bootloader"
  * must be copied in Arduino core dir `bootloaders/arduino_giga_r1_m7`
  * board must be updated with "Burn bootloader" afterwards

* `compile_userlib.sh` will produce a directory `build/userlib` with 2 files:
  * `zephyr_api.h` must be copied in Arduino core dir `./variants/arduino_giga_r1_m7`
  * `libzephyr_api.a` must be copied in Arduino core dir `./variants/arduino_giga_r1_m7/libs`
  * will be used in next rebuild
