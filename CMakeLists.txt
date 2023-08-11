cmake_minimum_required(VERSION 3.10)

project(FanControllerApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)

FetchContent_MakeAvailable(googletest)

add_library(temp_sensor_lib STATIC TempSensor.cpp TempSensor.h)

add_library(gpio_lib STATIC GpioWrapper.cpp GpioWrapper.h)

add_library(fan_controller_lib STATIC FanController.cpp FanController.h)

add_executable(FanControllerApp main.cpp Drivers.h)
target_link_libraries(FanControllerApp PRIVATE temp_sensor_lib gpio_lib
                                               fan_controller_lib)

enable_testing()

add_executable(FanControllerAppTest FanControllerTest.cpp)

target_link_libraries(FanControllerAppTest GTest::gtest_main GTest::gmock_main
                      temp_sensor_lib gpio_lib fan_controller_lib)

add_executable(TempSensorTest TempSensorTest.cpp)
target_link_libraries(TempSensorTest GTest::gtest_main GTest::gmock_main
                      temp_sensor_lib gpio_lib)

add_executable(GpioWrapperTest GpioWrapperTest.cpp)

target_link_libraries(GpioWrapperTest GTest::gtest_main GTest::gmock_main
                      gpio_lib)

include(GoogleTest)
gtest_discover_tests(FanControllerAppTest)
gtest_discover_tests(TempSensorTest)
gtest_discover_tests(GpioWrapperTest)
