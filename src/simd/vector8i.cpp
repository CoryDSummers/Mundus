#include "simd/vector8i.h"
#include "simd/vector8f.h"

simd::Vector8i::Vector8i(const simd::Vector8f& vf)
{
  this->value = _mm256_cvtps_epi32(vf.value);
}
