# Power Distribution Board Firware Project

Firmware for PDB

## Build Instructions

### Using Platformio IO VSCode Extension

Run builds against desired enviroments using extension

### Using Docker

Build toolchain container

```bash
make docker-build
```

Build firmware

```
make clean build
```

Upload firmware

1. Plug micro usb into Teensy 4.1 board

``` 
make upload
```