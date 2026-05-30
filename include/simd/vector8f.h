#pragma once
#include <immintrin.h>
#include <ostream>
namespace simd
{
  struct Vector8i;
  struct CompileTime{};
  struct Vector8f
  {
    inline Vector8f() : value(_mm256_setzero_ps()) {}
    explicit Vector8f(Vector8i val);
    inline Vector8f(__m256 val) : value(val) {}
    inline Vector8f(const float val) : value(_mm256_set1_ps(val)) {} 
    inline constexpr Vector8f(const float val, CompileTime) : value{val, val, val, val, val, val, val, val} {}
    inline Vector8f(float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7) 
    : value(_mm256_set_ps(f0, f1, f2, f3, f4, f5, f6, f7)) {}

    inline operator __m256() const { return value; }
    Vector8f const & operator=(float rhs) { this->value = _mm256_set1_ps(rhs); return *this; }
    inline Vector8f operator-=(const Vector8f & rhs)
    {
      value = _mm256_sub_ps(value, rhs);
      return *this;
    }
    inline void Store(float * dest) { _mm256_storeu_ps(dest, this->value); }
    __m256 value;
  };
  Vector8f Floor(const Vector8f & value);
  inline Vector8f operator+(const Vector8f & lhs, const Vector8f & rhs)
  {
    return simd::Vector8f(_mm256_add_ps(lhs.value, rhs.value));
  }
  inline Vector8f operator+(const Vector8f & lhs, float rhs)
  {
    return simd::Vector8f(_mm256_add_ps(lhs.value, _mm256_set1_ps(rhs)));
  }

  inline Vector8f operator-(const Vector8f & lhs, const Vector8f & rhs)
  {
    return simd::Vector8f(_mm256_sub_ps(lhs.value, rhs.value));
  }
  inline Vector8f operator-(const Vector8f & lhs, float rhs)
  {
    return simd::Vector8f(_mm256_sub_ps(lhs.value, _mm256_set1_ps(rhs)));
  }
  inline Vector8f operator-(float lhs, const Vector8f & rhs)
  {
    return simd::Vector8f(_mm256_sub_ps(_mm256_set1_ps(lhs), rhs.value));
  }

  inline Vector8f operator*(const Vector8f & lhs, float rhs)
  {
    return simd::Vector8f(_mm256_mul_ps(lhs, _mm256_set1_ps(rhs)));
  }
  inline Vector8f operator*(const Vector8f & lhs, const Vector8f & rhs){
    return simd::Vector8f(_mm256_mul_ps(lhs, rhs));
  }

  inline Vector8f operator/(const Vector8f & lhs, const Vector8f & rhs)
  {
    return simd::Vector8f(_mm256_div_ps(lhs, rhs));
  }
  inline Vector8f operator/(const Vector8f & lhs, float rhs)
  {
    return simd::Vector8f(_mm256_div_ps(lhs, simd::Vector8f(rhs)));
  }
  inline Vector8f clamp(Vector8f const & v, Vector8f const & lo, Vector8f const & hi)
  {
    return _mm256_max_ps(lo, _mm256_min_ps(hi, v));
  }
  inline Vector8f clamp(Vector8f const & v, float lo, float hi)
  {
    return _mm256_max_ps(
      _mm256_set1_ps(lo), 
      _mm256_min_ps(
        _mm256_set1_ps(hi),
        v)
      );
  }
}

inline
std::ostream & operator<<(std::ostream & stream, const simd::Vector8f & vec)
{
    float arr[8];
    _mm256_storeu_ps(arr, vec.value);

    // 2. Print from the array
    stream << "[ ";
    for (int i = 0; i < 7; ++i) {
        stream << arr[i] << ", ";
    }
    stream << arr[7] << " ]";
    return stream;
}