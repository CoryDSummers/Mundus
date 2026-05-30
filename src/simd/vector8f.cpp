#include "simd/vector8f.h"
#include "simd/vector8i.h"
simd::Vector8f simd::Floor(const Vector8f& value)
{
  return _mm256_floor_ps(value);
}
simd::Vector8f::Vector8f(simd::Vector8i val)
  : value(_mm256_cvtepi32_ps(val)) 
{

}