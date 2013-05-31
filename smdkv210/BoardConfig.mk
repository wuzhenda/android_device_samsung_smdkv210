#
# Copyright (C) 2011 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# These two variables are set first, so they can be overridden
# by BoardConfigVendor.mk


BOARD_USES_GENERIC_AUDIO := false 
USE_CAMERA_STUB := false

TARGET_BOARD_PLATFORM := smdkv210
TARGET_NO_BOOTLOADER := true	#uses u-boot instead
TARGET_NO_KERNEL := false
TARGET_NO_RECOVERY := false
TARGET_NO_RADIOIMAGE := true
# TARGET_CPU_SMP := true
TARGET_ARCH_VARIANT := armv7-a-neon
ARCH_ARM_HAVE_TLS_REGISTER := true

BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true

TARGET_BOOTLOADER_BOARD_NAME := smdkv210

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi

BOARD_EGL_CFG := device/samsung/smdkv210/egl.cfg

USE_OPENGL_RENDERER := true

TARGET_USERIMAGES_USE_EXT4 := true
TARGET_USERIMAGES_SPARSE_EXT_DISABLED := true
#BOARD_SYSTEMIMAGE_PARTITION_SIZE := 268435456
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 268849152
#BOARD_USERDATAIMAGE_PARTITION_SIZE := 536870912
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1075396608
BOARD_FLASH_BLOCK_SIZE := 4096

#ifeq ($(BOARD_USES_GENERIC_AUDIO),false)
#BOARD_USES_ALSA_AUDIO := true
#BUILD_WITH_ALSA_UTILS := true
#BOARD_USES_CIRCULAR_BUF_AUDIO := true
#USE_ULP_AUDIO := true
#endif

ifeq ($(USE_CAMERA_STUB),false)
BOARD_CAMERA_LIBRARIES := libcamera
endif

S5P_BOARD_USES_HDMI := true
ifeq ($(S5P_BOARD_USES_HDMI),true)
BOARD_HDMI_STD := STD_1080P
S5P_BOARD_USES_HDMI_SUBTITLES := false
endif

DEFAULT_FB_NUM	:= 0


#+++ Alan.Chu@BenQ.com
# Wifi related defines
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
BOARD_WLAN_DEVICE := bcm4329
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_wext
WIFI_DRIVER_MODULE_NAME     :="dhd"
#WIFI_DRIVER_FW_PATH_PARAM   := "/sys/module/bcmdhd/parameters/firmware_path"
WIFI_DRIVER_MODULE_ARG      := "iface_name=wlan0 firmware_path=/system/etc/wifi/fw_bcm4329_sta.bin nvram_path=/system/etc/wifi/nvram.txt"
WIFI_DRIVER_MODULE_PATH     := "/system/etc/wifi/dhd.ko"
WIFI_DRIVER_FW_PATH_STA     := "/system/etc/wifi/fw_bcm4329_sta.bin"
WIFI_DRIVER_FW_PATH_AP      := "/system/etc/wifi/fw_bcm4329_ap.bin"
#--- Alan.Chu@BenQ.com

#BEGIN Added by Seac.Cong, for porting r70
TARGET_CUSTOM_BLUEDROID := ../../../device/samsung/smdkv210/bluetooth_brcm.c

#END Added by Seac.Cong, for porting r7

BOARD_KERNEL_CONFIG := R70_evt3_defconfig
BOARD_KERNEL_TYPE := zImage
#{Add by Henry.Wu,for usbdevice to PC
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/devices/platform/s3c-usbgadget/gadget/lun0/file"
BOARD_UMS_LUNFILE := "/sys/class/android_usb/android0/f_mass_storage/lun/file"
# Add by Henry.Wu,for usbdevice to PC}

TARGET_RELEASETOOL_OTA_FROM_TARGET_SCRIPT :=device/samsung/smdkv210/releasetools/ota_from_target_files
#add by Henry.Wu,for gen boot.img
BOARD_KERNEL_BASE :=0x20000000
BOARD_KERNEL_CMDLINE :=console=ttySAC0,115200 init=/init

#add by Henry.Wu,for support extention device
BOARD_VOLD_DISC_HAS_MULTIPLE_MAJORS := true
