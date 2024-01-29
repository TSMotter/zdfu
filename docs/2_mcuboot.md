# mcuboot

## References
- [mcuboot and zephyr configuration](https://docs.mcuboot.com/readme-zephyr.html)

- [mcuboot design](https://docs.mcuboot.com/design.html)

## Concepts
- mcuboot is a secure bootloader for 32-bit microcontrollers

- It is in mcuboot's scope

    - To determine the standard image format and memory layout that will be used

    - To take decisions of whether to perform a swap, or a rollback, an image integrity check, etc.

- It is NOT in mcuboot's scope:

    - To define how the images will be managed

    - To define how the images will end up in the device's memory so that they can be swaped

### Requirements to build mcuboot within a Zephyr application:
- Make sure there are flash partitions defined in the board's device tree
```
boot_partition: for MCUboot itself
slot0_partition: the primary slot of Image 0
slot1_partition: the secondary slot of Image 0
scratch_partition: the scratch slot (if needed)
```

- Set `CONFIG_BOOTLOADER_MCUBOOT` KConfig in prj.conf

- The 2 image slots must be contiguous

- If mcuboot is used as stage1 bootloader, boot partition should be configured so that it is ran first after a reset

- [The mcuboot design page](https://docs.mcuboot.com/design.html) will describe in detail:

    - Bootloader design, high level operation and limitations

    - Image format

    - Flash layout and partitions dimensioning based on: memory sector sizes, flash wear and amount of desirable updates

    - Swap strategies and rollback capabilities

    - Boot swap types and how the bootloader decides which action to take upon reboot (image trailers)

    - Security capabilities

- I'll be using a [Swap using scratch](https://docs.mcuboot.com/design.html#image-swap-using-scratch) strategy


## Moving away from deafult keys
- In order to upgrade to an image (or even boot it, if MCUBOOT_VALIDATE_PRIMARY_SLOT is enabled), the images must be signed.

- mcuboot is distributes with example keys which must NOT be used for real applications

- It is possible to generate new key pair with default tools like `openssl` but it is also possible to use the tool provided by mcuboot, `imgtool.py`
```bash
(.venv) ggm@gAN515-52:~/zephyrproject $ find . -iname "*imgtool.py"
./bootloader/mcuboot/scripts/imgtool.py
(.venv) ggm@gAN515-52:~/zephyrproject/ (master)$ ./bootloader/mcuboot/scripts/imgtool.py keygen -k root-rsa-2048-01.pem -t rsa-2048
```

- Now you need to indicate to the build system where the custom key is located so that it can extract the public key and insert it into mcuboot binary

- To do this use the KConfig option `SB_CONFIG_BOOT_SIGNATURE_KEY_FILE="path/to/pem/file"` in `sysbuild.conf`

- After making this change it's possible to see the following log during the build process: `MCUBoot bootloader key file: /home/ggm/zephyrproject/zdfu/root-rsa-2048-01.pem`

- In a later stage, I managed to keep my keys in a private repository and define `SB_CONFIG_BOOT_SIGNATURE_KEY_FILE` in the `west build` command line

    - The command line being used to build can be seen in `bbuild.sh` script

- To work like this, clone the private repository at the same level as this repository.

- If you try to perform a DFU using an image signed with the wrong key, you'll see the following in the mcuboot terminal after a system reboot:
```bash
*** Booting Zephyr OS build zephyr-v3.5.0 ***
I: Starting bootloader
I: Primary image: magic=good, swap_type=0x2, copy_done=0x1, image_ok=0x1
I: Scratch: magic=unset, swap_type=0x1, copy_done=0x3, image_ok=0x3
I: Boot source: none
I: Image index: 0, Swap type: test
E: Image in the secondary slot is not valid!
I: Bootloader chainload address offset: 0x40000
I: Jumping to the first image slot
*** Booting Zephyr OS build zephyr-v3.5.0 ***
```