/*{
Copyright © 2018-2019 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
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

}*/

//#define UNITTEST_ONLY_XTEST 1
#include "unittest.h"

namespace stdx = std::experimental;
template <class... Ts> using base_template = stdx::simd_mask<Ts...>;
#include "testtypes.h"

template <typename From, typename To>
  void
  conversions()
  {
    using ToV = typename To::simd_type;

    using stdx::simd_cast;
    using stdx::static_simd_cast;
    using stdx::__proposed::resizing_simd_cast;

    auto x = resizing_simd_cast<To>(From());
    COMPARE(typeid(x), typeid(To));
    COMPARE(x, To()) << '\n'
	<< vir::typeToString<From>() << " -> "
	<< vir::typeToString<To>();

    x = resizing_simd_cast<To>(From(true));
    const To ref = ToV([](auto i) { return i; }) < int(From::size());
    COMPARE(x, ref) << "converted from: " << From(true) << '\n'
	<< vir::typeToString<From>() << " -> "
	<< vir::typeToString<To>();

    const ullong all_bits = ~ullong() >> (64 - From::size());
    for (ullong bit_pos = 1; bit_pos /*until overflow*/; bit_pos *= 2)
      {
	for (ullong bits : {bit_pos & all_bits, ~bit_pos & all_bits})
	  {
	    const auto from = From::__from_bitset(bits);
	    const auto to = resizing_simd_cast<To>(from);
	    COMPARE(to, To::__from_bitset(bits))
	      << "\nfrom: " << from << "\nbits: " << std::hex << bits << std::dec
	      << '\n'
		<< vir::typeToString<From>() << " -> " << vir::typeToString<To>();
	    for (std::size_t i = 0; i < To::size(); ++i)
	      {
		COMPARE(to[i], (bits >> i) & 1)
		<< "\nfrom: " << from << "\nto: " << to
		<< "\nbits: " << std::hex << bits << std::dec << "\ni: " << i
		<< '\n'
		<< vir::typeToString<From>() << " -> "
		<< vir::typeToString<To>();
	      }
	  }
      }
    vir::test::ADD_PASS() << vir::typeToString<From>() << " -> "
	<< vir::typeToString<To>();
  }

template <typename T, typename V, typename = void>
  struct rebind_or_max_fixed
  {
    using type = stdx::rebind_simd_t<
      T, stdx::resize_simd_t<stdx::simd_abi::max_fixed_size<T>, V>>;
  };

template <typename T, typename V>
  struct rebind_or_max_fixed<T, V, std::void_t<stdx::rebind_simd_t<T, V>>>
  {
    using type = stdx::rebind_simd_t<T, V>;
  };

TEST_TYPES(FromTo, conversions,
	   outer_product<all_test_types, all_arithmetic_types>)
  {
    using From = typename FromTo::template at<0>;
    using To = typename FromTo::template at<1>;

    using M0 = typename rebind_or_max_fixed<To, From>::type;
    using M1 = stdx::native_simd_mask<To>;
    using M2 = stdx::simd_mask<To>;
    using M3 = stdx::simd_mask<To, stdx::simd_abi::scalar>;

    using std::is_same_v;
    conversions<From, M0>();
    if constexpr (!is_same_v<M1, M0>)
      conversions<From, M1>();
    if constexpr (!is_same_v<M2, M0> && !is_same_v<M2, M1>)
      conversions<From, M2>();
    if constexpr (!is_same_v<M3, M0> && !is_same_v<M3, M1> && !is_same_v<M3, M2>)
      conversions<From, M3>();
  }
