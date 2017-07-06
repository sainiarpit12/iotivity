/* ****************************************************************
 *
 * Copyright 2017 Samsung Electronics All Rights Reserved.
 *
 *
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
 *
 ******************************************************************/

/**
 * This file contains the declaration of a class and its members related to AirConditionerDevice.
 */

#ifndef SMARTHOME_API_SERVER_AIRCONDITIONER_H_
#define SMARTHOME_API_SERVER_AIRCONDITIONER_H_

#include <SHBaseDevice.h>
#include <BinarySwitchResource.h>
#include <TemperatureResource.h>

namespace OIC
{
    namespace Service
    {
        namespace SH
        {
            /**
             * This class contains a set of functions to define device and
             * handle client requests.
             */
            class AirConditionerDevice: public SHBaseDevice
            {
            public:
                AirConditionerDevice();
                virtual ~AirConditionerDevice();

                BinarySwitchResource m_binarySwitch;
                TemperatureResource m_temperature;
            };
        }
    }
}
#endif /* SMARTHOME_API_SERVER_AIRCONDITIONER_H_ */