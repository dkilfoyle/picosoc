# picosoc with WS2812 support

## Installation

1. Install Apio for Windows
2. Install RISCV GCC in WSL
3. To build and upload picocsoc

```
cd hdl
apio upload
```

4. To build firmware (in WSL in project root directory)

```
make firmware
```

5. To upload firmware (in Windows cmd prompt)

```
cd bin
uploadfirmware
```

## Sources

1. https://github.com/mattvenn/TinyFPGA-BX/tree/ws2812/examples/picosoc
2. https://github.com/sam210723/fpga/tree/master/icesugar/picosoc
3. https://github.com/YosysHQ/picorv32/tree/master/picosoc
