# mcumgr

## References
- [mcumgr official repository](https://github.com/apache/mynewt-mcumgr)

- [mcumgr in Zephyr](https://docs.zephyrproject.org/latest/services/device_mgmt/mcumgr.html)

- [SMP Zephyr Sample](https://docs.zephyrproject.org/latest/samples/subsys/mgmt/mcumgr/smp_svr/README.html)

## Concepts
- mcumgr is a management library for 32-bit MCUs.

- The goal of mcumgr is to define a common management infrastructure with pluggable transport and encoding components

- The management is based on the Simple Management Protocol (SMP)

- mcumgr provides definitions and handlers for some core commands like image management

- There is a [mcumgr CLI tool written in Go](https://github.com/apache/mynewt-mcumgr-cli) that allows to update an image over BLE to devices running an SMP server

- Zephyr tutorials are all based in the CLI tool written in Go, but at the same time, the documentation itself states that this tool is for evaluation only and has bugs so it should not be used in production environments

- Alternatively there is a [mcumgr client written in Rust](https://github.com/vouch-opensource/mcumgr-client/) as well that allows to update an image over a serial port to devices running an mcumgr server

- Both tools were installed in my experience in order to play with both of them:
```bash
# Install mcumgr CLI tool written in Go
ggm@gAN515-52:~ $ sudo apt install golang-go
ggm@gAN515-52:~ $ echo "PATH=$PATH:$HOME/go/bin" >> ~/.bashrc
ggm@gAN515-52:~ $ go install github.com/apache/mynewt-mcumgr-cli/mcumgr@latest
ggm@gAN515-52:~ $ which mcumgr
/home/ggm/go/bin/mcumgr

# Install mcumgr-client Rust tool (alternatively could install from source, compiling rust code)
ggm@gAN515-52:~ $ wget https://github.com/vouch-opensource/mcumgr-client/releases/download/v0.0.3/mcumgr-client-linux-x86.zip -P ~/Downloads/
ggm@gAN515-52:~ $ unzip ~/Downloads/mcumgr-client-linux-x86.zip -d ~/Downloads/
ggm@gAN515-52:~ $ sudo cp Downloads/mcumgr-client-linux-x86/mcumgr-client /usr/bin/
ggm@gAN515-52:~ $ which mcumgr-client 
/usr/bin/mcumgr-client
```