#include <experimental/simd>

using namespace std::experimental::parallelism_v2;
native_simd<float>
fooLib3(const native_simd<float>& a)
{
  const native_simd<float> b = min(a, native_simd<float>(1));
  return b;
}
