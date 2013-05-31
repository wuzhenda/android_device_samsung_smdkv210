#WITH_SEC_OMX := true
WITH_SEC_OMX := false

ifeq ($(WITH_SEC_OMX), true)
  include $(all-subdir-makefiles)
endif
