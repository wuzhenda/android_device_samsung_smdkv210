PRODUCT_PACKAGES := \
    Camera \
    VoiceDialer \
    GestureBuilder \
    librs_jni \
    libRS \
    LiveWallpapersPicker \
    LiveWallpapers \
    MagicSmokeWallpapers \
    VisualizationWallpapers \
    hwcomposer.default

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base.mk)
$(call inherit-product, device/samsung/smdkv210/device.mk)

PRODUCT_LOCALES := zh_TW zh_CN en_US ja_JP
# Overrides
PRODUCT_NAME := full_smdkv210
PRODUCT_DEVICE := smdkv210
PRODUCT_BRAND := Android
PRODUCT_MODEL := Full AOSP on SMDKV210

