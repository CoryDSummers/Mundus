#pragma once
#include <iostream>
#include "voronoi/voronoi-class.h"
namespace voronoi{
  void JsonSerialize(CellArray const & cells, std::ostream & stream);
}