#pragma once
#include <vector>
#include "terrain/hex/hex-class.hpp"
namespace mundus::terrain::hex
{
  template<
    typename CellType,
    Vector3D VectorType,
    typename Allocator = std::allocator<CellType>>
  class HexGrid
  {
    public:
      using HexType       = Hex<VectorType>;
      using CellArray     = std::vector<CellType, Allocator>;
      using size_type     = std::size_t;
      using value_type    = CellType;
      using IndexVector   = glm::vec<2, size_type, glm::defaultp>;
      HexGrid() = default;
      HexGrid(size_type width, size_type height, CellType default_value = CellType{});
      CellType * data() { return cells_.data(); }
      size_type GetWidth()  const { return width_; }
      size_type GetHeight() const { return height_; }
      CellType & operator[](const HexType & hex);
    private:
      IndexVector HexToOffset(const HexType & hex) const;
      bool InBounds(const IndexVector & index) const;
      size_type width_ = 0;
      size_type height_ = 0;
      CellArray cells_;
  };
  template<typename CellType, Vector3D VectorType, typename Allocator>
  HexGrid<CellType, VectorType, Allocator>::HexGrid(size_type width, size_type height, CellType default_value)
    : width_(width)
    , height_(height)
    , cells_(width * height, default_value)
    {}
  template<typename CellType, Vector3D VectorType, typename Allocator>
  CellType & HexGrid<CellType, VectorType, Allocator>::operator[](const HexType & hex)
  {
    const IndexVector index = HexToOffset(hex);
    if(!InBounds(index))
      throw std::out_of_range("Hex out of grid bounds.");
    return cells_[index.y * width_ + index.x];
  }
  template<typename CellType, Vector3D VectorType, typename Allocator>
  HexGrid<CellType, VectorType, Allocator>::IndexVector HexGrid<CellType, VectorType, Allocator>::HexToOffset(const HexType & hex) const
  {
    size_type column = static_cast<size_type>(hex.Q());
    return {column, static_cast<size_type>(hex.R()) + (column - (column & 1)) / 2};
  }
  template<typename CellType, Vector3D VectorType, typename Allocator>
  bool HexGrid<CellType, VectorType, Allocator>::InBounds(const IndexVector & index) const
  {
    return index.x < width_ && index.y < height_;
  }
}