# Exploring esp32_devkitc_wroom

## References


## Intro
- I bought an `esp32_devkitc_wroom` kit and I know nothing about it

- This page documents the process of exploring the chip and kit

## Concepts
- Using `esptool.py` it is possible to inspect the chip
```bash
ggm@gAN515-52:~/zephyrproject $ find . -iname "*esptool*"
...
./modules/hal/espressif/components/esptool_py/esptool/esptool.py
...
ggm@gAN515-52:~/zephyrproject $ sudo ./modules/hal/espressif/components/esptool_py/esptool/esptool.py -c auto -p /dev/ttyUSB1 -b 115200 --before default_reset chip_id
esptool.py v4.5
Serial port /dev/ttyUSB1
Connecting....
Detecting chip type... Unsupported detection protocol, switching and trying again...
Connecting....
Detecting chip type... ESP32
Chip is ESP32-D0WD-V3 (revision v3.1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: fc:b4:67:50:cc:a0
Uploading stub...
Running stub...
Stub running...
Warning: ESP32 has no Chip ID. Reading MAC instead.
MAC: fc:b4:67:50:cc:a0
Hard resetting via RTS pin...
ggm@gAN515-52:~/zephyrproject $ sudo ./modules/hal/espressif/components/esptool_py/esptool/esptool.py -c auto -p /dev/ttyUSB1 -b 115200 --before default_reset flash_id
esptool.py v4.5
Serial port /dev/ttyUSB1
Connecting....
Detecting chip type... Unsupported detection protocol, switching and trying again...
Connecting....
Detecting chip type... ESP32
Chip is ESP32-D0WD-V3 (revision v3.1)
Features: WiFi, BT, Dual Core, 240MHz, VRef calibration in efuse, Coding Scheme None
Crystal is 40MHz
MAC: fc:b4:67:50:cc:a0
Uploading stub...
Running stub...
Stub running...
Manufacturer: 5e
Device: 4016
Detected flash size: 4MB
Hard resetting via RTS pin...
```

- It is possible to see that my chip is actually `ESP32-D0WD-V3`, with the following characteristics:

    - Dual Core

    - Crystal is 40MHz

    - MAC: fc:b4:67:50:cc:a0

    - Detected flash size: 4MB

- It is also possible to erase the flash memory:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject $ sudo ./modules/hal/espressif/components/esptool_py/esptool/esptool.py -c auto -p /dev/ttyUSB1 -b 115200 --before default_reset erase_flash
```