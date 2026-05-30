#include "noise/perlin-noise-map-class.h"
#include <algorithm>
#include <bit>
#include <iostream>
#include <immintrin.h>
#include <numeric>
#include <random>
#include <string_view>

template<typename T>
void PrintVector(std::string_view name, T & vec)
{
  std::cout << name << " = [";
  for(auto iter = std::begin(vec); iter != std::end(vec) - 1; ++iter)
  {
    std::cout << *iter << ", ";
  }
  std::cout << *(std::end(vec) - 1) << "]";
}

PerlinNoiseMap::PerlinNoiseMap()
  : permutations_(512)
{
  std::random_device random_device;
  GeneratePermutationVector(random_device());
}

PerlinNoiseMap::PerlinNoiseMap(const std::uint32_t seed)
  : permutations_(512)
{
  GeneratePermutationVector(seed);
}

void PerlinNoiseMap::GeneratePermutationVector(const std::uint32_t seed)
{
  std::default_random_engine engine(seed);
  std::iota(permutations_.begin(), permutations_.begin() + 256, 0);
  std::shuffle(permutations_.begin(), permutations_.begin() + 256, engine);
  std::copy(permutations_.begin(), permutations_.begin() + 256, permutations_.begin() + 256);
}

simd::Vector8f PerlinNoiseMap::ComputeGradient(simd::Vector8i const & hash, simd::Vector8f const & x, simd::Vector8f const & y)
{
  static const simd::Vector8i bitmask_1{1};
  static const simd::Vector8i bitmask_2{2};
  static const simd::Vector8i bitmask_3{3};
  const simd::Vector8i hash_masked = hash & bitmask_3;

  simd::Vector8f gradient_x = std::bit_cast<__m256, __m256i>(
    (simd::Vector8i(std::bit_cast<__m256i, __m256>(x.value)) ^ ((hash_masked & bitmask_1) << 31)).value
  );
  simd::Vector8f gradient_y = std::bit_cast<__m256, __m256i>(
    (simd::Vector8i(std::bit_cast<__m256i, __m256>(y.value)) ^ ((hash_masked & bitmask_2) << 30)).value
  );

  return gradient_x + gradient_y;
}

simd::Vector8f PerlinNoiseMap::Fade(const simd::Vector8f &  t)
{
  static const simd::Vector8f c6{6.0f};
  static const simd::Vector8f c15{15.0f};
  static const simd::Vector8f c10{10.0f};
  simd::Vector8f result = (t * c6) - c15;
  result = t * result + c10;
  return t * t * t * result;
}

simd::Vector8f PerlinNoiseMap::Lerp(simd::Vector8f const &t, simd::Vector8f const &a, simd::Vector8f const &b)
{
  simd::Vector8f diff = b - a;
  return t * diff + a;
}

simd::Vector8f PerlinNoiseMap::Normalize(simd::Vector8f const &value)
{
  static const simd::Vector8f k_contrast_multiplier(1.4142135623730950488016887242097f, simd::CompileTime{});
  simd::Vector8f stretched = value * k_contrast_multiplier;
  simd::Vector8f normalized = (stretched + 1.0f) * 0.5f;
  return simd::clamp(normalized, 0.0f, 1.0f);
}

PerlinNoiseMap::NoiseMap PerlinNoiseMap::GenerateMap(const unsigned width, const unsigned height, const float scale, int octave, float persistence, float lacunarity)
{
  const size_t k_size = width * height;
  const unsigned mod_remainder = width % 8;
  const unsigned mod_width = width - mod_remainder;
  const simd::Vector8f x_offsets{7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f, 0.0f};
  
  NoiseMap noise_map(k_size, 0.0f);
  
  for(unsigned y = 0; y < height; ++y)
  {
    float base_y = y * scale;
    simd::Vector8f y_index = base_y;
    
    unsigned x = 0;
    for(; x < mod_width; x += 8)
    {
      simd::Vector8f x_base = static_cast<float>(x);
      simd::Vector8f x_vec  = (x_base + x_offsets) * scale;
      
      simd::Vector8f result = this->GenerateFractalNoise(x_vec, y_index, octave, persistence, lacunarity); 
      int index = y * width + x;
      result.Store(&noise_map[index]);
    }
    if (x < width) {
      x = width - 8;
      simd::Vector8f x_base = static_cast<float>(x);
      simd::Vector8f x_vec  = (x_base + x_offsets) * scale;
            simd::Vector8f result = this->GenerateNoise(x_vec, y_index);
      int index = y * width + x;
      result.Store(&noise_map[index]);
    }
  }
  return noise_map;
}

simd::Vector8f PerlinNoiseMap::GenerateNoise(simd::Vector8f x, simd::Vector8f y)
{
  static const simd::Vector8i mask_255(255);
  const int * permutation_table_ptr = permutations_.data();
  simd::Vector8f floor_x = simd::Floor(x);
  simd::Vector8f floor_y = simd::Floor(y);

  simd::Vector8i grid_x = static_cast<simd::Vector8i>(floor_x) & mask_255;
  simd::Vector8i grid_y = static_cast<simd::Vector8i>(floor_y) & mask_255;

  x -= floor_x;
  y -= floor_y;

  simd::Vector8f fade_x = PerlinNoiseMap::Fade(x);
  simd::Vector8f fade_y = PerlinNoiseMap::Fade(y);
  
  simd::Vector8i grid_x_1 = grid_x + 1;
  simd::Vector8i grid_y_1 = grid_y + 1;
  simd::Vector8i hash_x     = simd::Gather(permutation_table_ptr, grid_x);
  simd::Vector8i hash_x_1   = simd::Gather(permutation_table_ptr, grid_x_1);
  simd::Vector8i hash_bl    = simd::Gather(permutation_table_ptr, hash_x + grid_y);
  simd::Vector8i hash_tl    = simd::Gather(permutation_table_ptr, hash_x + grid_y_1);
  simd::Vector8i hash_br    = simd::Gather(permutation_table_ptr, hash_x_1 + grid_y);
  simd::Vector8i hash_tr    = simd::Gather(permutation_table_ptr, hash_x_1 + grid_y_1);
  simd::Vector8f fract_x_minus_1 = x - 1.0f;
  simd::Vector8f fract_y_minus_1 = y - 1.0f;
  simd::Vector8f lerp_bottom = PerlinNoiseMap::Lerp(fade_x, 
    ComputeGradient(hash_bl, x, y),
    ComputeGradient(hash_br, fract_x_minus_1, y)
  );
  simd::Vector8f lerp_top = PerlinNoiseMap::Lerp(fade_x, 
    ComputeGradient(hash_tl, x, fract_y_minus_1),
    ComputeGradient(hash_tr, fract_x_minus_1, fract_y_minus_1)
  );
  simd::Vector8f result = PerlinNoiseMap::Lerp(fade_y, lerp_bottom, lerp_top);
  return PerlinNoiseMap::Normalize(result);
}

simd::Vector8f PerlinNoiseMap::GenerateFractalNoise(simd::Vector8f x, simd::Vector8f y, int octave, float persistence, float lacunarity)
{
  simd::Vector8f total(0.0f);
  float amplitude = 1.0f;
  float frequency = 1.0f;
  float max_value = 0.0f;
  for(int i = 0; i < octave; ++i)
  {
    simd::Vector8f sample_x = x * frequency;
    simd::Vector8f sample_y = y * frequency;
    simd::Vector8f noise    = this->GenerateNoise(sample_x, sample_y);
    total = total + (noise * amplitude);
    max_value += amplitude;
    amplitude *= persistence;
    frequency *= lacunarity;
  }
  return total / max_value;
}
