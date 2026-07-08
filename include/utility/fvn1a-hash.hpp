#ifndef MUNDUS_NODE_GRAPH_FVNA1_HASH_HPP_
#define MUNDUS_NODE_GRAPH_FVNA1_HASH_HPP_
#include <cstdint>
#include <string_view>
namespace mundus::utility
{
  template<typename T>
  class FVN1a
  {
    static constexpr std::uint64_t k_prime_ = 1099511628211ULL;
    static constexpr std::uint64_t k_offset_basis_ = 14695981039346656037ULL;
  };

  template<>
  class FVN1a<std::string_view>
  {
    constexpr std::size_t operator()(std::string_view key) const noexcept {
      std::uint64_t hash = FVN1a::k_offset_basis_;
      for(char c : key) {
        hash ^= static_cast<std::uint64_t>(c);
        hash *= FVN1a::k_prime_;
      }
      return static_cast<std::size_t>(hash);
    }
  };
}
constexpr std::size_t operator"" _FVN1a(const char * str, std::size_t length) noexcept
{
  return mundus::utility::FVN1a<std::string_view>{}(std::string_view(str, length));
}

#endif // MUNDUS_NODE_GRAPH_FVNA1_HASH_HPP_