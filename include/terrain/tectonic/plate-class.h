#pragma once
#include <glm/vec2.hpp>

namespace terrain::tectonic
{
  struct PlateParameters
  {
    glm::vec2 growth_axis;
    float base_cost_multiplier;
    float bias_strength;
  };
  struct Plate
  {
    glm::vec2 center_seed;
    glm::vec2 velocity;
    std::size_t seed_cell_index;
  };
  
}