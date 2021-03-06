//******************************************************************
//
// Copyright 2015 Samsung Electronics All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include "ResourceCacheManager.h"
#include "DataCache.h"
#include "RCSResourceAttributes.h"
#include "ResponseStatement.h"
#include "UnitTestHelper.h"

using namespace OIC::Service;

class DataCacheTest : public TestWithMock
{
    public:
        typedef std::function <
        void(const OIC::Service::HeaderOptions &, const OIC::Service::ResponseStatement &,
             int) > GetCallback;

        typedef std::function <
        void(const OIC::Service::HeaderOptions &, const OIC::Service::ResponseStatement &, int,
             int) > ObserveCallback;
    public:
        std::shared_ptr<DataCache> cacheHandler;
        CacheCB cb;
        CacheID id;
        static PrimitiveResource::Ptr pResource;
        static MockRepository mocks;
        static bool isLast;

    protected:
        DataCacheTest()
        {
            id = 0;
            static std::once_flag flag;
            std::call_once(flag, [this]()
            {
                isLast = false;
                auto deleter = [](PrimitiveResource *) { };
                pResource = PrimitiveResource::Ptr(mocks.Mock< PrimitiveResource >(), deleter);
            });
        }
        virtual ~DataCacheTest() noexcept(noexcept(std::declval<Test>().~Test()))
        {
            if (isLast)
            {
                mocks.reset();
            }
        }
        virtual void SetUp()
        {
            TestWithMock::SetUp();

            mocks.OnCall(pResource.get(), PrimitiveResource::isObservable).Return(false);
            cacheHandler.reset(new DataCache());
            cb = ([](std::shared_ptr<PrimitiveResource >,
                    const RCSResourceAttributes &, int) -> OCStackResult
                    {
                        return OC_STACK_OK;
                    });
        }

        virtual void TearDown()
        {
            cacheHandler.reset();
            TestWithMock::TearDown();
        }
};

PrimitiveResource::Ptr DataCacheTest::pResource = nullptr;
MockRepository DataCacheTest::mocks;
bool DataCacheTest::isLast;

TEST_F(DataCacheTest, initializeDataCache_normalCase)
{
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);
}

TEST_F(DataCacheTest, initializeDataCache_normalCaseObservable)
{

    mocks.OnCall(pResource.get(), PrimitiveResource::requestGet).Do(
        [](GetCallback callback)
    {
        OIC::Service::HeaderOptions hos;

        OIC::Service::RCSResourceAttributes attr;
        OIC::Service::ResponseStatement rep(attr);
        callback(hos, rep, OC_STACK_OK);
        return;
    }
    );
    mocks.OnCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.OnCall(pResource.get(), PrimitiveResource::requestObserve).Do(
        [](ObserveCallback callback)
    {
        OIC::Service::HeaderOptions hos;
        OIC::Service::RCSResourceAttributes attr;
        OIC::Service::ResponseStatement rep(attr);
        int seq = 0;
        callback(hos, rep, OC_STACK_OK, seq);
        return;
    }
    );
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);
}

TEST_F(DataCacheTest, initializeDataCache_normalCaseNonObservable)
{

    std::condition_variable responseCon;
    std::mutex mutexForCondition;

    mocks.OnCall(pResource.get(), PrimitiveResource::requestGet).Do(
        [& responseCon](GetCallback callback)
    {
        OIC::Service::HeaderOptions hos;

        OIC::Service::RCSResourceAttributes attr;
        OIC::Service::ResponseStatement rep(attr);
        callback(hos, rep, OC_STACK_OK);
        responseCon.notify_all();
    });
    mocks.OnCall(pResource.get(), PrimitiveResource::isObservable).Return(false);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    std::unique_lock< std::mutex > lock{ mutexForCondition };
    responseCon.wait_for(lock, std::chrono::milliseconds(1000));
}

TEST_F(DataCacheTest, initializeDataCache_normalCaseTimeOut)
{
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);
    cacheHandler->initializeDataCache(pResource);
    sleep(3);
}

TEST_F(DataCacheTest, addSubscriber_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);
    REPORT_FREQUENCY rf = REPORT_FREQUENCY::UPTODATE;
    long reportTime = 1l;

    ASSERT_NE(cacheHandler->addSubscriber(cb, rf, reportTime), 0);
}

TEST_F(DataCacheTest, deleteSubsciber_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    REPORT_FREQUENCY rf = REPORT_FREQUENCY::UPTODATE;
    long reportTime = 1l;

    id = cacheHandler->addSubscriber(cb, rf, reportTime);

    ASSERT_NE(cacheHandler->deleteSubscriber(id), 0);
}

TEST_F(DataCacheTest, getCacheState_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    ASSERT_EQ(cacheHandler->getCacheState(), CACHE_STATE::READY_YET);
}

TEST_F(DataCacheTest, getCachedData_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    ASSERT_EQ(cacheHandler->getCachedData(), RCSResourceAttributes());
}

TEST_F(DataCacheTest, getPrimitiveResource_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    ASSERT_EQ(cacheHandler->getPrimitiveResource(), pResource);
}

TEST_F(DataCacheTest, requestGet_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    cacheHandler->requestGet();
}

TEST_F(DataCacheTest, isEmptySubscriber_normalCase)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    ASSERT_EQ(cacheHandler->isEmptySubscriber(), true);
}

TEST_F(DataCacheTest, requestGet_normalCasetest)
{

    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestGet);
    mocks.OnCall(pResource.get(), PrimitiveResource::isObservable).Return(true);
    mocks.ExpectCall(pResource.get(), PrimitiveResource::requestObserve);
    mocks.OnCall(pResource.get(), PrimitiveResource::requestGet).Do(
        [](GetCallback callback)
    {
        OIC::Service::HeaderOptions hos;
        OIC::Service::RCSResourceAttributes attr;
        OIC::Service::ResponseStatement rep(attr);
        callback(hos, rep, OC_STACK_OK);
        return;
    });
    mocks.OnCall(pResource.get(), PrimitiveResource::cancelObserve);

    cacheHandler->initializeDataCache(pResource);

    cacheHandler->requestGet();
    isLast = true;
}
