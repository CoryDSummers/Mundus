#ifndef MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_CLASS_H_
#define MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_CLASS_H_
#include <memory>
#include <unordered_map>
#include <variant>
#include "terrain/tectonic/map-class.h"
#include "core/sbo-hash-map/sbo-hash-map-class.hpp"
#include "utility/fvn1a-hash.hpp"
namespace mundus::terrain::tectonic::pipeline
{
  class Step;
  class GenerationPipeline
  {
    public:
      GenerationPipeline(tectonic::Map & tectonic_map);
      void Restart();
      int  Execute();
      const Step * operator[](std::size_t index);
      int Step();
      //Returns index of present step
      std::size_t AddStep(std::unique_ptr<Step> step);
      Variable GetVariable(std::string const &);
      tectonic::Map & GetMap();
    protected:
      using StepPointer  = std::unique_ptr<Step>;
      using StepPipeline = std::vector<StepPointer>;
      using Variable     = std::variant<std::monostate, std:int64_t, float, glm::vec2>;
      using VariableMap  = core::sbo_hash_map<std::string, Variable, 16, utility::FVN1a>;
      VariableMap  variable_map_;
      StepPipeline step_pipeline_;
      tectonic::Map map_;
      std::size_t  step_index_ = 0;
  };
}
#endif // MUNDUS_TERRAIN_TECTONIC_PIPELINE_PIPELINE_CLASS_H_