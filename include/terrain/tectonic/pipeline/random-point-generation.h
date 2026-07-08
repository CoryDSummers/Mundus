#ifndef MUNDUS_TERRAIN_TECTONIC_PIPELINE_RANDOM_POINT_GENERATION_H_
#define MUNDUS_TERRAIN_TECTONIC_PIPELINE_RANDOM_POINT_GENERATION_H_
#include "terrain/tectonic/pipeline/step-interface.h"
namespace mundus::terrain::tectonic::pipeline
{
  class RandomPointGenerator : public Step
  {
    public:
      virtual int Execute(GenerationPipeline &) = 0;
      virtual const std::string & GetName() {return "RandomPointGenerator"; }
    private:
  };
}
#endif // MUNDUS_TERRAIN_TECTONIC_PIPELINE_RANDOM_POINT_GENERATION_H_