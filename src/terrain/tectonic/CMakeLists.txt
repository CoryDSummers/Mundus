add_library(tectonic map-class.cpp)

target_include_directories(tectonic PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(tectonic PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(tectonic PUBLIC Boost::polygon glm::glm)