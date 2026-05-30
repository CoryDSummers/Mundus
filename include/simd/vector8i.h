#pragma once
#include <immintrin.h>
#include <ostream>
namespace simd
{
  struct Vector8f;
  struct Vector8i
  {
    inline Vector8i() : value(_mm256_setzero_si256()) {}
    inline Vector8i(__m256i val) : value(val) {}
    inline Vector8i(int i) : value(_mm256_set1_epi32(i)) {}

    // NEW: Explicit conversion from Vec8f
    // We make this 'explicit' so the compiler doesn't accidentally 
    // convert floats to ints behind your back.
    explicit Vector8i(const Vector8f& vf);
    inline operator __m256i() const { return value; }
    __m256i value;
  };
  inline Vector8i Gather(const int * base, const Vector8i& indice)
  {
    return Vector8i(_mm256_i32gather_epi32(base, indice.value, 4));
  }
  inline Vector8i operator&(const Vector8i & lhs, const Vector8i & rhs)
  {
    return Vector8i(_mm256_and_si256(lhs, rhs));
  }
  inline Vector8i operator+(const Vector8i & lhs, const Vector8i & rhs)
  {
    return Vector8i(_mm256_add_epi32(lhs, rhs));
  }
  inline Vector8i operator+(const Vector8i & lhs, int rhs)
  {
    __m256i rhs_vec = _mm256_set1_epi32(rhs);
    return Vector8i(_mm256_add_epi32(lhs, rhs_vec));
  }
  inline Vector8i operator+(int lhs, const Vector8i & rhs)
  {
    return rhs + lhs;
  }

  inline Vector8i operator^(Vector8i const & lhs, Vector8i const & rhs)
  {
    return Vector8i(_mm256_xor_si256(lhs, rhs));
  }
  inline Vector8i operator<<(Vector8i const & lhs, int rhs)
  {
    return Vector8i(_mm256_slli_epi32(lhs, rhs));
  }
}
inline
std::ostream & operator<<(std::ostream & stream, const simd::Vector8i & vec)
{
  int arr[8];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(arr), vec.value);

    // 2. Print from the array
    stream << "[ ";
    for (int i = 0; i < 7; ++i) {
        stream << arr[i] << ", ";
    }
    stream << arr[7] << " ]";
    return stream;
}