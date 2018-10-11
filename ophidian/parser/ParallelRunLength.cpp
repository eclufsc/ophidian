/*
 * Copyright 2016 Ophidian
   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing,
   software distributed under the License is distributed on an
   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
   KIND, either express or implied.  See the License for the
   specific language governing permissions and limitations
   under the License.
 */

#include "ParallelRunLength.h"

namespace ophidian::parser
{
    void ParallelRunLength::add_length(ParallelRunLength::micrometer_type length) noexcept
    {
        m_lengths.push_back(length);
    }

    void ParallelRunLength::add_width(ParallelRunLength::micrometer_type width) noexcept
    {
        m_widths.push_back(width);
    }

    void ParallelRunLength::add_spacing(ParallelRunLength::micrometer_type width, ParallelRunLength::micrometer_type length, ParallelRunLength::micrometer_type spacing) noexcept
    {
        m_values.insert(std::make_pair(std::make_pair(width, length), spacing));
    }

    const ParallelRunLength::parallelRunLength_container_type& ParallelRunLength::values() const noexcept
    {
        return m_values;
    }

    const ParallelRunLength::micrometer_type &ParallelRunLength::spacing(const ParallelRunLength::micrometer_type width, const ParallelRunLength::micrometer_type length) const noexcept
    {
        return m_values.at(std::make_pair(width, length));
    }

    const ParallelRunLength::micrometer_container_type& ParallelRunLength::widths() const noexcept
    {
        return m_widths;
    }

    const ParallelRunLength::micrometer_container_type& ParallelRunLength::lengths() const noexcept
    {
        return m_lengths;
    }

    int ParallelRunLength::numWidth() const noexcept
    {
        return m_numWidth;
    }

    int ParallelRunLength::numLength() const noexcept
    {
        return m_numLength;
    }
}
