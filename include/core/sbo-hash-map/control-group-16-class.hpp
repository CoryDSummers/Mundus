#ifndef MUNDUS_CORE_SMALL_HASH_MAP_SIMD_METADATA_POLICY_8_HPP_
#define MUNDUS_CORE_SMALL_HASH_MAP_SIMD_METADATA_POLICY_8_HPP_
#include <cstdint>
#include <emmintrin.h>
#include "core/small-hash-map/match-group-class.hpp"
#include "core/small-hash-map/metadata-policy-concept.hpp"
namespace mundus::core
{
  class ControlGroup16
  {
    public:
      using value_type = std::uint8_t;
      using size_type  = std::size_t;
      using match_type = MatchGroup<std::uint16_t>;
      static constexpr value_type kHashMask     = 0x7F;
      static constexpr value_type kActiveMask   = 0x80;
      static constexpr value_type kEmptyState   = 0x00;
      static constexpr value_type kDeletedState = 0x01;
      static constexpr size_type  kAccessWidth  = 16ull;
      static inline    match_type Match(const value_type * metadata_array, value_type target_hash, std::size_t);
      static constexpr value_type CreateTag(std::size_t hash);
    private:
      using SIMDRegister = __m128i;

  };

  inline ControlGroup16::match_type ControlGroup16::Match(const value_type *metadata_array, value_type target_hash, std::size_t)
  {
    SIMDRegister target = _mm_set1_epi8(static_cast<char>(target_hash));
    SIMDRegister meta   = _mm_loadu_si128(reinterpret_cast<const SIMDRegister*>(metadata_array));
    SIMDRegister cmp    = _mm_cmpeq_epi8(target, meta);
    return MatchGroup<std::uint16_t>{static_cast<uint16_t>(_mm_movemask_epi8(cmp))};
  }
  inline constexpr ControlGroup16::value_type ControlGroup16::CreateTag(std::size_t hash)
  {
    return static_cast<value_type>((hash & kHashMask) | kActiveMask);
  }
}
#endif // MUNDUS_CORE_SMALL_HASH_MAP_SIMD_METADATA_POLICY_8_HPP_