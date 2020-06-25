// READ THIS LICENSE AGREEMENT CAREFULLY BEFORE USING THIS PRODUCT.
// BY USING THIS PRODUCT YOU INDICATE YOUR ACCEPTANCE OF THE TERMS OF THE FOLLOWING AGREEMENT.
//  THESE TERMS APPLY TO YOU AND ANY SUBSEQUENT LICENSEE OF THIS PRODUCT.

// License Agreement for CUGR

// Copyright (c) 2019 by The Chinese University of Hong Kong

// All rights reserved

// CU-SD LICENSE (adapted from the original BSD license) Redistribution of the any code,
//  with or without modification, are permitted provided that the conditions below are met.

// Redistributions of source code must retain the above copyright notice,
//  this list of conditions and the following disclaimer.

// Redistributions in binary form must reproduce the above copyright notice, 
// this list of conditions and the following disclaimer in the documentation 
// and/or other materials provided with the distribution.

// Neither the name nor trademark of the copyright holder or the author may be used to 
// endorse or promote products derived from this software without specific prior written permission.

// Users are entirely responsible, to the exclusion of the author, for compliance with 
// (a) regulations set by owners or administrators of employed equipment, 
// (b) licensing terms of any other software, and (c) local, national, 
// and international regulations regarding use, including those regarding import, 
// export, and use of encryption software.

// THIS FREE SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR ANY CONTRIBUTOR 
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, EFFECTS OF UNAUTHORIZED 
// OR MALICIOUS NETWORK ACCESS; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "log.h"

#include <iomanip>
#include <sstream>

#include <sys/resource.h>
#include <unistd.h>

namespace ophidian::util {

timer::timer() { start(); }
void timer::start() { _start = clock::now(); }
double timer::elapsed() const { return std::chrono::duration<double>(clock::now() - _start).count(); }

timer tstamp;

std::ostream& operator<<(std::ostream& os, const timer& t) {
    std::ostringstream oss;  // seperate the impact of format
    oss << "[" << std::setprecision(3) << std::setw(8) << std::fixed << t.elapsed() << "] ";
    os << oss.str();
    return os;
}

double mem_use::get_current() {
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

double mem_use::get_peak() {
    struct rusage rusage;
    getrusage(RUSAGE_SELF, &rusage);
    return rusage.ru_maxrss / 1024.0;
}

std::ostream& log(std::ostream& os) {
    os << tstamp;
    return os;
}

}  // namespace ophidian::util