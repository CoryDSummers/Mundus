add_library(voronoi polygon-class.cpp voronoi-class.cpp json-serialization.cpp)

target_include_directories(voronoi PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(voronoi PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(voronoi PRIVATE /wd5055)
endif()

target_link_libraries(voronoi PUBLIC Boost::polygon glm::glm)