#include "TempSensor.h"

TempSensor::TempSensor(VoltageSensorInterface &temp_sensor_raw,
                       GpioOutputInterface &sensor_power_enable,
                       std::shared_ptr<LinuxCallIntf> linux_call)
    : temp_sensor_raw_(temp_sensor_raw),
      sensor_power_enable_(sensor_power_enable), linux_call_(linux_call) {}

void TempSensor::SensorOn() {
  // Set the gpio to power up the sensor
  sensor_power_enable_.setOutput(GpioOutputWrapper::GpioState::ON);
  curr_state = SensorState::INITIALIZING;
  sensor_init_time = linux_call_->currepochtime();
}

void TempSensor::SensorOff() {
  sensor_power_enable_.setOutput(GpioOutputWrapper::GpioState::OFF);
  curr_state = SensorState::POWEROFF;
}

std::variant<TempSensor::SensorState, float> TempSensor::getTemperature() {

  if (curr_state == SensorState::POWEROFF) {
    return SensorState::POWEROFF;
  }
  if (curr_state == SensorState::INITIALIZING) {
    std::time_t diff = linux_call_->currepochtime() - sensor_init_time;

    if (diff < temp_sensor_wait_time) {
      return SensorState::INITIALIZING;
    } else {
      curr_state = SensorState::POWERON;
    }
  }
  /*
      VOUT = (V+/5 V) × [1.375 V +(22.5 mV/°C) × TA]
      based off this equation and V+ = 5V
      arrvied to this equation
      Confirmed this equation yields the border values
      "The output swings from 0.25 V at −50°C to +4.75 V at +150°C using a
     single +5.0 V supply"
  */
  float temp =
      (temp_sensor_raw_.getVoltage() - 1.375f) / temp_sensor_sensitivity_;
  return temp;
}