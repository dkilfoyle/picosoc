wsl -e bash -lic "make clean"
wsl -e bash -lic "make firmware"
apio raw "icesprog -o 0x100000 bin/picosoc_firmware.bin"