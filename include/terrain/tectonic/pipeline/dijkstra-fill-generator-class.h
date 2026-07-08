#ifndef MUNDUS_TERRAIN_TECTONIC_PIPELINE_DIJKSTRA_FILL_GENERATOR_CLASS_H_
#define MUNDUS_TERRAIN_TECTONIC_PIPELINE_DIJKSTRA_FILL_GENERATOR_CLASS_H_
#include <memory>
#include <queue>
#include <random>

#include "terrain/tectonic/types.h"
#include "terrain/tectonic/pipeline/pipeline-interface.h"

namespace mundus::terrain::tectonic::pipeline
{

  class DijkstraPlateGenerator : public Step
  {
    public:
      struct Parameters
      {
        float major_cost_multiplier = 0.2f;
        float minor_cost_multiplier = 1.0f;
      };
      
      DijkstraPlateGenerator(RandomEngine * rng_engine, Parameters & parameters = Parameters());
      virtual int Execute(GenerationPipeline & pipeline) override;
      virtual const std::string & GetName() { return "DijkstraPlateGenerator"; }
    private:
      struct PlateParameters
      {
        glm::vec2 growth_axis;
        float base_cost_multiplier;
        float bias_strength;
      };
      struct PlateExpansion
      {
        float     cost;
        CellIndex cell_index;
        PlateID   plate_id;
        bool operator>(const PlateExpansion &other) const
        {
            return cost > other.cost;
        }
      };
      using QueueType = std::priority_queue<PlateExpansion, std::vector<PlateExpansion>, std::greater<PlateExpansion>>;
      using PlateParametersArray = std::vector<PlateParameters>;

      void InitializePlates(tectonic::Map& map_data, std::uniform_int_distribution<int> & cell_picker);
      void GetOptimiumQueueSize();
      QueueType expansion_queue_;
      RandomEngine *       rng_engine_;
      PlateParametersArray plate_generation_parameters_;
      const Parameters     generation_parameters_;  
      tectonic::Map *      map_;
  };

}

#endif // MUNDUS_TERRAIN_TECTONIC_PIPELINE_DIJKSTRA_FILL_GENERATOR_CLASS_H_