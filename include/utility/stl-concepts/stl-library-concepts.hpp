#ifndef MUNDUS_UTILITY_STL_CONCEPTS_STL_OBJECT_CONCEPTS_H_
#define MUNDUS_UTILITY_STL_CONCEPTS_STL_OBJECT_CONCEPTS_H_
#include <concepts>
#include <cstddef>
namespace mundus::utility::concepts
{
  template<typename T>
  concept Allocator = std::copy_constructible<T>
    && std::equality_comparable<Alloc>
    && requires(T alloc, std::size_t n) 
  {
    typename T::value_type;
    { *alloc.allocate(n) } -> std::same_as<typename T::value_type&>;
    { alloc.deallocate(alloc.allocate(n), n) }; 
  };
  template<typename H, typename Key>
  concept Hash = 
    std::copy_constructible<H>
    && std::destructible<H>
    && std::regular_invocable<H>
    && (H h, const Key k)
    {
      { h(k) } -> std::same_as<std::size_t>;
    }
}
#endif // MUNDUS_UTILITY_STL_CONCEPTS_STL_OBJECT_CONCEPTS_H_