/*  $Date: 2010/1/22 17:45:35 $
 *  $Revision: 1.0 $
 *  
 */

/*
* Copyright (C) 2010 Bosch Sensortec GmbH
*
* Android hardware module for sensor manager
* 
* Usage:	create a hardware module of sensor.default.so at /sys/lib/hw/
*

Disclaimer
*
* Common:
* This Work is developed for the consumer goods industry. It may only be used 
 * within the parameters of the respective valid product data sheet.  The Work 
 * provided with the express understanding that there is no warranty of 
fitness for a particular purpose. 
 * It is not fit for use in life-sustaining, safety or security sensitive 
systems or any system or device 
 * that may lead to bodily harm or property damage if the system or device 
malfunctions. In addition, 
 * the Work is not fit for use in products which interact with motor vehicle 
systems.  
 * The resale and/or use of the Work are at the purchaser’s own risk and his 
own responsibility. The 
 * examination of fitness for the intended use is the sole responsibility of 
the Purchaser. 
 *
 * The purchaser shall indemnify Bosch Sensortec from all third party claims, 
including any claims for 
 * incidental, or consequential damages, arising from any Work or Derivative 
Work use not covered by the parameters of 
 * the respective valid product data sheet or not approved by Bosch Sensortec 
and reimburse Bosch 
 * Sensortec for all costs in connection with such claims.
 *
 * The purchaser must monitor the market for the purchased Work and 
Derivative Works, particularly with regard to 
 * product safety and inform Bosch Sensortec without delay of all security 
relevant incidents.
 *
 * Engineering Samples are marked with an asterisk (*) or (e). Samples may 
vary from the valid 
 * technical specifications of the product series. They are therefore not 
intended or fit for resale to third 
 * parties or for use in end products. Their sole purpose is internal client 
testing. The testing of an 
 * engineering sample may in no way replace the testing of a product series. 
Bosch Sensortec 
 * assumes no liability for the use of engineering samples. By accepting the 
engineering samples, the 
 * Purchaser agrees to indemnify Bosch Sensortec from all claims arising from 
the use of engineering 
 * samples.
 *
 * Special:
 * This Work and any related information (hereinafter called "Information") 
is provided free of charge 
 * for the sole purpose to support your application work. The Woek and 
Information is subject to the 
 * following terms and conditions: 
 *
 * The Work is specifically designed for the exclusive use for Bosch 
Sensortec products by 
 * personnel who have special experience and training. Do not use this Work 
or Derivative Works if you do not have the 
 * proper experience or training. Do not use this Work or Derivative Works 
fot other products than Bosch Sensortec products.  
 *
 * The Information provided is believed to be accurate and reliable. Bosch 
Sensortec assumes no 
 * responsibility for the consequences of use of such Information nor for any 
infringement of patents or 
 * other rights of third parties which may result from its use. No license is 
granted by implication or 
 * otherwise under any patent or patent rights of Bosch. Specifications 
mentioned in the Information are 
 * subject to change without notice.
*/


/*! \file sensors.ppc
    \brief This file contains all operation for acclerometer
    
    Details.
*/


#define LOG_TAG "Sensor"

#include <hardware/hardware.h>
#include <hardware/sensors.h>

#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <time.h>

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <sys/system_properties.h>
#include <stdio.h>

#include <cutils/native_handle.h>

#define USEC_PER_SEC	1000000L

#define DEBUG_SENSOR

#define	LOGV  ALOGV
#define	LOGD  ALOGD
#define	LOGW  ALOGW
#define	LOGE  ALOGE
//Brian@BenQ 20110607 start

/*****************************************************************************/

#define SENSOR_DATA_DEVICE "/dev/bma150"
//#define SENSOR_DATA_DEVICE "/dev/bma120"
//#define SENSOR_DATA_DEVICE "/dev/bmp085"
//#define PRESSURER
#define ACCELEROMETER
//#define BMA120_SENSOR

#define BMA150_SENSOR
// Convert to Gs
// FIXME!!!! The raw data is suppose to be +-2G with 8bit fraction
// FIXME!!!! We are getting 7bit fraction
#define CONVERT                     (GRAVITY_EARTH / 256)
#define CONVERT_X                   -(CONVERT)
#define CONVERT_Y                   -(CONVERT)
#define CONVERT_Z                   (CONVERT)


#define S_HANDLE_ACCELEROMETER		(1<<SENSOR_TYPE_ACCELEROMETER)
#define S_HANDLE_PRESSURER 			(1<<SENSOR_TYPE_ACCELEROMETER)


static int s_timeout = 400;

struct sensors_poll_context_t {
	struct sensors_poll_device_t device;
	int fd;
};


static struct sensor_t sensors_list[] = {
#ifdef ACCELEROMETER
    {
        name: "BMA150 Acclerometer",
        vendor: "Bosch",
        version: 1,
        handle: S_HANDLE_ACCELEROMETER,
        type: SENSOR_TYPE_ACCELEROMETER,
        maxRange: 1.0,   /* ???? */
        resolution: 1.0, /* ???? */
        power: 20,      /* ???? */
        minDelay:5,
        { }
    },
#endif

};

static int sensors_list_size = sizeof(sensors_list) / sizeof(sensors_list[0]);

static int poll__activate (struct sensors_poll_device_t *device,
            int handle,
            int enabled)
{
    struct sensors_poll_context_t *dev;
    dev = (struct sensors_poll_context_t *)device;
#ifdef DEBUG_SENSOR
	LOGD("%s\n",__FUNCTION__);
#endif
    // ioctl here?
    return 0;
}

static int poll__setDelay (struct sensors_poll_device_t *device,int handle,
             int64_t ns)
{
    struct sensors_poll_context_t *dev;
    dev = (struct sensors_poll_context_t *)device;
    s_timeout = ns/1000000;
#ifdef DEBUG_SENSOR
	LOGD("%s:delay:%d\n",__FUNCTION__,s_timeout);
#endif
    // ioctl here?
    return 0;
}


static int poll__close (struct hw_device_t* device)
{
    struct sensors_poll_context_t *dev;
    dev = (struct sensors_poll_context_t *)device;
    // ioctl here to quiet device?
    close(dev->fd);
#ifdef DEBUG_SENSOR
	LOGD("%s\n",__FUNCTION__);
#endif
    free (dev);
    return 0;
}
void mSleep(int msec)
{
	fd_set rfds;
	struct timeval timeout;
	timeout.tv_sec=0;
	timeout.tv_usec=msec*1000;
	
	FD_ZERO(&rfds);
	FD_SET(0,&rfds);
	
	select (0,&rfds,NULL,NULL,&timeout);
}

int64_t getTimestamp() {
    struct timespec t;
    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return int64_t(t.tv_sec)*1000000000LL + t.tv_nsec;
}

static int poll__poll (struct sensors_poll_device_t* device,
              sensors_event_t* data, int count)
{
    struct sensors_poll_context_t *dev;
    int err;
    int nb = 1;
#ifdef DEBUG_SENSOR
	LOGD("%s data[0x%x],count[%d],sizeof(data[0])[%d],nb[%d]\n",__FUNCTION__,data,count,sizeof(data[0]),nb);
#endif
    dev = (struct sensors_poll_context_t *)device;
    if (dev->fd < 0)
        return 0;
/*
    pollfd pfd = {fd: dev->fd, events: POLLIN, revents: 0};

    int err = poll (&pfd, 1, s_timeout);

    if (err < 0)
        return err;
    if (err == 0)
        return -EWOULDBLOCK;
*/
    // FIXME!!!!!
    // The device currently always has data available and so
    // it can eat up the CPU!!!
    //usleep(s_timeout);
    
    if(s_timeout > sensors_list[SENSOR_TYPE_ACCELEROMETER-1].minDelay)
	usleep(s_timeout *1000);
	else
	usleep(sensors_list[SENSOR_TYPE_ACCELEROMETER-1].minDelay*1000);

#ifdef  ACCELEROMETER
#ifdef	BMA150_SENSOR
    struct {
        short x;
        short y;
        short z;
    } rawData;
#endif

#ifdef BMA120_SENSOR
	struct {
        signed char x;
        signed char y;
        signed char z;
    } rawData;
#endif
	//LOGD("before open\n");
while(nb-->0){
    err = read (dev->fd, &rawData, sizeof(rawData));
	//LOGD("after open:%d\n",err);
    if (err < 0)
        return err;

    data->timestamp = getTimestamp();
		//LOGD("after gettime\n");

    data->sensor = S_HANDLE_ACCELEROMETER;// Brian@BenQ 20110607 add ID
    data->type= SENSOR_TYPE_ACCELEROMETER;
    data->acceleration.status = SENSOR_STATUS_ACCURACY_HIGH;
//    data->acceleration.x = rawData.y * CONVERT_X;
//    data->acceleration.y = -rawData.x * CONVERT_Y;
//    data->acceleration.z = -rawData.z * CONVERT_Z;
    data->acceleration.x = rawData.x * CONVERT_X;
    data->acceleration.y = rawData.y * CONVERT_Y;
    data->acceleration.z = -rawData.z * CONVERT_Z;

#ifdef DEBUG_SENSOR
    LOGD("Sensor data: t x,y,z: %dms %f, %f, %f\n",
                (int)(data->timestamp / USEC_PER_SEC),
                data->acceleration.x,
                data->acceleration.y,
                data->acceleration.z);
#endif
	//data++;

}
	//LOGD("before return\n");

    return 1;//Brian@BenQ 20110607 return one count
#endif


}

/*****************************************************************************/

static int s_get_sensors_list (struct sensors_module_t* module,
                    struct sensor_t const** list)
{
    *list = sensors_list;
#ifdef DEBUG_SENSOR
	LOGD("%s\n",__FUNCTION__);
#endif
    return sensors_list_size;
}

/*****************************************************************************/

int s_device_open(const struct hw_module_t* module,
              const char* id,
              struct hw_device_t** device)
{
    int status = -EINVAL;
#ifdef DEBUG_SENSOR
	LOGD("%s\n",__FUNCTION__);
#endif
        if (!strcmp(id, SENSORS_HARDWARE_POLL)) {
        struct sensors_poll_context_t *dev;
        dev = (struct sensors_poll_context_t *)malloc(sizeof(*dev));
        if (! dev) return status;

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */

        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version  = 0;
        dev->device.common.module   = const_cast<hw_module_t*>(module);
        dev->device.common.close    = poll__close;
        dev->device.activate        = poll__activate;
        dev->device.setDelay        = poll__setDelay;
        dev->device.poll            = poll__poll;
        status = 0;
        dev->fd = open(SENSOR_DATA_DEVICE, O_RDONLY);
        ALOGE_IF(dev->fd<0, "Couldn't open %s (%s)", SENSOR_DATA_DEVICE, strerror(errno));
	 if(dev->fd < 0)
	 	status = -EINVAL;
        *device = &dev->device.common;
#ifdef DEBUG_SENSOR
	LOGD("%s:return status:%d\n",__FUNCTION__,status);
#endif
    }
		
    return status;
}


static struct hw_module_methods_t s_module_methods = {
    open: s_device_open
};

//extern "C" const struct sensors_module_t HAL_MODULE_INFO_SYM = {
struct sensors_module_t HAL_MODULE_INFO_SYM = {
   common: {
       tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: SENSORS_HARDWARE_MODULE_ID,
        name: "sensor module",
        author: "Bosch",
        methods: &s_module_methods,
    },
    get_sensors_list :s_get_sensors_list,
};
//Brian@BenQ 20110607 end
