#ifndef MUNDUS_CORE_SMALL_HASH_MAP_MATCH_GROUP_CLASS_HPP_
#define MUNDUS_CORE_SMALL_HASH_MAP_MATCH_GROUP_CLASS_HPP_
#include <bit>

#include <cstddef>
namespace mundus::core
{
  template<std::unsigned_integral T>
  class MatchGroup
  {
    public:
      using value_type = T;
      value_type bitmask;
      struct Iterator
      {
        using value_type = int;
        using difference_type = std::ptrdiff_t;
        value_type mask;

        int operator*() const {
          return std::countr_zero(mask);
        }
        Iterator& operator++()
        {
          mask &= (mask - 1);
          return *this;
        }
        Iterator operator++(int)
        {
          Iterator temp = *this;
          ++(*this);
          return temp;
        }
        bool operator==(const Iterator& other) const { return mask == other.mask; }
        bool operator!=(const Iterator& other) const { return mask != other.mask; }
      };
      Iterator begin() const { return Iterator{bitmask}; }
      Iterator end()   const { return Iterator{0}; }
  };
}

#endif // MUNDUS_CORE_SMALL_HASH_MAP_MATCH_GROUP_CLASS_HPP_