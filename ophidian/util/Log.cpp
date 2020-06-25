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

#include "Log.h"
#include <iostream>
#include <sys/resource.h>
#include <unistd.h>

namespace ophidian::util
{
    double MemoryUsage::get_current()
    {
        long rss = 0L;
        FILE* fp = NULL;
        if ((fp = fopen("/proc/self/statm", "r")) == NULL) {
            return 0.0; /* Can't open? */
        }
        if (fscanf(fp, "%*s%ld", &rss) != 1) {
            fclose(fp);
            return 0.0; /* Can't read? */
        }
        fclose(fp);
        return rss * sysconf(_SC_PAGESIZE) / 1048576.0;
    }

    double MemoryUsage::get_peak()
    {
        struct rusage rusage;
        getrusage(RUSAGE_SELF, &rusage);
        return rusage.ru_maxrss / 1024.0;
    }

} // end namespace ophidian::util