/*
 * Copyright 2019, Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef EPROSIMA_DDS_TOPIC_TFILTER_HPP_
#define EPROSIMA_DDS_TOPIC_TFILTER_HPP_

/**
 * @file
 */

/*
 * OMG PSM class declaration
 */
#include <dds/topic/Filter.hpp>

namespace dds {
namespace topic {

template<typename D>
TFilter<D>::TFilter(
        const std::string& query_expression)
    : dds::core::Value<D>(query_expression)
{
}

template<typename D>
template<typename FWIterator>
TFilter<D>::TFilter(
        const std::string& query_expression,
        const FWIterator& params_begin,
        const FWIterator& params_end)
    : dds::core::Value<D>(query_expression, params_begin, params_end)
{
}

template<typename D>
TFilter<D>::TFilter(
        const std::string& query_expression,
        const std::vector<std::string>& params)
    : dds::core::Value<D>(query_expression, params.begin(), params.end())
{
}

template<typename D>
const std::string& TFilter<D>::expression() const
{
    //To implement
    //    return this->delegate().expression();
}

template<typename D>
typename TFilter<D>::const_iterator TFilter<D>::begin() const
{
    //To implement
    //    return this->delegate().begin();
}

template<typename D>
typename TFilter<D>::const_iterator TFilter<D>::end() const
{
    //To implement
    //    return this->delegate().end();
}

template<typename D>
typename TFilter<D>::iterator TFilter<D>::begin()
{
    return this->delegate().begin();
}

template<typename D>
typename TFilter<D>::iterator TFilter<D>::end()
{
    //To implement
    //    return this->delegate().end();
}

template<typename D>
template<typename FWIterator>
void TFilter<D>::parameters(
        const FWIterator& begin,
        const FWIterator end)
{
    //To implement
    //    this->delegate().parameters(begin, end);
}

template<typename D>
void TFilter<D>::add_parameter(
        const std::string& param)
{
    //To implement
    //    this->delegate().add_parameter(param);
}

template<typename D>
uint32_t TFilter<D>::parameters_length() const
{
    //To implement
    //    return this->delegate().parameters_length();
}

} //namespace topic
} //namespace dds

#endif //EPROSIMA_DDS_TOPIC_TFILTER_HPP_