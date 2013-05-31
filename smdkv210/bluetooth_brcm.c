/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "bluedroid"

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <cutils/log.h>
#include <cutils/properties.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <bluedroid/bluetooth.h>

#ifndef HCI_DEV_ID
#define HCI_DEV_ID 0
#endif

//+++ Alan.Chu@BenQ.com, improve LPM, 4/1/2011
#define LPM_PATH "/proc/bluetooth/sleep/proto"
//--- Alan.Chu@BenQ.com, improve LPM, 4/1/2011

//+++ Alan.Chu@BenQ.com, for downloading patch ram, 3/18/2011
#define BRCM_PATCH_RAM_CMD \
 "/system/bin/brcm_patchram_plus \
 --patchram /system/etc/bluetooth/bcm4329.hcd \
 --baudrate 3000000 \
 --enable_lpm \
 /dev/s3c2410_serial1"

//#define HCID_START_DELAY_SEC   1 //3
#define HCID_START_DELAY_USEC 200000
//--- Alan.Chu@BenQ.com, 3/18/2011
#define HCID_STOP_DELAY_USEC 500000

#define MIN(x,y) (((x)<(y))?(x):(y))

#define LOGD  ALOGD
#define LOGW  ALOGW
#define LOGE  ALOGE
#define LOGI  ALOGI
#define LOGV  ALOGV


static int rfkill_id = -1;
static char *rfkill_state_path = NULL;

static int init_rfkill() {
    char path[64];
    char buf[16];
    int fd;
    int sz;
    int id;
    for (id = 0; ; id++) {
        snprintf(path, sizeof(path), "/sys/class/rfkill/rfkill%d/type", id);
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            LOGW("open(%s) failed: %s (%d)\n", path, strerror(errno), errno);
            return -1;
        }
        sz = read(fd, &buf, sizeof(buf));
        close(fd);
        if (sz >= 9 && memcmp(buf, "bluetooth", 9) == 0) {
            rfkill_id = id;
            break;
        }
    }

    asprintf(&rfkill_state_path, "/sys/class/rfkill/rfkill%d/state", rfkill_id);
    return 0;
}

static int check_bluetooth_power() {
    int sz;
    int fd = -1;
    int ret = -1;
    char buffer;

    if (rfkill_id == -1) {
        if (init_rfkill()) goto out;
    }

    fd = open(rfkill_state_path, O_RDONLY);
    if (fd < 0) {
        LOGE("open(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }
    sz = read(fd, &buffer, 1);
    if (sz != 1) {
        LOGE("read(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }

    switch (buffer) {
    case '1':
        ret = 1;
        break;
    case '0':
        ret = 0;
        break;
    }

out:
    if (fd >= 0) close(fd);
    return ret;
}

static int set_bluetooth_power(int on) {
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');

    if (rfkill_id == -1) {
        if (init_rfkill()) goto out;
    }

    fd = open(rfkill_state_path, O_WRONLY);
    if (fd < 0) {
        LOGE("open(%s) for write failed: %s (%d)", rfkill_state_path,
             strerror(errno), errno);
        goto out;
    }
    sz = write(fd, &buffer, 1);
    if (sz < 0) {
        LOGE("write(%s) failed: %s (%d)", rfkill_state_path, strerror(errno),
             errno);
        goto out;
    }
    ret = 0;

out:
    if (fd >= 0) close(fd);
    return ret;
}

//+++ Alan.Chu@BenQ.com, improve LPM, 4/1/2011
static int set_bluetooth_lpm(int on) {
    int sz;
    int fd = -1;
    int ret = -1;
    const char buffer = (on ? '1' : '0');

    LOGI("%s: %s",__FUNCTION__, (on ? "enable" : "disable"));

    fd = open(LPM_PATH, O_WRONLY);
    if (fd < 0) {
        LOGE("open(%s) for write failed: %s (%d)", LPM_PATH,
             strerror(errno), errno);
        goto out;
    }
    sz = write(fd, &buffer, 1);
    if (sz < 0) {
        LOGE("write(%s) failed: %s (%d)", LPM_PATH, strerror(errno),
             errno);
        goto out;
    }
    ret = 0;

out:
    if (fd >= 0) close(fd);
    return ret;
}
//--- Alan.Chu@BenQ.com, improve LPM, 4/1/2011
static inline int create_hci_sock() {
    int sk = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sk < 0) {
        LOGE("Failed to create bluetooth hci socket: %s (%d)",
             strerror(errno), errno);
    }
    return sk;
}

//+++ Alan.Chu@BenQ.com, handle patchram timeout, 3/28/2011
void expired_handle_patchram(int sig)
{
    LOGD("Patch RAM Timeout");
    system("/system/bin/logwrapper /system/bin/busybox killall -9 brcm_patchram_plus");
} 

int run_patch_ram() {
    int ret = -1;
    int attempt = 3;

    LOGI("%s: start",__FUNCTION__);
    signal(SIGALRM, expired_handle_patchram);
    do {
	if (set_bluetooth_power(1) < 0) break;
	usleep(200000);  // 200ms delay for BT power up ready

	alarm(10);
        ret = system(BRCM_PATCH_RAM_CMD);
	LOGD("brcm_pathcram ret=%d",ret);

        if(0 != ret) {
            if (set_bluetooth_power(0) < 0) break;
        }
    } while ((0 != ret) && --attempt );
 
    alarm(0);
    LOGI("%s: end",__FUNCTION__);
    return ret;
}
//--- Alan.Chu@BenQ.com, handle patchram timeout, 3/28/2011

//+++ Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011
void expired_handle_hciattach(int sig)
{
    LOGD("HCI attach Timeout");
    LOGI("Stop hciattach daemon");
    if (property_set("ctl.stop", "hciattach") < 0) {
        LOGE("Failed to stop hciattach");
        return;
    }

    /*
    (void)run_patch_ram();
    usleep(500000);

    if (property_set("ctl.start", "hciattach") < 0) {
        LOGE("Failed to start hciattach");
        return;
    }
    */
}
//--- Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011

int bt_enable() {
    LOGV(__FUNCTION__);

    int ret = -1;
    int hci_sock = -1;
    int attempt;

    LOGI("Starting %s",BRCM_PATCH_RAM_CMD);
    if ( 0 != run_patch_ram()) goto out;

    LOGI("Starting hciattach daemon");
    if (property_set("ctl.start", "hciattach") < 0) {
        LOGE("Failed to start hciattach");
        goto out;
    }

    // Try for 10 seconds, this can only succeed once hciattach has sent the
    // firmware and then turned on hci device via HCIUARTSETPROTO ioctl
//+++ Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011
    signal(SIGALRM, expired_handle_hciattach);
    alarm(10);
//--- Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011
    for (attempt = 1000; attempt > 0;  attempt--) {
        hci_sock = create_hci_sock();
        if (hci_sock < 0) goto out;

//+++ Alan.Chu@BenQ.com, 3/18/2011
        LOGI("Sending HCIDEVUP");
//--- Alan.Chu@BenQ.com, 3/18/2011
        if (!ioctl(hci_sock, HCIDEVUP, HCI_DEV_ID)) {
            break;
        }
        close(hci_sock);
        usleep(10000);  // 10 ms retry delay
    }
    
//+++ Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011
    alarm(0);
//--- Alan.Chu@BenQ.com, handle hciattach timeout, 3/28/2011
    if (attempt == 0) {
        LOGE("%s: Timeout waiting for HCI device to come up", __FUNCTION__);
        goto out;
    }

//+++ Alan.Chu@BenQ.com, improve LPM, 4/1/2011
    /* enable LPM */
    if ( set_bluetooth_lpm(1) < 0) goto out;
//--- Alan.Chu@BenQ.com, improve LPM, 4/1/2011

    LOGI("Starting bluetoothd deamon");
    if (property_set("ctl.start", "bluetoothd") < 0) {
        LOGE("Failed to start bluetoothd");
        goto out;
    }
    //sleep(HCID_START_DELAY_SEC);
    usleep(HCID_START_DELAY_USEC);

    ret = 0;

out:
    if (hci_sock >= 0) close(hci_sock);
    LOGI("%s: end",__FUNCTION__);
    return ret;
}

int bt_disable() {
    LOGV(__FUNCTION__);

    int ret = -1;
    int hci_sock = -1;

//+++ Alan.Chu@BenQ.com, improve LPM, 4/1/2011
    /* disable LPM */
    if ( set_bluetooth_lpm(0) < 0) goto out;
//--- Alan.Chu@BenQ.com, improve LPM, 4/1/2011
    LOGI("Stopping bluetoothd deamon");
    if (property_set("ctl.stop", "bluetoothd") < 0) {
        LOGE("Error stopping bluetoothd");
        goto out;
    }
    usleep(HCID_STOP_DELAY_USEC);

    hci_sock = create_hci_sock();
    if (hci_sock < 0) goto out;
    ioctl(hci_sock, HCIDEVDOWN, HCI_DEV_ID);

    LOGI("Stopping hciattach deamon");
    if (property_set("ctl.stop", "hciattach") < 0) {
        LOGE("Error stopping hciattach");
        goto out;
    }

    if (set_bluetooth_power(0) < 0) {
        goto out;
    }
    ret = 0;

out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int bt_is_enabled() {
    LOGV(__FUNCTION__);

    int hci_sock = -1;
    int ret = -1;
    struct hci_dev_info dev_info;


    // Check power first
    ret = check_bluetooth_power();
    if (ret == -1 || ret == 0) goto out;

    ret = -1;

    // Power is on, now check if the HCI interface is up
    hci_sock = create_hci_sock();
    if (hci_sock < 0) goto out;

    dev_info.dev_id = HCI_DEV_ID;
    if (ioctl(hci_sock, HCIGETDEVINFO, (void *)&dev_info) < 0) {
        ret = 0;
        goto out;
    }


out:
    if (hci_sock >= 0) close(hci_sock);
    return ret;
}

int ba2str(const bdaddr_t *ba, char *str) {
    return sprintf(str, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
                ba->b[5], ba->b[4], ba->b[3], ba->b[2], ba->b[1], ba->b[0]);
}

int str2ba(const char *str, bdaddr_t *ba) {
    int i;
    for (i = 5; i >= 0; i--) {
        ba->b[i] = (uint8_t) strtoul(str, &str, 16);
        str++;
    }
    return 0;
}
