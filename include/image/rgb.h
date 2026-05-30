#pragma once
#include <cstdint>
namespace image
{
  //0x000000FF = RED
  
  union RGBA
  {
    std::uint32_t value;
    std::uint8_t r, b, g, a;
  };
}