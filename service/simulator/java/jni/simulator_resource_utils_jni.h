/******************************************************************
 *
 * Copyright 2015 Samsung Electronics All Rights Reserved.
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

#ifndef SIMULATOR_RESOURCE_UTILS_JNI_H_
#define SIMULATOR_RESOURCE_UTILS_JNI_H_

#include <jni.h>
#include "simulator_single_resource.h"
#include "simulator_collection_resource.h"
#include "simulator_remote_resource.h"

/**
 * API to create the single resource
 *
 * @return resource object
 */
jobject CreateSingleResource(JNIEnv *env, SimulatorResourceSP singleResource);

/**
 * API to create collection resource
 *
 * @return resource object
 */
jobject CreateCollectionResource(JNIEnv *env, SimulatorResourceSP collectionResource);

/**
 * API to create simulator resource
 *
 * @return resource object
 */
jobject CreateSimulatorResource(JNIEnv *env, SimulatorResourceSP resource);

/**
 * API to create simulator resource vector
 *
 * @return vector object
 */
jobject CreateSimulatorResourceVector(JNIEnv *env, std::vector<SimulatorResourceSP> &resources);

/**
 * API to create simulator remote resource
 *
 * @return resource object
 */
jobject CreateSimulatorRemoteResource(JNIEnv *env, SimulatorRemoteResourceSP &remoteResource);

#endif
