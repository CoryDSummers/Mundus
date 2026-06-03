#pragma once
#include <vector>
#include <glm/vec2.hpp>

#include <boost/polygon/point_concept.hpp>
#include <boost/polygon/point_traits.hpp>
namespace boost {
  namespace polygon {
      template <>
      struct geometry_concept<glm::ivec2> {
          typedef point_concept type;
      };
      template <>
      struct point_traits<glm::ivec2> {
          typedef int coordinate_type;

          static inline coordinate_type get(const glm::ivec2& point, orientation_2d orient) {
              return (orient == HORIZONTAL) ? point.x : point.y;
          }
      };
      // Write traits (How Boost constructs/assigns a glm point internally)
    template <>
    struct point_mutable_traits<glm::ivec2> {
        typedef int coordinate_type;
        static inline void set(glm::ivec2& point, orientation_2d orient, int value) {
            if (orient == HORIZONTAL) point.x = value;
            else point.y = value;
        }
        static inline glm::ivec2 construct(int x_value, int y_value) {
            return glm::ivec2(x_value, y_value);
        }
    };
  }
  
}

#include <boost/polygon/voronoi.hpp>
namespace voronoi
{
  struct Cell;

  using Point       = glm::vec2;
  using Vertex      = glm::vec2;
  using VertexArray = std::vector<Vertex>;
  using PointArray  = std::vector<Point>;
  using CellArray   = std::vector<Cell>;
  
  using IntegerPoint      = glm::ivec2;
  using IntegerPointArray = std::vector<IntegerPoint>;
}