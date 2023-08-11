#include "FanController.h"
#include <iostream>
FanController::FanController(VoltageSensorInterface &temp_sensor_raw,
                             GpioOutputInterface &sensor_power_enable,
                             GpioOutputInterface &fan_relay_enable,
                             PwmOutputInterface &fan_output_raw,
                             std::shared_ptr<LinuxCallIntf> linux_call)
    : sensor_(new TempSensor(temp_sensor_raw, sensor_power_enable, linux_call)),
      fan_relay_enable_(fan_relay_enable), fan_output_raw_(fan_output_raw) {}

void FanController::init() {
  sensor_->SensorOn();
  // Starting with fan off, as gpio can float
  // and it's good to start with a known state
  fan_relay_enable_.setOutput(GpioOutputWrapper::GpioState::OFF);
}

float FanController::calculatePwmOutput(float temperature) {
  if (temperature > 140.0f) {
    return 100.0f;
  } else if (temperature >= 130.0) {
    return 90.0f;
  } else if (temperature >= 100.0) {
    return (temperature - 98.0f) * 2.0f + 20.0f;
  } else if (temperature >= 50.0) {
    return (temperature - 50.0f) / 2.0f;
  }

  return 0.0;
}

void FanController::loop() {
  std::variant<TempSensor::SensorState, float> res;
  if (sensor_) {
    res = sensor_->getTemperature();
  } else {
    std::cerr << "temp sensor instance null" << std::endl;
  }
  if (std::holds_alternative<float>(res)) {
    float temp = std::get<float>(res);
    // We don't want write gpio frequently if it's the same state
    GpioOutputWrapper::GpioState gpio_val = fan_relay_enable_.currgpioval();
    if ((gpio_val == GpioOutputWrapper::GpioState::OFF) && temp > 60.0f) {
      fan_relay_enable_.setOutput(GpioOutputWrapper::GpioState::ON);
    } else if ((gpio_val == GpioOutputWrapper::GpioState::ON) && temp < 50.0f) {
      fan_relay_enable_.setOutput(GpioOutputWrapper::GpioState::OFF);
    }
    float pwm_val = calculatePwmOutput(temp);
    if (fan_relay_enable_.currgpioval() == GpioOutputWrapper::GpioState::ON) {
      fan_output_raw_.setOutputDuty(pwm_val);
    }
  } else {
    TempSensor::SensorState val = std::get<TempSensor::SensorState>(res);
    std::cout << "WARN Sensor reading not available = " << static_cast<int>(val)
              << std::endl;
  }
}