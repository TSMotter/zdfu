# Zephyr Device Firmware Update - zdfu
- This project describes the concepts as well as the practical steps of developing a proof of concept application to explore the device firmware update (dfu) capabilities supported by Zephyr RTOS
- This project uses:
    - **Zephyr** as platform
        ```bash
        (.venv) ggm@gAN515-52:~/zephyrproject/zephyr ((HEAD detached at v3.5.0))$ git log -n1 --oneline --decorate
        a6eef0ba37 (HEAD, tag: zephyr-v3.5.0, tag: v3.5.0) release: Zephyr 3.5.0 release
        ```
    - **mcuboot** as bootloader
    - **mcumgr** as implementation of the simple management protocol (SMP)
- Read more about each element of the system under the `docs` folder
- The practical steps taken to produce the minimal proof of concept example can be seen in `docs/4_practical_proof_of_concept.md`