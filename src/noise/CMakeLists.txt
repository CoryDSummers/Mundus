add_library(noise perlin-noise-class.cpp perlin-noise-map-class.cpp)


if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(noise PUBLIC -mavx2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(noise PUBLIC /arch:AVX2)
endif()

target_include_directories(noise PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(noise PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(noise PUBLIC glm::glm)