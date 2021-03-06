# Copyright (C) 2008 The Android Open Source Project
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

ifeq ($(S5P_BOARD_USES_HDMI),true)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_PRELINK_MODULE := false
#LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := libutils liblog libedid libcec
LOCAL_SRC_FILES := SecHdmi.cpp
LOCAL_C_INCLUDES := \
	$(PRODUCT_COMMON_DIR)/include \
	$(PRODUCT_COMMON_DIR)/libfimc \
	$(PRODUCT_COMMON_DIR)/libg2d

LOCAL_CFLAGS  += \
        -DSCREEN_WIDTH=$(SCREEN_WIDTH) \
        -DSCREEN_HEIGHT=$(SCREEN_HEIGHT) \
	-DDEFAULT_FB_NUM=$(DEFAULT_FB_NUM) 


LOCAL_SHARED_LIBRARIES += libfimc libg2d

ifeq ($(S5P_BOARD_USES_HDMI_SUBTITLES),true)
LOCAL_CFLAGS += -DS5P_BOARD_USES_HDMI_SUBTITLES
endif

ifeq ($(BOARD_HDMI_STD),STD_480P)
LOCAL_CFLAGS  += -DSTD_480P
endif

ifeq ($(BOARD_HDMI_STD),STD_720P)
LOCAL_CFLAGS  += -DSTD_720P
endif

ifeq ($(BOARD_HDMI_STD),STD_1080P)
LOCAL_CFLAGS  += -DSTD_1080P
endif

ifeq ($(TARGET_R70_HDMI_720P),true)
LOCAL_CFLAGS  +=-DTARGET_HDMI_720P
endif

LOCAL_MODULE := libhdmi
include $(BUILD_SHARED_LIBRARY)

#
# libTVOut
#

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES := \
	SecTVOutService.cpp \
	ISecTVOut.cpp

LOCAL_C_INCLUDES := \

LOCAL_SHARED_LIBRARIES := \
	libbinder \
	libutils

LOCAL_C_INCLUDES += $(PRODUCT_COMMON_DIR)/include
LOCAL_C_INCLUDES += $(PRODUCT_COMMON_DIR)/libhdmi
LOCAL_C_INCLUDES += $(PRODUCT_COMMON_DIR)/libfimc
LOCAL_SHARED_LIBRARIES += libhdmi libfimc

LOCAL_MODULE := libTVOut

include $(BUILD_SHARED_LIBRARY)

#
# libhdmiclient
#

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_PRELINK_MODULE := false

LOCAL_SRC_FILES:= SecHdmiClient.cpp

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libbinder \
	libsurfaceflinger_client \
	libui \
	libTVOut

ifeq ($(TARGET_SIMULATOR),true)
ifeq ($(TARGET_OS),linux)
ifeq ($(TARGET_ARCH),x86)
LOCAL_LDLIBS += -lpthread -ldl -lrt
endif
endif
endif

LOCAL_CFLAGS += -DS5P_BOARD_USES_HDMI

ifeq ($(WITH_MALLOC_LEAK_CHECK),true)
	LOCAL_CFLAGS += -DMALLOC_LEAK_CHECK
endif

LOCAL_MODULE := libhdmiclient

include $(BUILD_SHARED_LIBRARY)

endif
