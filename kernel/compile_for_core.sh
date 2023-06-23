#!/bin/bash
ARDUINO_ZEPHYR_CORE_DIR=$1
if ! [ -f $ARDUINO_ZEPHYR_CORE_DIR/boards.txt ] ; then
	echo "Usage: $0 <arduino_zephyr_core_dir>"
	exit 1
fi
set -e
rm -rf core/
west build -b arduino_giga_r1_m7 . -p -t userlib -- -DUSERLIB=1
mv build/userlib core
west build -b arduino_giga_r1_m7 . -p
cp build/zephyr/zephyr.{bin,elf,map} core/

echo
cp core/zephyr.bin $ARDUINO_ZEPHYR_CORE_DIR/bootloaders/arduino_giga_r1_m7/zephyr.bin
cp core/libzephyr_api.a $ARDUINO_ZEPHYR_CORE_DIR/variants/arduino_giga_r1_m7/libs/
echo "Updated core in $ARDUINO_ZEPHYR_CORE_DIR"
