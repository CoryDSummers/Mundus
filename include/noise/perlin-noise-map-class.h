#pragma once
#include <vector>

#include <cstdint>
#include "simd/vector8f.h"
#include "simd/vector8i.h"
class PerlinNoiseMap
{
  public:
    using NoiseMap = std::vector<float>;

    PerlinNoiseMap();
    PerlinNoiseMap(const std::uint32_t seed);
    NoiseMap GenerateMap(const unsigned width, const unsigned height, const float scale, int octave, float persistence, float lacunarity);
    simd::Vector8f GenerateNoise(simd::Vector8f x, simd::Vector8f y);
    simd::Vector8f GenerateFractalNoise(simd::Vector8f x, simd::Vector8f y, int octave, float persistence, float lacunarity);
  private:
    void GeneratePermutationVector(const std::uint32_t seed);
    static simd::Vector8f ComputeGradient(simd::Vector8i const &, simd::Vector8f const &, simd::Vector8f const &);
    using PermutationVector = std::vector<std::int32_t>;
    PermutationVector permutations_;
    static simd::Vector8f Fade(const simd::Vector8f &);
    static simd::Vector8f Lerp(simd::Vector8f const & t, simd::Vector8f const & a, simd::Vector8f const & b);
    static simd::Vector8f Normalize(simd::Vector8f const & value);
};