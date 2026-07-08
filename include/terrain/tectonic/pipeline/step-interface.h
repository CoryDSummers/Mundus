#ifndef MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_INTERFACE_H_
#define MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_INTERFACE_H_
#include <string>
namespace mundus::terrain::tectonic::pipeline
{
  class GenerationPipeline;
  class Step
  {
     public:
      virtual int Execute(GenerationPipeline &) = 0;
      virtual const std::string & GetName() = 0;
  };
}
/*
Input defined via JSON
Graphs connected via node inputs and outputs


*/
#endif // MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_INTERFACE_H_