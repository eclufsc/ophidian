
/*
 * Copyright 2018 Ophidian
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

#include "CornerSpacing.h"

namespace ophidian::parser
{

    const CornerSpacing::width_type& CornerSpacing::eol_width() const noexcept
    {
        return m_eol_width;
    }

    const CornerSpacing::width_to_spacing_container_type& CornerSpacing::width_to_spacing() const noexcept
    {
        return m_width_legth_to_spacing;
    }

}
