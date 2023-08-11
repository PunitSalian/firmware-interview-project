#pragma once
#include "Drivers.h"
// Wrapper/adapter class to hold the last value to
// toggle gpio only on last state
class GpioOutputWrapper {
public:
  enum class GpioState : bool { OFF = false, ON = true };
  GpioOutputWrapper(GpioOutputInterface &gpio_intf);

  void setOutput(GpioState val);

  GpioState currgpioval();

private:
  GpioOutputInterface &gpio_intf_;
  GpioState curr_state = GpioState::OFF;
};
