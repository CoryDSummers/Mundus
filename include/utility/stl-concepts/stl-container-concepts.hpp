#ifndef MUNDUS_UTILITY_STL_CONCEPTS_STL_CONTAINER_CONCEPTS_H_
#define MUNDUS_UTILITY_STL_CONCEPTS_STL_CONTAINER_CONCEPTS_H_
#include <concepts>
#include "utility/stl-concepts/stl-library-concepts.hpp"
namespace mundus::utility::concepts
{
  template<typename T>
  concept Container = std::copyable<T>
    && std::default_initializable<T> 
    && std::forward_iterator<typename T::iterator> 
    && std::forward_iterator<typename T::const_iterator>
    && requires(T a, const T b)
  {
    typename T::value_type;
    typename T::reference;
    typename T::const_reference;
    typename T::iterator;
    typename T::const_iterator;
    typename T::difference_type;
    typename T::size_type;
    
    { a.begin() }    -> std::same_as<typename T::iterator>;
    { a.end() }      -> std::same_as<typename T::iterator>;
    { b.begin() }    -> std::same_as<typename T::const_iterator>;
    { b.end() }      -> std::same_as<typename T::const_iterator>;
    { a.cbegin() }   -> std::same_as<typename T::const_iterator>;
    { a.cend() }     -> std::same_as<typename T::const_iterator>;

    { b.size() }     -> std::same_as<typename T::size_type>;
    { b.max_size() } -> std::same_as<typename T::size_type>;
    { b.empty() }    -> std::same_as<bool>;
    { a == b }       -> std::convertible_to<bool>;
    { a != b }       -> std::convertible_to<bool>;

    a.swap(a);
  };

  template<typename T>
  concept AllocatorAwareContainer = Container<T>
    && requires (T a, const T t, typename T::allocator_type m)
  {
    typename T::allocator_type;

    requires Allocator<typename T::allocator_type>;
    requires std::same_as<typename T::value_type, typename T::allocator_type::value_type>;

    { a.get_allocator() } -> std::same_as<typename T::allocator_type>;

    requires std::constructible_from<T, typename T::allocator_type>;
    requires std::constructible_from<T, const T&, typename T::allocator_type>;
    requires std::constructible_from<T, T&&, typename T::allocator_type>;
  }

  template<typename T>
  concept AssociativeContainer = AllocatorAwareContainer<T>
    && requires(T a, const T b, typename T::key_type key) 
  {
    typename T::key_type;
    typename T::mapped_type;
    typename T::key_compare;
    requires std::copy_constructible<T::key_compare>;
    requires std::predicate<typename T::value_compare, typename T::value_type, typename T::value_type>;

    { b.key_comp() } -> std::same_as<typename T::key_compare>;
    { b.value_comp() } -> std::same_as<typename T::value_compare>;


    { b.find(key) } -> std::same_as<typename T::const_iterator>;
    { b.count(key) } -> std::same_as<typename T::size_type>;
    { b.lower_bound(key) } -> std::same_as<typename T::const_iterator>;
    { b.upper_bound(key) } -> std::same_as<typename T::const_iterator>;
    { b.equal_range(key) } -> std::same_as<std::pair<typename T::const_iterator, typename T::const_iterator>>;

    { a.find(key) } -> std::same_as<typename T::iterator>;
    { a.lower_bound(key) } -> std::same_as<typename T::iterator>;
    { a.upper_bound(key) } -> std::same_as<typename T::iterator>;
    { a.equal_range(key) } -> std::same_as<std::pair<typename T::iterator, typename T::iterator>>;
    

    
  };

  template<typename T>
  concept UnorderedAssociativeContainer = AllocatorAwareContainer<T>
    && requires(T a, const T b, typename T::key_type key)
  {
    typename T::key_type;
    typename T::mapped_type;
    typename T::key_compare;
    typename T::hasher;
    typename T::key_equal;
    typename T::local_iterator;
    typename T::const_local_iterator;
    typename T::node_type;

    //MemberType Requirements
    requires Hash<T::hasher, T::key_type>;
    //TODO Ensure functions
  }
}

#endif // MUNDUS_UTILITY_STL_CONCEPTS_STL_CONTAINER_CONCEPTS_H_