/*
 * Copyright 2017 Ophidian
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

#ifndef OPHIDIAN_UTIL_RANGE_H
#define OPHIDIAN_UTIL_RANGE_H

namespace ophidian
{
    namespace util
    {
        template <class IteratorType>
        class Range final
        {
        public:

            Range(const IteratorType begin, const IteratorType end):
                    begin_(begin),
                    end_(end)
            {
            }

            ~Range()
            {
            }

            inline IteratorType begin() const
            {
                return begin_;
            }

            inline IteratorType end() const
            {
                return end_;
            }

            inline typename IteratorType::difference_type size() const
            {
                return std::distance(begin(), end());
            }

            inline bool empty() const
            {
                return size() == 0;
            }

        private:
            const IteratorType begin_;
            const IteratorType end_;
        };
    }     // namespace util
}     // namespace ophidian

#endif // OPHIDIAN_UTIL_RANGE_H