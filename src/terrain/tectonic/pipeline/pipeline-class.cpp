#include "pipeline-class.h"

mundus::terrain::tectonic::pipeline::GenerationPipeline::GenerationPipeline(tectonic::Map &tectonic_map)
  : map_(tectonic_map)
  , variable_map_()
{

}

void mundus::terrain::tectonic::pipeline::GenerationPipeline::Restart()
{
  step_index_ = 0;
}

int mundus::terrain::tectonic::pipeline::GenerationPipeline::Execute()
{
  while(step_index_ != variable_map_.size())
  {
    if(Step() == -1)
      return -1;
  }
  return 0
}

const Step *mundus::terrain::tectonic::pipeline::GenerationPipeline::operator[](std::size_t index)
{
  return variable_map_[index].get();
}

int mundus::terrain::tectonic::pipeline::GenerationPipeline::Step()
{
  if(step_index_ == variable_map_.size())
    return -1;

  auto * current_step = variable_map_[step_index].get();
  int result = current_step->Execute();

  if(result == -1)
    return -1;
  #if defined NDEBUG || defined _DEBUG
    std::cout << current_step->GetName() << " returned " << result << "\n";   
  #endif
  return static_cast<int>(++step_index_);
}

std::size_t mundus::terrain::tectonic::pipeline::GenerationPipeline::AddStep(std::unique_ptr<Step> step)
{
  variable_map_.push_back(std::move(step));
  return variable_map_.size() - 1;
}
