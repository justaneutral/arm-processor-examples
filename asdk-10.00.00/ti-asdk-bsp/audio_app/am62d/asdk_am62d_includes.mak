
DEVICE = am62dx
PROFILE=release
AUDIO_TYPE = MULTI_CHANNEL
AVB_AUDIO = 1

CWD := $(abspath $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

# AWE Code Path
export AWE8_CORE_C7_PATH?=$(abspath ${CWD}/../../packages/dspc/c7x/AWECore)
export AWE8_CORE_A53_PATH?=$(abspath ${CWD}/../../packages/dspc/a53/AWECore)

# MCU PLUS SDK Path
export MCU_PLUS_SDK_PATH:=$(abspath ${CWD}/../../mcu_plus_sdk/${DEVICE})

export MATHLIB_PATH:=$(abspath ${MCU_PLUS_SDK_PATH}/source/mathlib)
export DSPLIB_C7_PATH:=$(abspath ${CWD}/../../packages/ti/dsplib)
