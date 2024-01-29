# Cheat Sheet

- When working with `esp32_devkitc_wroom` I had problems flashing the board

- To work around the issues, had to do the following:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ sudo usermod -a -G dialout $USER
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ sudo chmod a+rw /dev/ttyUSB0

# Now, holding the boot button, flash an image:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ west flash
```

## Commands references
- `sudo apt install stlink-tools`
- `st-flash --connect-under-reset erase`
- `west build --board=stm32f4_disco --pristine --sysbuild -- -DSB_CONFIG_BOOT_SIGNATURE_KEY_FILE=\""$(pwd)/../privkeys/root-rsa-2048-01.pem"\"`
- `sudo picocom /dev/ttyUSB0 --baud 115200`
- `mcumgr conn add ggmconn type="serial" connstring="dev=/dev/ttyUSB0,baud=115200,mtu=256"`
- `mcumgr -c ggmconn echo hello`
- `mcumgr -c ggmconn stat list`
- `mcumgr -c ggmconn stat smp_server_stats`
- `mcumgr -c ggmconn image list`
- `mcumgr-client -d /dev/ttyUSB0 list`
- `west sign -t imgtool -- --key ../privkeys/root-rsa-2048-01.pem`
- `mcumgr -c ggmconn image upload -e build/zdfu/zephyr/zephyr.signed.bin`
- `mcumgr-client -s 1 -m 512 -l 128 -d /dev/ttyUSB0 upload build/zdfu/zephyr/zephyr.signed.bin`
- `mcumgr -c ggmconn image test 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b`
- `mcumgr -c ggmconn image confirm ""`