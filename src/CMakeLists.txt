add_executable(Mundus main.cpp)
target_include_directories(Mundus PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

add_subdirectory("simd")

set_target_properties(Mundus PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_link_libraries(Mundus PRIVATE glm::glm)