#
# Auto generated makefile
#
include ${CURDIR}/asdk_am62d_includes.mak
export MCU_PLUS_SDK_PATH:=$(abspath ../../mcu_plus_sdk)

ifeq ($(BUILD_AWE_MULTI),)
   $(info ---------------------------------)
   $(info Building for Single instance AWE)
   $(info ---------------------------------)
else
   $(info ---------------------------------)
   $(info Building for Multi-Instance AWE)
   $(info ---------------------------------)
endif

PROFILE?=release
DEVICE?=am62ax

all: 
	$(MAKE) -C a53/gcc-aarch64/ all
	$(MAKE) -C a53-smp/gcc-aarch64/ all
	$(MAKE) -C mcu-r5f/ti-arm-clang/ all
	$(MAKE) -C c75/ti-c7000/ all
	$(MAKE) -C r5f/ti-arm-clang/ all

debug:
	$(MAKE) -C a53/gcc-aarch64/ all PROFILE=debug
	$(MAKE) -C a53-smp/gcc-aarch64/ all PROFILE=debug
	$(MAKE) -C mcu-r5f/ti-arm-clang/ all PROFILE=debug
	$(MAKE) -C c75/ti-c7000/ all PROFILE=debug
	$(MAKE) -C r5f/ti-arm-clang/ all PROFILE=debug

clean:
	$(MAKE) -C a53/gcc-aarch64/ clean
	$(MAKE) -C a53-smp/gcc-aarch64/ clean
	$(MAKE) -C mcu-r5f/ti-arm-clang/ clean
	$(MAKE) -C c75/ti-c7000/ clean
	$(MAKE) -C r5f/ti-arm-clang/ clean

scrub:
	$(MAKE) -C a53/gcc-aarch64/ scrub
	$(MAKE) -C a53-smp/gcc-aarch64/ scrub
	$(MAKE) -C mcu-r5f/ti-arm-clang/ scrub
	$(MAKE) -C c75/ti-c7000/ scrub
	$(MAKE) -C r5f/ti-arm-clang/ scrub

