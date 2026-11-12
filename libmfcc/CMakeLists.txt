cmake_minimum_required(VERSION 3.16)

project(mfcc C)

option(MFCC_BUILD_TESTS "Build the standalone MFCC tests" ON)
option(MFCC_USE_CMSIS_DSP "Use CMSIS-DSP for FFT and vector operations" OFF)
set(MFCC_CMSIS_DSP_INCLUDE_DIR "" CACHE PATH
  "Directory containing CMSIS-DSP arm_math.h")
set(MFCC_CMSIS_DSP_TARGET "" CACHE STRING
  "Optional CMake target providing CMSIS-DSP")

add_library(mfcc STATIC
  Source/mfcc.c
)

target_include_directories(mfcc PUBLIC
  "Include"
)

target_compile_features(mfcc PUBLIC c_std_99)
target_link_libraries(mfcc PUBLIC m)

if(MFCC_USE_CMSIS_DSP)
  if(NOT MFCC_CMSIS_DSP_INCLUDE_DIR)
    message(FATAL_ERROR
      "MFCC_CMSIS_DSP_INCLUDE_DIR is required when MFCC_USE_CMSIS_DSP=ON")
  endif()
  target_include_directories(mfcc PUBLIC "${MFCC_CMSIS_DSP_INCLUDE_DIR}")
  target_compile_definitions(mfcc PUBLIC MFCC_USE_CMSIS_DSP=1)
  if(MFCC_CMSIS_DSP_TARGET)
    target_link_libraries(mfcc PUBLIC "${MFCC_CMSIS_DSP_TARGET}")
  endif()
endif()

if(MFCC_BUILD_TESTS)
  enable_testing()
  add_executable(mfcc_test tests/test_mfcc.c)
  target_link_libraries(mfcc_test PRIVATE mfcc m)
  add_test(NAME mfcc_test COMMAND mfcc_test)
endif()