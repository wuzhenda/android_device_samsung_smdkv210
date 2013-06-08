# These is the hardware-specific overlay, which points to the location
# of hardware-specific resource overrides, typically the frameworks and
# application settings that are stored in resourced.
DEVICE_PACKAGE_OVERLAYS := device/samsung/smdkv210/overlay

PRODUCT_COMMON_DIR := device/samsung/common/s5p


# Alan.Chu@BenQ.com, add android.hardware.wifi.xml for Wi-Fi Settings
PRODUCT_COPY_FILES := \
	frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	device/samsung/smdkv210/android.hardware.bluetooth.xml:system/etc/permissions/android.hardware.bluetooth.xml \
	device/samsung/smdkv210/bcm4329.hcd:system/etc/bluetooth/bcm4329.hcd \
	device/samsung/smdkv210/init.smdkv210.rc:root/init.smdkv210.rc \
	device/samsung/smdkv210/init.smdkv210.usb.rc:root/init.smdkv210.usb.rc \
	device/samsung/smdkv210/ueventd.smdkv210.rc:root/ueventd.smdkv210.rc \
	device/samsung/smdkv210/s3c-keypad.kl:system/usr/keylayout/s3c-keypad.kl \
        device/samsung/smdkv210/s3c-keypad.kcm:system/usr/keychars/s3c-keypad.kcm \
        device/samsung/smdkv210/s3c_ts.idc:system/usr/idc/s3c_ts.idc \
        device/samsung/smdkv210/busybox:system/bin/busybox \
	device/samsung/smdkv210/vold.fstab:system/etc/vold.fstab

# Alan.Chu@BenQ.com, add the property "wifi.interface", 
#                    ro.sf.lcd_density=160 for tablet
#                    ro.sf.lcd_density=240 or 320 for phone
PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	ro.sf.lcd_density=160

# Alan.Chu@BenQ.com
PRODUCT_CHARACTERISTICS := tablet

# Set default USB interface
PRODUCT_DEFAULT_PROPERTY_OVERRIDES += \
        persist.sys.usb.config=mass_storage

PRODUCT_PROPERTY_OVERRIDES += \
        hwui.render_dirty_regions=false \
	dalvik.vm.dexopt-data-only=1

#for otapackage
PRODUCT_COPY_FILES += \
	device/samsung/smdkv210/init.rc:root/init.rc \
	device/samsung/smdkv210/recovery.fstab:system/etc/recovery.fstab


PRODUCT_COPY_FILES += \
	device/samsung/smdkv210/MakeMirror.sh:MakeMirror.sh 

PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PACKAGES += \
	gralloc.smdkv210 

#audio
PRODUCT_PACKAGES += \
        audio_policy.smdkv210 \
        audio.primary.smdkv210 \
        audio.a2dp.default \
        lights.smdkv210 \
        hwcomposer.smdkv210 \
        libaudioutils

#wifi
PRODUCT_PACKAGES += \
	dhd.ko \
	nvram.txt \
	fw_bcm4329_sta.bin \
	fw_bcm4329_ap.bin 

#hdmi 
PRODUCT_PACKAGES += \
	libhdmiservice_jni \
	libhdmiclient 

#flashevn
PRODUCT_PACKAGES += \
	flashenv \
	libflashenv 

# These is the OpenMAX IL configuration files
PRODUCT_COPY_FILES += \
	$(PRODUCT_COMMON_DIR)/sec_mm/sec_omx/sec_omx_core/secomxregistry:system/etc/secomxregistry \
	$(PRODUCT_COMMON_DIR)/media_profiles.xml:system/etc/media_profiles.xml

#MFC Firmware
PRODUCT_COPY_FILES += \
        $(PRODUCT_COMMON_DIR)/samsung_mfc_fw.bin:system/vendor/firmware/samsung_mfc_fw.bin


# These are the OpenMAX IL modules
PRODUCT_PACKAGES += \
        libSEC_OMX_Core \
        libOMX.SEC.AVC.Decoder \
        libOMX.SEC.M4V.Decoder \
        libOMX.SEC.M4V.Encoder \
        libOMX.SEC.AVC.Encoder

# Misc other modules
PRODUCT_PACKAGES += \
	sensors.smdkv210
#Brian@BenQ 20110607 add sensor HW

# Include libstagefright module
PRODUCT_PACKAGES += \
	libstagefrighthw
# Camera
PRODUCT_PACKAGES += \
	camera.smdkv210

# Filesystem management tools
PRODUCT_PACKAGES += \
	make_ext4fs \
	setup_fs

#$(call inherit-product, frameworks/base/build/phone-xhdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, vendor/samsung/smdkv210/device-vendor.mk)


 
ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL_800_600 := device/samsung/smdkv210/prebuilt/kernel_800
LOCAL_KERNEL_1280_1080 := device/samsung/smdkv210/prebuilt/kernel_1080
LOCAL_KERNEL_1280_720 := device/samsung/smdkv210/prebuilt/kernel_1280_720
endif

 
PRODUCT_COPY_FILES += \
 $(LOCAL_KERNEL_800_600):zImage_800_600 \
 $(LOCAL_KERNEL_1280_1080):zImage_1280_1080 \
 $(LOCAL_KERNEL_1280_720):zImage_1280_720 



ifeq ($(TARGET_R70_KERNEL_800_600),true)
  LOCAL_KERNEL := $(LOCAL_KERNEL_800_600)
else ifeq ($(TARGET_R70_KERNEL_1280_720),true)
  LOCAL_KERNEL := $(LOCAL_KERNEL_1280_720)
else
  LOCAL_KERNEL := $(LOCAL_KERNEL_1280_1080)
endif

 
  

PRODUCT_COPY_FILES += \
    $(LOCAL_KERNEL):kernel   \
    $(LOCAL_KERNEL):zImage  \
    device/samsung/smdkv210/prebuilt/cmd.txt:cmd.txt  \
    device/samsung/smdkv210/prebuilt/logo.img:logo.img  \
    device/samsung/smdkv210/prebuilt/ramdisk-uboot.img:ramdisk-uboot.img  \
    device/samsung/smdkv210/prebuilt/sdfuse.txt:sdfuse.txt  \
    device/samsung/smdkv210/prebuilt/u-boot.bin:u-boot.bin  

#    device/samsung/smdkv210/prebuilt/recovery.img:recovery.img  

# Ray.KC.Liu for Phone Control usage
PRODUCT_COPY_FILES += \
	device/samsung/smdkv210/Uinput_For_Phone_Control.kl:system/usr/keylayout/Uinput_For_Phone_Control.kl
