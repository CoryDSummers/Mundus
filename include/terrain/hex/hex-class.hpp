#pragma once
#include <array>
#include <glm/mat2x2.hpp>

#include "terrain/hex/types.hpp"
namespace mundus::terrain::hex
{

  template<Vector3D VectorType>
  class Hex
  {
  public:
    using Neighbors  = std::array<Hex, 6>;
    using ScalarType = VectorType::value_type;
    Hex() = default;
    Hex(ScalarType q, ScalarType r, ScalarType s);
    Hex(ScalarType q, ScalarType r);
    Hex(VectorType const & coord);

    ScalarType Q() { return coords_.x; }
    ScalarType R() { return coords_.y; }
    ScalarType S() { return coords_.z; }

    Neighbors GetNeighbors() const;
    ScalarType Length() const;
    ScalarType Distance(const Hex<VectorType> & rhs);
    template<Vector2D CartesianType>
    CartesianType ToCartesian(const ScalarType hex_size, CartesianType offset = CartesianType(0, 0)) const;

    Hex operator+(const Hex &other) const
    {
      VectorType result = coords_ + other.coords_;
      return Hex(result);
    }

    Hex operator-(const Hex &other) const
    {
      VectorType result = coords_ - other.coords_;
      return Hex(result);
    }

    bool operator==(const Hex &other) const
    {
      return coords_ == other.coords_;
    }

  protected:
    VectorType coords_;
  };
  template<Vector3D VectorType>
  Hex<VectorType>::Hex(ScalarType q, ScalarType r, ScalarType s)
  : coords_(q, r, s)
  {
  }
  template<Vector3D VectorType>
  Hex<VectorType>::Hex(ScalarType q, ScalarType r)
    : coords_(q, r, -q - r)
    {
    }
  template<Vector3D VectorType>
  Hex<VectorType>::Hex(VectorType const & coord)
    : coords_(coord)
  { 
  }
  template<Vector3D VectorType>
  Hex<VectorType>::Neighbors Hex<VectorType>::GetNeighbors() const
  {
    static const Neighbors k_directions = {
        Hex(1, 0, -1), Hex(1, -1, 0), Hex(0, -1, 1),
        Hex(-1, 0, 1), Hex(-1, 1, 0), Hex(0, 1, -1)
    }; 
    Neighbors neighbors;
    for(std::size_t i = 0; i < k_directions.size(); ++i)
    {
      neighbors[i] = *this + k_directions;
    }
    return neighbors;
  }

  
  template<Vector3D VectorType>
  Hex<VectorType>::ScalarType Hex<VectorType>::Length() const
  {
    glm::ivec3 absolute_coords = glm::abs(glm::vec3(coords_.x, coords_.y, coords_.z));
    return (absolute_coords.x + absolute_coords.y + absolute_coords.z) / 2;
  }



  
  template<Vector3D VectorType>
  Hex<VectorType>::ScalarType Hex<VectorType>::Distance(const Hex<VectorType> & rhs)
  {
    return (*this - rhs).length();
  }

  template<Vector3D VectorType>
  template<Vector2D CartesianType>
  CartesianType Hex<VectorType>::ToCartesian(const ScalarType hex_size, CartesianType offset) const
  {
    static const glm::mat2 orientation{
      std::sqrt(3.0), 0.0f,
      std::sqrt(3.0)/ 2.0f, 1.5f
    };
    CartesianType cartesian_coords = (orientation * CartesianType{this->Q(), this->R()}) * hex_size;
    return cartesian_coords + offset;
  }


}