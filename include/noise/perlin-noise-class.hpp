#pragma once
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
namespace mundus::noise
{
    class PerlinNoise3D
    {
        public:
            PerlinNoise3D();
            PerlinNoise3D(const std::uint32_t seed);
            float GenerateNoise(glm::vec3 point);
            float GenerateFractalNoise(glm::vec3 point, int octaves, float persistence, float lacunarity);
        private:
            using PermutationVector = std::vector<std::int32_t>;
            void GeneratePermutationVector(const std::uint32_t seed);
            PermutationVector permutations_;
            static float Fade(float value);
            static float Grad(int hash, const glm::vec3&point);
    };
}