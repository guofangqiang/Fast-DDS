// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
 * @file History.cpp
 *
 */


#include <fastdds/rtps/history/History.h>

#include <fastdds/dds/log/Log.hpp>
#include <fastdds/rtps/common/CacheChange.h>

#include <rtps/history/BasicPayloadPool.hpp>
#include <rtps/history/CacheChangePool.h>

#include <mutex>

namespace eprosima {
namespace fastrtps {
namespace rtps {

History::History(
        const HistoryAttributes& att)
    : m_att(att)
    , m_isHistoryFull(false)
    , max_payload_size_(att.payloadMaxSize)
    , mp_mutex(nullptr)

{
    int32_t max_caches = std::max(att.maximumReservedCaches, 0);
    int32_t initial_caches = std::max(att.initialReservedCaches, 0);
    m_changes.reserve(static_cast<size_t>(initial_caches));

    PoolConfig pool_config
    {
        att.memoryPolicy,
        att.payloadMaxSize,
        static_cast<uint32_t>(initial_caches),
        static_cast<uint32_t>(max_caches)
    };

    payload_pool_ = BasicPayloadPool::get(pool_config);

    if ((att.memoryPolicy == PREALLOCATED_MEMORY_MODE) || (att.memoryPolicy == PREALLOCATED_WITH_REALLOC_MEMORY_MODE))
    {
        auto init_cache = [this](
                CacheChange_t* item)
        {
            if (payload_pool_->get_payload(m_att.payloadMaxSize, *item))
            {
                payload_pool_->release_payload(*item);
            }
        };

        change_pool_ = std::make_shared<CacheChangePool>(pool_config, init_cache);
    }
    else
    {
        change_pool_ = std::make_shared<CacheChangePool>(pool_config);
    }
}

History::~History()
{
    logInfo(RTPS_HISTORY, "");

    for (auto change : m_changes)
    {
        do_release_cache(change);
    }

    // As releasing the change pool will delete the cache changes it owns,
    // the payload pool may be called to release their payloads, so we should
    // ensure that the payload pool is destroyed after the change pool.
    change_pool_.reset();
    payload_pool_.reset();
}

void History::do_release_cache(
        CacheChange_t* ch)
{
    IPayloadPool* pool = ch->payload_owner();
    if (pool)
    {
        pool->release_payload(*ch);
    }
    change_pool_->release_cache(ch);
}

bool History::remove_all_changes()
{
    if (mp_mutex == nullptr)
    {
        logError(RTPS_HISTORY, "You need to create a RTPS Entity with this History before using it");
        return false;
    }

    std::lock_guard<RecursiveTimedMutex> guard(*mp_mutex);
    if (!m_changes.empty())
    {
        while (!m_changes.empty())
        {
            remove_change(m_changes.front());
        }
        m_changes.clear();
        m_isHistoryFull = false;
        return true;
    }
    return false;
}

bool History::get_min_change(
        CacheChange_t** min_change)
{
    if (!m_changes.empty())
    {
        *min_change = m_changes.front();
        return true;
    }
    return false;

}

bool History::get_max_change(
        CacheChange_t** max_change)
{
    if (!m_changes.empty())
    {
        *max_change = m_changes.back();
        return true;
    }
    return false;
}

bool History::get_change(
        const SequenceNumber_t& seq,
        const GUID_t& guid,
        CacheChange_t** change) const
{

    if (mp_mutex == nullptr)
    {
        logError(RTPS_HISTORY, "You need to create a RTPS Entity with this History before using it");
        return false;
    }

    std::lock_guard<RecursiveTimedMutex> guard(*mp_mutex);
    get_change_nts(seq, guid, change, m_changes.cbegin());
    return *change != nullptr;
}

History::const_iterator History::get_change_nts(
        const SequenceNumber_t& seq,
        const GUID_t& guid,
        CacheChange_t** change,
        History::const_iterator hint) const
{
    const_iterator returned_value = hint;
    *change = nullptr;

    for (; returned_value != m_changes.end(); ++returned_value)
    {
        if ((*returned_value)->writerGUID == guid)
        {
            if ((*returned_value)->sequenceNumber == seq)
            {
                *change = *returned_value;
                break;
            }
            else if ((*returned_value)->sequenceNumber > seq)
            {
                break;
            }
        }
    }

    return returned_value;
}

bool History::get_earliest_change(
        CacheChange_t** change)
{
    if (mp_mutex == nullptr)
    {
        logError(RTPS_HISTORY, "You need to create a RTPS Entity with this History before using it");
        return false;
    }

    std::lock_guard<RecursiveTimedMutex> guard(*mp_mutex);

    if (m_changes.empty())
    {
        return false;
    }

    *change = m_changes.front();
    return true;
}

} // namespace rtps
} // namespace fastrtps
} // namespace eprosima


//TODO Remove if you want.
#include <sstream>

namespace eprosima {
namespace fastrtps {
namespace rtps {

void History::print_changes_seqNum2()
{
    std::stringstream ss;
    for (std::vector<CacheChange_t*>::iterator it = m_changes.begin();
            it != m_changes.end(); ++it)
    {
        ss << (*it)->sequenceNumber << "-";
    }
    ss << std::endl;
    std::cout << ss.str();
}

} // namespace rtps
} /* namespace rtps */
} /* namespace eprosima */
