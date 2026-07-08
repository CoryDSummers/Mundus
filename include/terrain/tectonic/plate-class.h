#pragma once
#include <cstdint>
#include <glm/vec2.hpp>
#include "terrain/tectonic/types.h"
namespace mundus::terrain::tectonic
{
  class Plate
  {
    public:
      static constexpr PlateID k_unassigned = -1;
      glm::vec2 center_seed;
      glm::vec2 velocity;
      CellIndex seed_cell_index;
  };
  
}