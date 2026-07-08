#ifndef MUNDUS_UTILITY_STL_CONCEPTS_HPP_
#define MUNDUS_UTILITY_STL_CONCEPTS_HPP_
#include <concepts>
#include <string>
namespace mundus::utility::concepts
{
  
  static_assert(Container<std::vector<int>>);
}
#endif // MUNDUS_UTILITY_STL_CONCEPTS_HPP_