#pragma once
#include "Drivers.h"
#include "GpioWrapper.h"
#include <memory>
#include <variant>
class TempSensor {
public:
  enum class SensorState : uint8_t {
    INITIALIZING = 0,
    POWERON = 1,
    POWEROFF = 2
  };
  TempSensor(VoltageSensorInterface &temp_sensor_raw,
             GpioOutputInterface &sensor_power_enable,
             std::shared_ptr<LinuxCallIntf> linux_call);

  std::variant<TempSensor::SensorState, float> getTemperature();
  void SensorOn();
  void SensorOff();

private:
  VoltageSensorInterface &temp_sensor_raw_;
  GpioOutputWrapper sensor_power_enable_;
  const float temp_sensor_sensitivity_ = 0.0225; // 22.5mV/C
  SensorState curr_state = SensorState::POWEROFF;
  std::shared_ptr<LinuxCallIntf> linux_call_;
  std::time_t sensor_init_time = 0;
  const int temp_sensor_wait_time = 5;
};
