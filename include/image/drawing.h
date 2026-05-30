#pragma once
#include <algorithm>
#include <limits>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "image/rgb.h"
#include "image/image.h"

#include <immintrin.h>
#include "simd/vector8i.h"
namespace image
{
  struct Box
  {
    union
    {
      struct
      {
        int top, right, bottom, left;
      };
      __m128i vec;
    };
  };
  using VertexArray = std::vector<glm::vec2>;
  inline int DrawLine(Image &image, RGBA color, glm::ivec2 p0, glm::ivec2 p1, int offset = 0)
  {
    glm::ivec2 dxdy{glm::abs(p1 - p0)};
    glm::ivec2 sxsy{p0.x < p1.x ? 1 : -1, p0.y < p1.y ? 1 : -1};
    int error = dxdy.x - dxdy.y;
    while (true)
    {
      image[p0.y + offset][p0.x + offset] = color;
      if (p0.x == p1.x && p0.y == p1.y)
        break;
      int error_2 = 2 * error;
      if (error_2 > -dxdy.y)
      {
        error -= dxdy.y;
        p0.x += sxsy.x;
      }
      if (error_2 < dxdy.x)
      {
        error += dxdy.x;
        p0.y += sxsy.y;
      }
    }
    return 0;
  }

  inline Box GetVecticeExtremes(VertexArray &vertices)
  {
    Box extremes = {
        std::numeric_limits<int>::min(), // top
        std::numeric_limits<int>::min(), // right
        std::numeric_limits<int>::max(), // bottom
        std::numeric_limits<int>::max(), // left
    };
    for (auto &vertice : vertices)
    {
      __m128 fvals = _mm_set_ps(vertice.x, vertice.y, vertice.x, vertice.y);
      __m128i vals = _mm_cvtps_epi32(fvals);
      __m128i max_vals = _mm_max_epi32(extremes.vec, vals);
      __m128i min_vals = _mm_min_epi32(extremes.vec, vals);
      extremes.vec = _mm_castps_si128(
          _mm_blend_ps(
              _mm_castsi128_ps(max_vals),
              _mm_castsi128_ps(min_vals),
              0b1100));
    }
    return extremes;
  }

  inline int CreateNodeList(VertexArray const &vertices, std::vector<int> &nodes, int pixel_y)
  {
    const int k_poly_corners = static_cast<int>(vertices.size());
    const float fpixel_y = static_cast<float>(pixel_y);
    int j = k_poly_corners - 1;
    int node_index = 0;
    for (int i = 0; i < k_poly_corners; ++i)
    {
      const glm::vec2 &vertice_i = vertices[i];
      const glm::vec2 &vertice_j = vertices[j];
      if (
          vertice_i.y < fpixel_y && vertice_j.y >= fpixel_y || vertice_j.y < fpixel_y && vertice_i.y >= fpixel_y)
      {
        nodes[node_index] = static_cast<int>(vertice_i.x + (pixel_y - vertice_i.y) / (vertice_j.y - vertice_i.y) * (vertice_j.x - vertice_i.x));
        ++node_index;
      }
      j = i;
    }
    return node_index;
  }
  inline int AVXFill(RGBA *arr, RGBA color, std::size_t length)
  {
    __m256i fill_vec = _mm256_set1_epi32(color.value);
    std::size_t i = 0;
    for (; i + 7 < length; i += 8)
    {
      _mm256_storeu_si256((__m256i *)(arr + i), fill_vec);
    }
    for (; i < length; ++i)
    {
      arr[i] = color;
    }
    return 0;
  }
  inline void FillNode(Image &image, RGBA color, std::vector<int> &node_x, int pixel_y, int index)
  {
    const int x_start = node_x[index];
    const int x_end = node_x[index + 1];
    const int span_length = x_end - x_start;
    const int image_width = image.Width();
    const int image_height = image.Height();
    if (span_length < 0)
      return;
    if (span_length >= image_width)
    {
      AVXFill(&(image[pixel_y][0]), color, image_width);
      return;
    }
    int start_wrapped = (x_start % image_width + image_width) % image_width;
    if (start_wrapped + span_length <= image_width)
    {
      AVXFill(&(image[pixel_y][start_wrapped]), color, span_length);
    }
    else
    {
      // The line segment crosses the 0 / map_width seam.
      // We split it into two AVX fills to maintain memory safety.
      int first_chunk_length = image_width - start_wrapped;
      int second_chunk_length = span_length - first_chunk_length;

      // Fill the right side up to the map's edge
      AVXFill(&(image[pixel_y][start_wrapped]), color, first_chunk_length);

      // Fill the remaining length starting back at 0 on the left side
      if (second_chunk_length > 0)
      {
        AVXFill(&(image[pixel_y][0]), color, second_chunk_length);
      }
    }
  }
  inline int FillPolygon(Image &image, RGBA color, VertexArray &vertices)
  {
    const int k_poly_corners = static_cast<int>(vertices.size());
    const Box extremes = GetVecticeExtremes(vertices);
    std::vector<int> node_x(k_poly_corners);

    int start_y = std::max(0, extremes.bottom);
    int end_y = std::min(image.Height(), extremes.top);

    for (int pixel_y = start_y; pixel_y < end_y; ++pixel_y)
    {
      int nodes = CreateNodeList(vertices, node_x, pixel_y);
      if (nodes == 0)
      {
        continue;
      }
      std::sort(node_x.begin(), node_x.begin() + nodes);
      for (int i = 0; i < nodes; i += 2)
      {
        FillNode(image, color, node_x, pixel_y, i);
      }
    }
    return 0;
  }
  inline void DrawCross(Image &image, RGBA color, glm::ivec2 position, const int size = 1)
  {
    // 1. Calculate the true mathematical bounds
    const int x_start = position.x - size;
    const int x_end = position.x + size + 1; // +1 because loops are usually exclusive (<)
    const int y_start = position.y - size;
    const int y_end = position.y + size + 1;

    // 2. Clamp bounds safely to the image dimensions
    const int draw_x_start = std::max(0, x_start);
    const int draw_x_end = std::min(image.Width(), x_end);
    const int draw_y_start = std::max(0, y_start);
    const int draw_y_end = std::min(image.Height(), y_end);

    // 3. Draw horizontal arm (if the Y coordinate is on-screen)
    if (position.y >= 0 && position.y < image.Height())
    {
      for (int x = draw_x_start; x < draw_x_end; ++x)
      {
        image[position.y][x] = color;
      }
    }
    // 4. Draw vertical arm (if the X coordinate is on-screen)
    if (position.x >= 0 && position.x < image.Width())
    {
      for (int y = draw_y_start; y < draw_y_end; ++y)
      {
        // Optional: Prevent drawing the center pixel twice
        if (y == position.y)
          continue;

        image[y][position.x] = color;
      }
    }
  }
  inline void DrawVertices(Image &image, RGBA color, glm::ivec2 position, const VertexArray & vertices, int offset = 0)
  {
    for(int i = 0; i < vertices.size() - 1; ++i)
    {
      DrawLine(image, color, vertices[i], vertices[i+1], offset);
    }
    DrawLine(image, color, vertices[vertices.size() - 1], vertices[0]);
  }
  inline void Transform(VertexArray & vertices, glm::vec2 translate = glm::vec2(0.0f), glm::vec2 scale = {1.f, 1.f}, float rotate = 0.f)
  {
    glm::mat4 transform = glm::mat3(1.0f);
    transform = glm::translate(transform, glm::vec3(translate, 1.f));
    transform = glm::rotate(transform,    glm::radians(rotate), {0.f, 1.f, 0.f});
    transform = glm::scale(transform,     glm::vec3{scale, 1.f});
    std::transform(vertices.begin(), vertices.end(), vertices.begin(),
    [&transform](const glm::vec2 & v) { return glm::vec2(transform * glm::vec4(v, 1.0f, 1.f));});
  }
}