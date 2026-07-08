#include "terrain/tectonic/pipeline/dijkstra-fill-generator-class.h"
#include "dijkstra-fill-generator-class.h"

namespace tpl = mundus::terrain::tectonic::pipeline;

int tpl::DijkstraPlateGenerator::Execute(GenerationPipeline & pipeline)
{
  std::uniform_int_distribution<int>    cell_picker(0, map_data.GetGeometries().size() - 1);
  std::uniform_real_distribution<float> noise_dist(1.0f, 3.0f);
  InitializePlates(plate_generation_parameters, cell_picker);
  
  return 0;
}

tpl::DijkstraPlateGenerator::DijkstraPlateGenerator(RandomEngine * rng_engine, Parameters &parameters)
  : expansion_queue_()
  , rng_engine_(rng_engine)
  , PlateParametersArray()
  , generation_parameters_(parameters)
{
}

void tpl::DijkstraPlateGenerator::InitializePlates(tectonic::Map &map_data, std::uniform_int_distribution<int> &cell_picker)
{
  const auto&         map_params = map_data.GetParameters();
  const std::uint32_t k_total_plate_count = map_params.major_plate_count + map_params.minor_plate_count;
  const float         k_min_seed_distance = map_params.map_width / 12.0f;

  std::uniform_real_distribution<float> dist_normalized(-1.0f, 1.0f);
  std::uniform_real_distribution<float> bias_dist(0.0f, 0.9f);
  
  PlateArray      plates{k_total_plate_count};
  GeometryArray & geometries = map_data.GetGeometries();

  plate_parameters_ = PlateParametersArray(k_total_plate_count);

  for (std::uint32_t plate_index = 0; plate_index < k_total_plate_count; ++plate_index)
  {
    CellIndex starting_cell   = GetSpacedStartingCell(cell_picker, k_min_seed_distance, *rng_engine);
    bool      is_major        = (plate_index < this->parameters_.major_plate_count);
    float     cost_multiplier = is_major ? this->parameters_.major_cost_multiplier : this->parameters_.minor_cost_multiplier;

    glm::vec2 growth_axis     = glm::normalize(glm::vec2(dist_normalized(*rng_engine), dist_normalized(*rng_engine)));
    glm::vec2 velocity        = glm::normalize(glm::vec2(dist_normalized(*rng_engine), dist_normalized(*rng_engine)));

    plates[plate_index] = Plate{
          geometries[starting_cell].seed,
          velocity,
          starting_cell
    };
    plate_parameters_[plate_index] = PlateParameters{
      growth_axis,
      cost_multiplier,
      bias_dist(gen)
    };
    map_data.GetPlateIDs()[starting_cell] = PlateID(plate_index);
  }
  map_data.SetPlates(plates);
}
