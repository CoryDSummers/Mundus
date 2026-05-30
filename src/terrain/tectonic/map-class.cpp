#include "terrain/tectonic/map-class.h"
#include <numbers>

#include <glm/glm.hpp>
#include "voronoi/jittered-cell.h"
#include "terrain/geometry.h"
#include "noise/perlin-noise-class.hpp"
struct PlateExpansion
{
    float cost;
    int cell_index;
    int plate_id;

    // We want the lowest cost to bubble to the top
    bool operator>(const PlateExpansion &other) const
    {
        return cost > other.cost;
    }
};
void terrain::tectonic::Map::Initialize(const GeometryArray &generated_cells)
{
    geometries_ = generated_cells;
    cell_plate_ids_.assign(geometries_.size(), -1);
}

void terrain::tectonic::Map::GeneratePlatesSimple(int map_width, int map_height, int grid_cols, int grid_rows, int seed)
{
    voronoi::PointArray raw_plate_seeds = voronoi::GenerateJitteredSeeds(map_width, map_height, grid_cols, grid_rows, seed);
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> vel_dist(-1.0f, 1.0f);
    std::normal_distribution<float> size_weight(1.f, .33f);
    plates_.clear();
    plates_.reserve(raw_plate_seeds.size());
    for (const auto &seed : raw_plate_seeds)
    {
        plates_.push_back({seed, glm::normalize(glm::vec2(vel_dist(gen), vel_dist(gen))), glm::vec2(std::clamp(size_weight(gen), 0.f, 2.f), std::clamp(size_weight(gen), 0.f, 2.f))});
    }
    for (std::size_t cell_index = 0; cell_index < geometries_.size(); ++cell_index)
    {
        float min_distance = std::numeric_limits<float>::max();
        int closest_plate_index = -1;
        for (std::size_t plate_index = 0; plate_index < plates_.size(); ++plate_index)
        {
            float distance = math::GetCylindricalDistance(geometries_[cell_index].seed, plates_[plate_index].center_seed, map_width);
            if (distance < min_distance)
            {
                min_distance = distance;
                closest_plate_index = static_cast<int>(plate_index);
            }
        }
        cell_plate_ids_[cell_index] = closest_plate_index;
    }
}
void terrain::tectonic::Map::DijkstraNoiseFillGeneratePlates(int map_width, int map_height, int plate_count, int seed)
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> vel_dist(-1.0f, 1.0f);
    std::uniform_real_distribution<float> noise_dist(1.0f, 3.0f);
    cell_plate_ids_.assign(geometries_.size(), -1);
    plates_.clear();
    std::priority_queue<PlateExpansion, std::vector<PlateExpansion>, std::greater<PlateExpansion>> queue;
    // 2. Pick random Voronoi cells to act as our initial Plate Seeds
    std::uniform_int_distribution<int> cell_picker(0, geometries_.size() - 1);
    for (int i = 0; i < plate_count; ++i)
    {
        int starting_cell = cell_picker(gen);

        // Ensure we don't pick the same cell twice
        while (cell_plate_ids_[starting_cell] != -1)
        {
            starting_cell = cell_picker(gen);
        }

        // Initialize plate data
        plates_.push_back({
            geometries_[starting_cell].seed,
            glm::normalize(glm::vec2(vel_dist(gen), vel_dist(gen))),
            glm::vec2(1.f, 1.f) // You can still vary size weight if desired
        });

        // Claim the starting cell and push it to the queue with 0 cost
        cell_plate_ids_[starting_cell] = i;
        for (int neighbor_index : geometries_[starting_cell].neighbors)
        {
            if (cell_plate_ids_[neighbor_index] == -1)
            {
                float dist = math::GetCylindricalDistance(
                    geometries_[starting_cell].seed,
                    geometries_[neighbor_index].seed,
                    map_width);

                float step_cost = dist * noise_dist(gen);
                queue.push({step_cost, neighbor_index, i});
            }
        }
    }
    // 3. Grow the plates via Dijkstra's Algorithm
    while (!queue.empty())
    {
        PlateExpansion current = queue.top();
        queue.pop();
        if (cell_plate_ids_[current.cell_index] != -1)
        {
            continue;
        }
        cell_plate_ids_[current.cell_index] = current.plate_id;
        // Iterate through the adjacent neighbors of this Voronoi cell
        // (You will need to implement/call your adjacency list here)
        for (int neighbor_index : geometries_[current.cell_index].neighbors)
        {

            // If the neighbor is already claimed by a plate, skip it
            if (cell_plate_ids_[neighbor_index] != -1)
            {
                continue;
            }

            // Calculate cost to move to this neighbor.
            // Distance ensures general compactness, noise creates jagged edges.
            float dist = math::GetCylindricalDistance(
                geometries_[current.cell_index].seed,
                geometries_[neighbor_index].seed,
                map_width);

            float step_cost = dist * noise_dist(gen);
            float new_cost = current.cost + step_cost;

            // Claim the cell immediately so other plates don't queue it
            // cell_plate_ids_[neighbor_index] = current.plate_id;

            // Push the neighbor into the queue to continue growing
            queue.push({new_cost, neighbor_index, current.plate_id});
        }
    }
    float avg_area = static_cast<float>(map_width * map_height) / geometries_.size();
    float avg_cell_width = std::sqrt(avg_area);

    // 15% of the average width crumples the edges without breaking the geometry
    float safe_warp_strength = avg_cell_width * 0.15f;
    float frequency = 0.05f;

    // Execute the domain warp
    //WarpPlateBoundaries(map_width, safe_warp_strength, frequency, seed);
}
void terrain::tectonic::Map::WarpPlateBoundaries(int map_width, float warp_strength, float frequency, int seed)
{
    const float k_radius = map_width / (2.0f * std::numbers::pi_v<float>);
    mundus::noise::PerlinNoise3D noise(seed);
    // Fractal Noise (fBm) parameters
    const int octaves = 4;
    const float lacunarity = 2.0f;
    const float gain = 0.5f;
    for (std::size_t cell_index = 0; cell_index < geometries_.size(); ++cell_index)
    {
        // Iterate through all vertices of this specific cell
        for (auto &vertex : geometries_[cell_index].vertices)
        {
            int normalized_x = vertex.x % map_width;
            if (normalized_x < 0)
            {
                normalized_x += map_width;
            }

            // Convert the normalized X coordinate to a 3D cylindrical angle
            float theta = (static_cast<float>(normalized_x) / static_cast<float>(map_width)) * (2.0f * std::numbers::pi_v<float>);
            float sample_x = std::cos(theta) * k_radius;
            float sample_z = std::sin(theta) * k_radius;

            glm::vec3 base_point(sample_x, static_cast<float>(vertex.y), sample_z);

            float total_dx = 0.0f;
            float total_dy = 0.0f;
            float current_frequency = frequency;
            float current_amplitude = 1.0f;
            float max_amplitude = 0.0f;

            // 3. Evaluate Fractal Noise (fBm)
            for (int i = 0; i < octaves; ++i)
            {
                // Sample for X displacement
                total_dx += noise.GenerateNoise(base_point * current_frequency) * current_amplitude;

                // FIX 2: Use arbitrary fractional offsets to prevent Perlin integer-grid correlation
                glm::vec3 offset_pos = base_point + glm::vec3(133.71f, 91.33f, 17.59f);
                total_dy += noise.GenerateNoise(offset_pos * current_frequency) * current_amplitude;

                max_amplitude += current_amplitude;

                // Adjust amplitude and frequency for the next octave layer
                current_amplitude *= gain;
                current_frequency *= lacunarity;
            }

            // 4. Normalize the fractal result and apply your warp strength
            float dx = (total_dx / max_amplitude) * warp_strength;
            float dy = (total_dy / max_amplitude) * warp_strength;

            // 5. Apply the physical displacement to the vertex
            vertex.x += static_cast<int>(std::round(dx));
            vertex.y += static_cast<int>(std::round(dy));

            // 6. Enforce map boundaries after warping to ensure they map correctly across the cylinder
            if (vertex.x < 0)
            {
                vertex.x += map_width;
            }
            else if (vertex.x >= map_width)
            {
                vertex.x -= map_width;
            }
        }
    }
}
