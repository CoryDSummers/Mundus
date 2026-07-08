#ifndef MUNDUS_NODE_GRAPH_NODE_GRAPH_CLASS_HPP_
#define MUNDUS_NODE_GRAPH_NODE_GRAPH_CLASS_HPP_
#include <string>
#include <unordered_map>
#include "utility/stl-concepts/stl-container-concepts.hpp"
#include "core/sbo-hash-map/sbo-hash-map-class.hpp"
#include "utility/fvn1a-hash.hpp"

namespace mundus::core{
  

  template<
    template NodeDataClass,
    typename NodeKey, 
    typename NodeVariableType,
    utility::concepts::Hash<NodeKey> Hasher = utility::FVN1a<NodeKey>,
    utility::concepts::UnorderedAssociativeContainer VariableContainer = core::sbo_hash_map<std::size_t, NodeVariableType, 8, Hasher>
    >
  class NodeGraph
  {
    public:
      struct Node
      {
        using NodeIDArray = std::vector<std::size_t>
        using value_type  = NodeDataClass;
        value_type        logic_payload;
        NodeIDArray       input_nodes_;
        NodeIDArray       output_nodes_;
        VariableContainer internal_variables;
        VariableContainer output_variables;
        bool is_evaluated = false;
      };
    protected:
    private:
  };
}

#endif // MUNDUS_NODE_GRAPH_NODE_GRAPH_CLASS_HPP_