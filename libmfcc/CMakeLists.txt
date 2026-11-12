cmake_minimum_required(VERSION 3.16)

project(mfcc C)

option(MFCC_BUILD_TESTS "Build the standalone MFCC tests" ON)

add_library(mfcc STATIC
  Source/mfcc.c
)

target_include_directories(mfcc PUBLIC
  "Include"
)

target_compile_features(mfcc PUBLIC c_std_99)
target_link_libraries(mfcc PUBLIC m)

if(MFCC_BUILD_TESTS)
  enable_testing()
  add_executable(mfcc_test tests/test_mfcc.c)
  target_link_libraries(mfcc_test PRIVATE mfcc m)
  add_test(NAME mfcc_test COMMAND mfcc_test)
endif()