#pragma once
#include "Drivers.h"
#include "GpioWrapper.h"
#include "TempSensor.h"
#include <ctime>
#include <thread>

/**
 * @brief Example FanController base
 */
class FanController {
public:
  FanController(VoltageSensorInterface &temp_sensor_raw,
                GpioOutputInterface &sensor_power_enable,
                GpioOutputInterface &fan_relay_enable,
                PwmOutputInterface &fan_output_raw,
                std::shared_ptr<LinuxCallIntf> linux_call =
                    std::make_shared<LinuxCallProd>());
  ~FanController() {}
  /*
   * @brief One time executed initialization method
   */
  void init();
  /**
   * @brief Periodic loop method that will be called every 500ms
   */
  void loop();

private:
  float calculatePwmOutput(float temperature);
  std::unique_ptr<TempSensor> sensor_;
  GpioOutputWrapper fan_relay_enable_;
  PwmOutputInterface &fan_output_raw_;
};