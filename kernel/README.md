To build, run `compile_for_core.sh <path-to-Arduino-Zephyr-core>` . 
This will produce a folder `core` with the relevant build files, and copy:

  * `zephyr.bin` in Arduino core dir `bootloaders/arduino_giga_r1_m7`
  * `libzephyr_api.a` in Arduino core dir `variants/arduino_giga_r1_m7/libs`

`zephyr_api.h`, if changed, must be manually synced with Arduino core dir `./variants/arduino_giga_r1_m7`

Board must be updated with "Burn bootloader" when a new kernel is compiled.
