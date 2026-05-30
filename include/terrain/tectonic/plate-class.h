#pragma once
#include <glm/vec2.hpp>

namespace terrain::tectonic
{
  struct Plate
  {
    glm::ivec2 center_seed;
    glm::vec2  velocity;
    glm::vec2  weight;
  };
  
}