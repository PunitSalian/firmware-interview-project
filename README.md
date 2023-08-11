# Introduction
MoxionPower has a lot of cooling fans in our system. Your task today is to implement a controller for one of our fans. We value correct and well tested code at MoxionPower. We hope that you would feel comfortable using your solution in our production codebase.

# System Overview
## Sensor Power Enable
This GPIO enable line controls the power supply for our temperature sensor. It must be enabled for our temperature sensor to provide valid measurements. The sensor power supply takes 5 seconds to stabilize after a rising edge on the enable line. Control of this signal code is provided via the `GpioOutputInterface& sensor_power_enable` object.

## Temperature Sensor
An Analog Devices AD22100 temperature sensor is used by our system to sense the voltage of our inverter. The datasheet for this sensor is included. We are powering the sensor using a 5V supply voltage. Your code will be passed a `VoltageSensorInterface& temp_sensor_raw` reference that can be used to get the raw value of the voltage input. The datasheet will be required to parse usable temperature values.

## Fan Control Enable
First there is a GPIO enable line that controls a relay powering the fans. When this output is low, power is cut to the fans and they will stop, drawing no system power. When this output is high, power will be supplied to the fans. Control of this signal code is provided via the `GpioOutputInterface& fan_relay_enable` object.

## Fan Control PWM Duty Cycle
When power is provided to the fans they will start turning. The speed the fans turn at is determined by the duty cycle of the provided PWM signal. Control of this signal code is provided via the `PwmOutputInterface& fan_output_raw` object.

# Requirements
1. The controller shall enable sensor power prior to using temperature measurements for fan control and wait 5 seconds before considering a temperature measurement valid.
2. The controller shall enable the fan control relay when the temperature measurement is above 60C.
3. The controller shall disable the fan control relay when the temperature measurement is below 50C.
4. The controller shall implement the following table to determine output fan duty cycle to command at a given temperature.

| Temperature (t) | PWM Output = f(t)    |
|-----------------|----------------------|
| 50-99           | = (t-50) / 2         |
| 100-129         | = (t-98) * 2 + 20    |
| 130-140         | = 90                 |
| > 140           | = 100                |


# Usage of Controller Object
Your implementation of `FanController` will be used like the following example code. Feel free to further decompose functions into other classes or functions as you see fit. Moxion will interface with your controller via the `init()` and `loop()` methods and will be confirming correctness based on the signals of the GPIO and PWM outputs.

```
#include "fan_controller.h"
#include "drivers.h"

void run_test_demo_code(VoltageSensorInterface& temp_sensor_raw, GpioOutputInterface& sensor_power_enable, GpioOutputInterface& fan_relay_enable, PwmOutputInterface& fan_output_raw) {
  FanController controller(temp_sensor_raw, sensor_power_enable, fan_relay_enable, fan_output_raw);
  controller.init();
  while(true) {
    controller.loop();
    delay(500);
  }
}
```

# Instructions
1. Please implement `FanController` to the requirements above
2. Please write unit tests for your `FanController` class, test for failure modes!
3. Feel free to reach out to ask for questions or clarifications

# Assessment of submissions

Your work will be assessed on the following criteria:
1. Completeness and correctness of requirements implementation
2. Adherence to effective design principles and coding standards
3. Unit test coverage, and consideration of potential failure modes

# Compile and Run Test
mkdir build && cd build && cmake .. && cmake --build . && ctest
