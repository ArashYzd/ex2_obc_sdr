# ex2_obc_sdr
Software Defined Radio software for Ex-Alta2 on-board computer

## Build and Test on Linux

[Meson](https://mesonbuild.com/index.html) is use to manage build and test functions. The main steps are

```
meson setup build
cd build
ninja
meson test
    
## Build and Test for Hercules
