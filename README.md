# xio-device-base-rp2040
Base repository for RP2040 based devices


## Requirements

- [CMake](https://cmake.org/download/)
- [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
  - You need the filename ending with -arm-none-eabi.exe - I used `arm-gnu-toolchain-12.2.mpacbti-bet1-mingw-w64-i686-arm-none-eabi`.
- [Raspberry Pico SDK](https://github.com/raspberrypi/pico-sdk.git)
  - Clone it to a `pico-sdk` directory in the parent directory of this repository

## Build Instructions (Windows 10)

### Using VSCode

You can run `env.bat` on the root, which defaults to the build directory.
Run CMake and then start VSCode at the root directory:

```
git submodule update --recursive --init
cmake -G "NMake Makefiles" ..
code ..
```

### Manual Instructions

```
git submodule update --recursive --init
mkdir build
cd build
cmake -G "NMake Makefiles" ..
nmake
```
