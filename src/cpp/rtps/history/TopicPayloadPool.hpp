// Copyright 2020 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Base.hpp
 */

#ifndef RTPS_HISTORY_TOPICPAYLOADPOOL_HPP
#define RTPS_HISTORY_TOPICPAYLOADPOOL_HPP

#include <fastdds/rtps/common/CacheChange.h>
#include <fastdds/rtps/history/IPayloadPool.h>
#include <fastdds/rtps/resources/ResourceManagement.h>
#include <fastdds/dds/log/Log.hpp>
#include <rtps/history/PoolConfig.h>
#include <rtps/history/ITopicPayloadPool.h>

#include <memory>
#include <vector>

namespace eprosima {
namespace fastrtps {
namespace rtps {

class TopicPayloadPool : public ITopicPayloadPool
{

public:

    TopicPayloadPool() = default;

    virtual ~TopicPayloadPool()
    {
        logInfo(RTPS_UTILS, "PayloadPool destructor");

        for (octet* payload : all_payloads_)
        {
            free(payload);
        }
    }

    virtual bool get_payload(
            uint32_t size,
            CacheChange_t& cache_change) override;

    virtual bool get_payload(
            SerializedPayload_t& data,
            IPayloadPool*& data_owner,
            CacheChange_t& cache_change) override;

    virtual bool release_payload(
            CacheChange_t& cache_change) override;

    virtual bool reserve_history(
            const PoolConfig& config,
            bool is_reader) override;

    virtual bool release_history(
            const PoolConfig& config,
            bool is_reader) override;

    size_t get_allPayloadsSize() const override
    {
        return all_payloads_.size();
    }

    size_t get_freePayloadsSize() const override
    {
        return free_payloads_.size();
    }

    static std::shared_ptr<ITopicPayloadPool> get(
            PoolConfig config);

protected:

    struct FreePayload
    {
        uint32_t max_size = 0;
        octet* data = nullptr;
    };

    /**
     * Adds a new payload in the pool, but does not add it to the list of free payloads
     */
    virtual octet* allocate(
            uint32_t size);

    virtual void update_maximum_size(
            const PoolConfig& config,
            bool is_reserve);

    /**
     * Ensures the pool has capacity for at least @c num_payloads elements.
     *
     * @param [IN] min_num_payloads Minimum number of payloads reserved in the pool
     * @param [IN] size             Size to allocate for the payloads that need to be added to the pool
     *
     * @pre
     *   - @c size <= @c max_pool_size_
     * @post
     *   - @c get_allPayloadsSize() >= @c min_num_payloads
     */
    virtual void reserve (
            uint32_t min_num_payloads,
            uint32_t size);

    /**
     * Ensures the pool has capacity for at most @c num_payloads elements.
     *
     * @param [IN] max_num_payloads Maximum number of payloads reserved in the pool
     *
     * @return @c true on success, @c false otherwise
     *
     * @post
     *   - On success, get_allPayloadsSize() <= max_num_payloads
     *   - On failure, memory for some payloads may have been released, but get_allPayloadsSize() > min_num_payloads
     */
    bool shrink (
            uint32_t max_num_payloads);

    /**
     * Resizes a payload to have @c size octets, and updates the buffer of the pool accordingly.
     *
     * @param [IN,OUT] payload   Payload buffer to resize
     * @param [IN,OUT] size      Size of the payload buffer, will be updated with the final size
     * @param [IN]     new_size  Number of octets that the Payload needs to fit
     *
     * @return @c true on success, @c false otherwise
     *
     * @pre
     *   - @c payload is allocated in the pool
     *   - @c payload is not in the buffer of free payloads. Only payloads marked as 'used' can be resized.
     * @post
     *   - On success, @c payload has @c size octets
     *   - On failure, nothing has changed
     *   - If the buffer is enlarged, newly allocated octets are initialized to zero.
     */
    bool resize_payload (
            octet*& payload,
            uint32_t& size,
            uint32_t new_size);

    virtual MemoryManagementPolicy_t memory_policy() const = 0;

    uint32_t max_pool_size_             = 0;  //< Maximum size of the pool
    uint32_t infinite_histories_count_  = 0;  //< Number of infinite histories reserved
    uint32_t finite_max_pool_size_      = 0;  //< Maximum size of the pool if no infinite histories were reserved

    std::vector<FreePayload> free_payloads_;
    std::vector<octet*> all_payloads_;
};


}  // namespace rtps
}  // namespace fastrtps
}  // namespace eprosima

#endif  // RTPS_HISTORY_TOPICPAYLOADPOOL_HPP
