/*
**
** Copyright 2008, The Android Open Source Project
** Copyright 2010, Samsung Electronics Co. LTD
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/*
**
** @author  Taikyung, Yu(taikyung.yu@samsung.com)
** @date    2011-07-06
*/

#ifndef SECTVOUTSERVICE_H
#define SECTVOUTSERVICE_H

#include <stdint.h>
#include <sys/types.h>
#include <binder/Parcel.h>
#include <utils/KeyedVector.h>

#include "ISecTVOut.h"
#include "sec_format.h"
#include "sec_utils.h"
#include "SecHdmi.h"

namespace android {
    class SecTVOutService : public BBinder
    {
        public :
            enum {
                HDMI_MODE_NONE = 0,
                HDMI_MODE_UI,
                HDMI_MODE_VIDEO,
            };

            mutable Mutex mLock;

            SecTVOutService();
            static int instantiate ();
            virtual status_t onTransact(uint32_t, const Parcel &, Parcel *, uint32_t);
            virtual ~SecTVOutService ();

            virtual void                        setHdmiStatus(uint32_t status);
            virtual void                        setHdmiMode(uint32_t mode);
            virtual void                        setHdmiResolution(uint32_t resolution);
            virtual void                        setHdmiHdcp(uint32_t enHdcp);
            virtual void                        blit2Hdmi(uint32_t w, uint32_t h,
                                                uint32_t colorFormat,uint32_t hdmiMode,
                                                uint32_t pPhyYAddr,uint32_t pPhyCAddr,
                                                uint32_t num_of_hwc_layer);
            bool                                hdmiCableInserted(void);
            void                                setLCDsize(void);

        private:
            SecHdmi                     mSecHdmi;
            bool                        mHdmiCableInserted;
            int                         mUILayerMode;
            uint32_t                    mLCD_width, mLCD_height;
    };
};
#endif
