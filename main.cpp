
#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include "Drivers.h"
#include "FanController.h"

/**
 * @brief Example of how parameters will be passed to your Controller and how it
 * will be executed
 */
void run_test_demo_code(VoltageSensorInterface &temp_sensor_raw,
                        GpioOutputInterface &sensor_power_enable,
                        GpioOutputInterface &fan_relay_enable,
                        PwmOutputInterface &fan_output_raw) {
  /** TODO: instantiate and initialize FanController **/

  std::chrono::minutes loopDuration(1);
  auto startTime = std::chrono::steady_clock::now();
  FanController controller(temp_sensor_raw, sensor_power_enable,
                           fan_relay_enable, fan_output_raw);
  controller.init();
  while (true) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::minutes>(
        currentTime - startTime);
    if (elapsedTime >= loopDuration) {
      break;
    }
    controller.loop();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}

int main() {
  VoltageSensorInterface temp_sensor_raw;
  GpioOutputInterface sensor_power_enable;
  GpioOutputInterface fan_relay_enable;
  PwmOutputInterface fan_output_raw;

  run_test_demo_code(temp_sensor_raw, sensor_power_enable, fan_relay_enable,
                     fan_output_raw);
  return 0;
}