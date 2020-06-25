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

// Some logging utilities
// 1. "log() << ..." will show a time stamp first
// 2. "print(a, b, c)" is python-like print for any a, b, c that has operator<< overloaded. For example,
//      int a = 10;
//      double b = 3.14;
//      std::string c = "Gengjie";
//      print(a, b, c);
//     This code piece will show "10 3.14 Gengjie".
//

#ifndef OPHIDIAN_UTIL_LOG_H
#define OPHIDIAN_UTIL_LOG_H

#include <chrono>
#include <iostream>
#include <string>

namespace ophidian::util {

// 1. Timer

class timer {
    using clock = std::chrono::high_resolution_clock;

private:
    clock::time_point _start;

public:
    timer();
    void start();
    double elapsed() const;  // seconds
};

std::ostream& operator<<(std::ostream& os, const timer& t);

// 2. Memory

class mem_use {
public:
    static double get_current();  // MB
    static double get_peak();     // MB
};

// 3. Easy print

// print(a, b, c)
inline void print() { std::cout << std::endl; }
template <typename T, typename... TAIL>
void print(const T& t, TAIL... tail) {
    std::cout << t << ' ';
    print(tail...);
}

// "log() << a << b << c" puts a time stamp in beginning
std::ostream& log(std::ostream& os = std::cout);

// "printlog(a, b, c)" puts a time stamp in beginning
template <typename... T>
void printlog(T... t) {
    log();
    print(t...);
}

template <typename... T>
void printflog(T... t) {
    log();
    printf(t...);
}

}  // namespace ophidian::util


#endif // OPHIDIAN_UTIL_LOG_H