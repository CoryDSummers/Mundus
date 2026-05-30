add_library(hex hex-class.cpp)

target_include_directories(hex PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(hex PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(hex PUBLIC glm::glm)