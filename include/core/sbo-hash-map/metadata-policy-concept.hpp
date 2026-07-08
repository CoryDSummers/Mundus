#ifndef MUNDUS_CORE_SMALL_HASH_MAP_METADATA_POLICY_CONCEPT_HPP_
#define MUNDUS_CORE_SMALL_HASH_MAP_METADATA_POLICY_CONCEPT_HPP_
#include <concepts>
#include <ranges>
namespace mundus::core::concepts
{
  template<typename T>
  concept IndexRange = std::ranges::range<T> && std::integral<std::ranges::range_value_t<T>>;

  template<typename Policy>
  concept MetadataPolicy = 
    requires(
      const typename Policy::value_type * array, 
      typename Policy::value_type target, 
      typename Policy::size_type  capacity,
      std::size_t hash
    ) {
      typename Policy::value_type;
      typename Policy::size_type;
      typename Policy::match_type;

      requires IndexRange(Policy::match_type);

      { Policy::kHashMask     }   -> std::convertible_to<typename Policy::value_type>;
      { Policy::kActiveMask   }   -> std::convertible_to<typename Policy::value_type>;
      { Policy::kDeletedState }   -> std::convertible_to<typename Policy::value_type>;
      { Policy::kEmptyState   }   -> std::convertible_to<typename Policy::value_type>;
      { Policy::kAccessWidth  }   -> std::convertible_to<typename Policy::value_type>;
      { Policy::CreateTag(hash) }                -> std::same_as<typename Policy::value_type>;
      { Policy::Match(array, target, capacity) } -> std::same_as<typename Policy::match_type>;
  }; 
}


#endif // MUNDUS_CORE_SMALL_HASH_MAP_METADATA_POLICY_CONCEPT_HPP_