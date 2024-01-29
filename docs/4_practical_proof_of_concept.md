# Practical proof of concept

## References:
- [Zephyr mcumgr documentation](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html)

- [Zephyr mcumgr example](https://docs.zephyrproject.org/latest/samples/subsys/mgmt/mcumgr/smp_svr/README.html)

- [mcuboot information about image signing](https://docs.mcuboot.com/readme-zephyr.html)

## Procedure
- Now it's time to put it all together in a minimal working example

- Started the project as a combination between the `blinky` sample and the `subsys/mgmt/mcumgr/smp_svr` sample

- This example will use the following capabilities of the mcumgr:

    - [Image management](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html#image-management)

    - [Statistics management](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html#image-management)

- The plan was to produce 2 firmwares (binaries) and swap between them via device firmware update (mcuboot+mcumgr):

    - "Binary One", which blinks the LED0 (DTC alias) at 1Hz

    - "Binary Two" which blinks the LED0 (DTC alias) at 10Hz

- The final sample source code can be seen in `main.cpp`

- The differentiation between "Binary One" and "Binary Two" is done via compilation symbols (`#define` directives) in code

- Project configuration wise, started by copying the main KConfig configurations from the SMP Server sample in order to make sure that I have properly setup the SMP server in my embedded device

### Example with `stm32f4_disco`
- Erase device's flash memory before starting this tutorial just to make sure it is always starting from the same point
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ sudo apt install stlink-tools
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ st-flash --connect-under-reset erase
st-flash 1.7.0
2024-01-27T19:09:25 INFO common.c: F4xx: 192 KiB SRAM, 1024 KiB flash in at least 16 KiB pages.
Mass erasing................
```

- For different chips/manufacturers/providers you'll probably need a different tool other than `stlink-tools` to perform a mass erase of flash memory

- Project can then be compiled and flashed to the embedded target device using `sysbuild` by doing:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ ./bbuild.sh -f -r -l --board stm32f4_disco

# Which is equivalent to
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ rm -rf build
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ west build --board=stm32f4_disco --pristine --sysbuild -- -DSB_CONFIG_BOOT_SIGNATURE_KEY_FILE=\""$(pwd)/../privkeys/root-rsa-2048-01.pem"\"
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ west flash
```

- After this is done, the device should be running (mcuboot + "Binary One") and it's LED should be blinking at 1Hz

- Also, if you open a serial port at the Zephyr's console UART and reset the embedded target, it's possible to see mcuboot logs:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ sudo picocom /dev/ttyUSB0 --baud 115200
Terminal ready
*** Booting Zephyr OS build zephyr-v3.5.0 ***
I: Starting bootloader
I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Boot source: primary slot
I: Image index: 0, Swap type: none
I: Bootloader chainload address offset: 0x40000
I: Jumping to the first image slot
*** Booting Zephyr OS build zephyr-v3.5.0 ***
```

- Having installed the SMP CLI/Client tools as described in the `mcumgr.md` document, the next step is to verify the following:

    - The installed SMP client tools are working

    - The embedded target device is in fact running an SMP server

- To do that, perform some of the following basic mcumgr commands:

1. Create a "conn device configuration" so that I do not have to specify it all the time when using the mcumgr CLI tool written in Go:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr conn add ggmconn type="serial" connstring="dev=/dev/ttyUSB0,baud=115200,mtu=256"
```

2. Testing a simple echo
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn echo hello
hello
```

3. Testing SMP Statistics management capabilities using the mcumgr CLI tool written in Go:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn stat list
stat groups:
    smp_server_stats
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn stat smp_server_stats
stat group: smp_server_stats
       553 counter_stat
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn stat smp_server_stats
stat group: smp_server_stats
       559 counter_stat
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn stat smp_server_stats
stat group: smp_server_stats
       560 counter_stat
```

4. Testing SMP Image management capabilities using the mcumgr CLI tool written in Go:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image list
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 221eb36a58157210701cc51d8959c66c8335aece2b0bbe2c8fe2977c754bed2e
Split status: N/A (0)
```

5. Testing SMP Image management capabilities using the mcumgr-client written in Rust:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr-client -d /dev/ttyUSB0 list
mcumgr-client 0.0.3, Copyright © 2023 Vouch.io LLC

22:32:38 [INFO] send image list request
22:32:38 [INFO] response: {
  "images": [
    {
      "hash": [
        34,
        30,
        179,
        106,
        88,
        21,
        114,
        16,
        112,
        28,
        197,
        29,
        137,
        89,
        198,
        108,
        131,
        53,
        174,
        206,
        43,
        11,
        190,
        44,
        143,
        226,
        151,
        124,
        117,
        75,
        237,
        46
      ],
      "slot": 0,
      "active": true,
      "pending": false,
      "version": "0.0.0",
      "bootable": true,
      "confirmed": true,
      "permanent": false
    }
  ],
  "splitStatus": 0
}
```

- In the output of the previous commands it is possible to see the `flags` fields which is relevant to **mcuboot**

- Now, change the compilation symbol (`#define` directives) in code in order to produce "Binary Two" (which blinks the LED0 (DTC alias) at 10Hz) and recompile the code (do not use `west` to flash it this time, since we'll perform a firmware update via SMP)
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ ./bbuild.sh -f -r --board stm32f4_disco

# Which is equivalent to
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ rm -rf build
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ west build --board=stm32f4_disco --pristine --sysbuild -- -DSB_CONFIG_BOOT_SIGNATURE_KEY_FILE=\""$(pwd)/../privkeys/root-rsa-2048-01.pem"\"
```

- If the produced binary is not already signed, sign it:
```bash
# Already signed:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ find build -iname "*sign*"
build/zdfu/zephyr/zephyr.signed.bin
build/zdfu/zephyr/zephyr.signed.hex

# Manually signing it would be something like this:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ west sign -t imgtool -- --key ../privkeys/root-rsa-2048-01.pem
```

- Transfer the binary to the target via SMP protocol using one of the two methods:
```bash
# Option 1: Transfer the binary to the target using the mcumgr CLI tool written in Go:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image upload -e build/zdfu/zephyr/zephyr.signed.bin
 48.43 KiB / 48.43 KiB [=============================================================================================================================================================================] 100.00% 3.01 KiB/s 16s
Done


# Option 2: Transfer the binary to the target using the mcumgr-client written in Rust:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr-client -s 1 -m 512 -l 128 -d /dev/ttyUSB0 upload build/zdfu/zephyr/zephyr.signed.bin
mcumgr-client 0.0.3, Copyright © 2023 Vouch.io LLC

22:41:54 [INFO] upload file: build/zdfu/zephyr/zephyr.signed.bin
22:41:54 [INFO] flashing to slot 1
22:41:54 [INFO] 49588 bytes to transfer
  [00:00:07] [=====================================================================================================================================================================================] 48.43 KiB/48.43 KiB (0s)22:42:02 [INFO] upload took 8s
```

- Sometimes the process might fail. In that case start it over:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr-client -s 1 -m 512 -l 128 -d /dev/ttyUSB0 upload build/zdfu/zephyr/zephyr.signed.bin
mcumgr-client 0.0.3, Copyright © 2023 Vouch.io LLC

22:41:47 [INFO] upload file: build/zdfu/zephyr/zephyr.signed.bin
22:41:47 [INFO] flashing to slot 1
22:41:47 [INFO] 49588 bytes to transfer
22:41:52 [ERROR] Error: read error, expected: 6, read: 91
```

- Check that the second image is now present on the other slot of target's flash
```bash
# Using the mcumgr CLI tool written in Go:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image list
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 221eb36a58157210701cc51d8959c66c8335aece2b0bbe2c8fe2977c754bed2e
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: 
    hash: 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b
Split status: N/A (0)

# using the mcumgr-client written in Rust:
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr-client -d /dev/ttyUSB0 list
mcumgr-client 0.0.3, Copyright © 2023 Vouch.io LLC

22:43:16 [INFO] send image list request
22:43:16 [INFO] response: {
  "images": [
    {
      "hash": [
        34,
        30,
        179,
        106,
        88,
        21,
        114,
        16,
        112,
        28,
        197,
        29,
        137,
        89,
        198,
        108,
        131,
        53,
        174,
        206,
        43,
        11,
        190,
        44,
        143,
        226,
        151,
        124,
        117,
        75,
        237,
        46
      ],
      "slot": 0,
      "active": true,
      "pending": false,
      "version": "0.0.0",
      "bootable": true,
      "confirmed": true,
      "permanent": false
    },
    {
      "hash": [
        94,
        249,
        73,
        74,
        43,
        178,
        154,
        0,
        207,
        143,
        139,
        236,
        158,
        39,
        105,
        131,
        110,
        109,
        172,
        130,
        82,
        16,
        35,
        103,
        66,
        38,
        155,
        88,
        199,
        18,
        164,
        75
      ],
      "slot": 1,
      "active": false,
      "pending": false,
      "version": "0.0.0",
      "bootable": true,
      "confirmed": false,
      "permanent": false
    }
  ],
  "splitStatus": 0
}
```

- Indicate to mcuboot to perform the image SWAP on the next boot and restart the device (notice the change on the "flags" field of the image on slot=1)
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image test 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 221eb36a58157210701cc51d8959c66c8335aece2b0bbe2c8fe2977c754bed2e
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: pending
    hash: 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b
Split status: N/A (0)
```

- After the reboot wait a little bit for mcuboot to perform the image swap and then the "Binary Two" should be running!!

- If you open a terminal before restarting the device, it should be possible to see mcuboot logs:
```bash
*** Booting Zephyr OS build zephyr-v3.5.0 ***
I: Starting bootloader
I: Primary image: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Boot source: primary slot
I: Image index: 0, Swap type: test
I: Starting swap using scratch algorithm.
I: Bootloader chainload address offset: 0x40000
I: Jumping to the first image slot
*** Booting Zephyr OS build zephyr-v3.5.0 ***
```

- The image status should be like this (it's possible to see that the images were swapped!)
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image list
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active
    hash: 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: confirmed
    hash: 221eb36a58157210701cc51d8959c66c8335aece2b0bbe2c8fe2977c754bed2e
Split status: N/A (0)
```

- At this point, if you restart the target again, it'll go back to "Binary One"

- In order for it to keep using the "Binary Two", you should "confirm" that the "Binary Two" binary is "OK", indicating to the bootloader that is should NOT perform a rollack in the next reboot

- To swap and confirm the current image: (notice the change in the flags field)
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image confirm ""
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 5ef9494a2bb29a00cf8f8bec9e2769836e6dac825210236742269b58c712a44b
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: 
    hash: 221eb36a58157210701cc51d8959c66c8335aece2b0bbe2c8fe2977c754bed2e
Split status: N/A (0)
```

### Example with `esp32_devkitc_wroom`
- Once the embedded target device is flashed and running the "Binary One", it is possible to build "Binary Two" and send it to the device via SMP protocol, using serial port:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr-client -s 1 -m 512 -l 128 -d /dev/ttyUSB0 upload build/zdfu/zephyr/zephyr.signed.bin
mcumgr-client 0.0.3, Copyright © 2023 Vouch.io LLC

01:03:34 [INFO] upload file: build/zdfu/zephyr/zephyr.signed.bin
01:03:34 [INFO] flashing to slot 1
01:03:34 [INFO] 100104 bytes to transfer
  [00:00:14] [=====================================================================================================================================================================================] 97.76 KiB/97.76 KiB (0s)01:03:48 [INFO] upload took 14s
```
- Listing the 2 binaries available in the embedded target device memory
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image list
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 29015f41f5c8311aefaf1faf97ae34f2bb1c7c290cf58c4933288b6045b0fa8a
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: 
    hash: 3ce9b5f10dd9543f22958bbaf744e1257475b5f3fac168d02d7cd4957801a0e5
Split status: N/A (0)
```

- Requesting a test switch:
```bash
(.venv) ggm@gAN515-52:~/zephyrproject/zdfu (master)$ mcumgr -c ggmconn image test 3ce9b5f10dd9543f22958bbaf744e1257475b5f3fac168d02d7cd4957801a0e5
Images:
 image=0 slot=0
    version: 0.0.0
    bootable: true
    flags: active confirmed
    hash: 29015f41f5c8311aefaf1faf97ae34f2bb1c7c290cf58c4933288b6045b0fa8a
 image=0 slot=1
    version: 0.0.0
    bootable: true
    flags: pending
    hash: 3ce9b5f10dd9543f22958bbaf744e1257475b5f3fac168d02d7cd4957801a0e5
Split status: N/A (0)
```

- At this stage, the embedded target device reboots and boots running "Binary Two", but when I list the images available, I expected to see 2 images, but rather I just see a single image present on memory

- Right now it is not clear yet why this is happening, but there are some suspects:

    - For some reason mcuboot boots as a secondary bootloader in this case, which might be related to this situation

    - Needs further investigation

    - Once this is figured out, next step is to make it work via bluetooth

```bash
[esp32] [INF] MCUboot 2nd stage bootloader
[esp32] [INF] compiled at Jan 28 2024 22:01:10
[esp32] [INF] Chip revision: 3
[esp32] [INF] Chip revision: 3, min. bootloader chip revision: 0
[esp32] [INF] SPI Flash speed: 40MHz, mode: DIO, size: 4MB
[esp32] [INF] Image index: 0, Swap type: test
[esp32] [INF] Loading image 0 - slot 0 from flash, area id: 1
[esp32] [INF] Application start=4008539Ch
[esp32] [INF] DRAM segment: paddr=00001F14h, vaddr=3FFB0000h, size=00C3Ch (  3132) load
[esp32] [INF] IRAM segment: paddr=00002B50h, vaddr=40080000h, size=05C28h ( 23592) load
[esp32] [INF] DROM segment: paddr=00010040h, vaddr=3F400040h, size=01ED4h (  7892) map
[esp32] [INF] IROM segment: paddr=00020000h, vaddr=400D0000h, size=086E0h ( 34528) map

*** Booting Zephyr OS build zephyr-v3.5.0 ***
```