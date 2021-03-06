/*{{{
Copyright © 2019 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
                 Matthias Kretz <m.kretz@gsi.de>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the names of contributing organizations nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

}}}*/

#include "bench.h"

MAKE_VECTORMATH_OVERLOAD(hypot)

template <bool Latency, class T> double benchmark()
{
    T a = T() + 0x1.fe8222p-10f;
    T b = T() + 0x1.82a4bcp-9f;
    // b = std::numeric_limits<T>::quiet_NaN();
    T c = T() + 0x1.f323e6p2f;
    return time_mean<50'000'000>([&]() {
        using ::hypot;
        using std::hypot;
        using std::experimental::hypot;
        fake_modify(a, b, c);
        T r = hypot(a, b, c);
        if constexpr (Latency)
            a = r;
        else
            fake_read(r);
    });
}

int main()
{
    bench_all<float>();
    bench_all<double>();
}
