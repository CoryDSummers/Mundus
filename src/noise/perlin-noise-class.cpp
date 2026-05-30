#include "noise/perlin-noise-class.hpp"
#include <numeric>
#include <random>
mundus::noise::PerlinNoise3D::PerlinNoise3D()
    : permutations_(512)
{
    std::random_device random_device;
    GeneratePermutationVector(random_device());
}

mundus::noise::PerlinNoise3D::PerlinNoise3D(const std::uint32_t seed)
    : permutations_(512)
{
    GeneratePermutationVector(seed);
}

float mundus::noise::PerlinNoise3D::GenerateNoise(glm::vec3 point)
{
    const glm::vec3 point_floored = glm::floor(point);
    const glm::ivec3 cube = glm::ivec3(point_floored) & 255;
    glm::vec3 rel = point - point_floored;
    glm::vec3 f = glm::vec3(Fade(rel.x), Fade(rel.y), Fade(rel.z));

    int hashX0 = permutations_[cube.x], hashX1 = permutations_[cube.x + 1];
    int hashX0_Y0 = permutations_[hashX0 + cube.y], hashX0_Y1 = permutations_[hashX0 + cube.y + 1];
    int hashX1_Y0 = permutations_[hashX1 + cube.y], hashX1_Y1 = permutations_[hashX1 + cube.y + 1];

    int c000 = permutations_[hashX0_Y0 + cube.z], c010 = permutations_[hashX0_Y1 + cube.z];
    int c100 = permutations_[hashX1_Y0 + cube.z], c110 = permutations_[hashX1_Y1 + cube.z];
    int c001 = permutations_[hashX0_Y0 + cube.z + 1], c011 = permutations_[hashX0_Y1 + cube.z + 1];
    int c101 = permutations_[hashX1_Y0 + cube.z + 1], c111 = permutations_[hashX1_Y1 + cube.z + 1];

    return glm::mix(
        glm::mix(
            glm::mix(Grad(c000, rel), Grad(c100, rel - glm::vec3(1.0f, 0.0f, 0.0f)), f.x),
            glm::mix(Grad(c010, rel - glm::vec3(0.0f, 1.0f, 0.0f)), Grad(c110, rel - glm::vec3(1.0f, 1.0f, 0.0f)), f.x),
            f.y),
        glm::mix(
            glm::mix(Grad(c001, rel - glm::vec3(0.0f, 0.0f, 1.0f)), Grad(c101, rel - glm::vec3(1.0f, 0.0f, 1.0f)), f.x),
            glm::mix(Grad(c011, rel - glm::vec3(0.0f, 1.0f, 1.0f)), Grad(c111, rel - glm::vec3(1.0f, 1.0f, 1.0f)), f.x),
            f.y),
        f.z);
}

float mundus::noise::PerlinNoise3D::GenerateFractalNoise(glm::vec3 point, int octaves, float persistence, float lacunarity)
{
    float total = 0.0f;
    float frequency = 1.0f;
    float amplitude = 1.0f;
    float maxValue = 0.0f; // Used for normalizing the result

    for (int i = 0; i < octaves; ++i)
    {
        // Evaluate noise at the current frequency and scale by the current amplitude
        total += GenerateNoise(point * frequency) * amplitude;

        // Accumulate the maximum possible value for normalization
        maxValue += amplitude;

        // Prepare for the next octave
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    // Normalize the final output to keep it within the original noise bounds
    return total / maxValue;
}

void mundus::noise::PerlinNoise3D::GeneratePermutationVector(const std::uint32_t seed)
{
    std::default_random_engine engine(seed);
    std::iota(permutations_.begin(), permutations_.begin() + 256, 0);
    std::shuffle(permutations_.begin(), permutations_.begin() + 256, engine);
    std::copy(permutations_.begin(), permutations_.begin() + 256, permutations_.begin() + 256);
}

float mundus::noise::PerlinNoise3D::Fade(float value)
{
    return value * value * value * (value * (value * 6.0f - 15.0f) + 10.0f);
}

float mundus::noise::PerlinNoise3D::Grad(int hash, const glm::vec3 &point)
{
    int h = hash & 15;
    float u = h < 8 ? point.x : point.y;
    float v = h < 4 ? point.y : h == 12 || h == 14 ? point.x : point.z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
