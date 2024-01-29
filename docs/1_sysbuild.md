# Zephyr sysbuild

## References
- [Sample with MCUboot](https://docs.zephyrproject.org/latest/samples/application_development/sysbuild/with_mcuboot/README.html)

- [Sysbuild (System build)](https://docs.zephyrproject.org/latest/build/sysbuild/index.html)

## Concepts
- Higher level build system used to combine multiple other build systems together in a hierarchical build system

- Example:

    - It is possible to use `sysbuild` to build a Zephyr application + the mcuboot bootloader as well as flash both of them onto your device, and debug...

- `sysbuild` works by configuring and building at least one Zephyr application and, optionally, as many additional projects as required

- Building with `sysbuild` can be done via `west`

    - `west build -b reel_board --sysbuild samples/hello_world`

- Or via `CMake`
```bash
cmake -Bbuild -GNinja -DBOARD=reel_board -DAPP_DIR=samples/hello_world share/sysbuild
ninja -Cbuild
```

- It is also possible to configure west build to use `sysbuild` by default if desired

- Namespacing configuration:

    - When building a single Zephyr app, the `CMake` settings and/or `KConfig` build options are handled by Zephyr build system

    - When using `sysbuild` and combining multiple Zephyr build systems, there could be settings exclusive to sysbuild and not used by any of the applications

    - namespaces can be used to specify which variables are to be 'sent' to one of the multiple possible Zephyr build systems and which to maintain only at a sysbuild level 

    - Example:
    ```bash
    west build -b reel_board --sysbuild samples/hello_world -- -DSB_CONFIG_BOOTLOADER_MCUBOOT=y -DCONFIG_DEBUG_OPTIMIZATIONS=y -Dmcuboot_DEBUG_OPTIMIZATIONS=y
    ```

    - This will add mcuboot as the bootloader at a `sysbuild` level (see "-DSB_xxx") whereas it'll add debug optimizations to both images - application & boot (see there's only "-Dxxx")

- CMake variable namespacing:

    - It's concept is similar to the previous one, but in this case it allows for the creation of namespaces and variables can be defined within that specific namespace

    - Example if you have a `sysbuild` env that is composed of 2 different independent projects and you want to define a compilation symbol on one of those projects, using the concept of namespaces it is possible to do so by following the pattern `-D<namespace>_<var>=<value>`

    - Example:

        - `west build --sysbuild ... -- -Dmy_sample_FOO=BAR`

- KConfig namespacing

    - Similar concept to the ones above, it's possible to specify a KConfig which is related at the `sysbuild` level or at one of the project's level with the pattern `-D<namespace>_CONFIG_<var>=<value>`

    - Example:

        - `west build --sysbuild ... -- -Dmy_sample_CONFIG_FOO=BAR`

## Sysbuild configuration

### sysbuild.conf
- It is possible to create a sysbuild.conf file to define Kconfig-like symbols that are relevant at a sysbuild level

### sysbuild folder
- It is possible to create a folder named `sysbuild` in the root of the project

- This folder might contain configurations that can be used during a build, targeting one specific image being built

- There are 2 approaches:

- **Approach 1:**
```bash
# Template
zdfu/sysbuild/<image_name>.overlay
zdfu/sysbuild/<image_name>.conf

# Example:
zdfu/sysbuild/mcuboot.overlay
zdfu/sysbuild/mcuboot.conf
```

- This approach will append configurations defined in `zdfu/sysbuild/**` with the original configurations defined in the image's root directory

- In the example of mcuboot given above, it will append the conf file defined in `zdfu/sysbuild/mcuboot.conf` with the one present on `zephyrproject/bootloader/mcuboot/boot/zephyr/prj.conf`

- This allows for a less invasive approach

- **Approach 2:**
```bash
# Template
zdfu/sysbuild/<image_name>/boards/<board>.overlay
zdfu/sysbuild/<image_name>/boards/<board>.conf
zdfu/sysbuild/<image_name>/prj.conf

# Example:
zdfu/sysbuild/
                └── mcuboot
                    ├── boards
                    │   ├── esp32_devkitc_wroom.conf
                    │   ├── esp32_devkitc_wroom.overlay
                    │   └── stm32f4_disco.overlay
                    └── prj.conf
```

- This approach will override the configurations defined in that image's original root directory

- In the example of mcuboot given above, it will stop using the configurations defined in `zephyrproject/bootloader/mcuboot/boot/zephyr/prj.conf` and `zephyrproject/bootloader/mcuboot/boot/zephyr/boards/**` and will only use the configurations defined within `zdfu/sysbuild/mcuboot/**`

- This allows user to take full control of the configuration of one of sysbuild's projects being built

- Unfortunately, because I want to support more then 1 board at the same time in the same project, the only approach viable seems to be **approach 2**.

    - Using approach 1 in my case works for `stm32f4_disco` but fails for `esp32_devkitc_wroom`

    - If there was a way to use approach 1 + specify it to take effect only for specific boards, that would be ideal. Something like `zdfu/sysbuild/mcuboot_stm32f4_disco.overlay`

    - Because this is not possible, using approach 2 is the way to go. The bad part is that I had to copy most part of the contents of the original mcuboot conf and overlay files
