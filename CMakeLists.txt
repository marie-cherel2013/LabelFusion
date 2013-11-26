# This is the root ITK CMakeList file: 
cmake_minimum_required(VERSION 2.8)

# This project is designed to be built outside the Insight source tree.
project(LocalWeights)

# Find ITK
find_package(ITK REQUIRED)
INCLUDE (${ITK_USE_FILE})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

SET(LocalWeights_SOURCE main.cxx itkLocalNormalizedCorrelationWeightFilter.hxx itkWeightedMajorityVotingFilter.hxx)
SET(LocalWeights_HEADERS itkLocalNormalizedCorrelationWeightFilter.h itkWeightedMajorityVotingFilter.h Atlas.h)

add_executable(LocalWeights ${LocalWeights_SOURCE})

TARGET_LINK_LIBRARIES(LocalWeights ${ITK_LIBRARIES})
