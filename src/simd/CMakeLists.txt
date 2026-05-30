add_library(simd vector8f.cpp vector8i.cpp)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(simd PUBLIC -mavx2)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(simd PUBLIC /arch:AVX2)
endif()

target_include_directories(simd PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)