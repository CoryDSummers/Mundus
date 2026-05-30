add_library(terrain map-class.cpp)

add_subdirectory("./tectonic")

target_include_directories(terrain PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(terrain PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(terrain PRIVATE tectonic)