include(FetchContent)
message(STATUS "Downloading and extracting GLM")
Set(FETCHCONTENT_QUIET FALSE) # Needed to print downloading progress

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.3  # Or a specific commit hash/branch
)
FetchContent_MakeAvailable(glm)

set(BOOST_INCLUDE_LIBRARIES polygon)
set(BOOST_ENABLE_CMAKE ON CACHE BOOL "Enable Boost CMake support")
set(BOOST_SUBMODULES
    libs/polygon
    libs/headers
    libs/config
    libs/assert
    libs/core
    libs/integer
    libs/mpl
    libs/throw_exception
    tools/cmake
)

message(STATUS "Downloading and extracting boost library sources. This will take some time...")
FetchContent_Declare(
    boost
    GIT_REPOSITORY https://github.com/boostorg/boost
    GIT_TAG        boost-1.91.0-1  # Or a specific commit hash/branch
    GIT_SUBMODULES ${BOOST_SUBMODULES}
    GIT_SHALLOW TRUE

)
FetchContent_MakeAvailable(boost)