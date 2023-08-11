#include "GpioWrapper.h"
#include "Drivers.h"

GpioOutputWrapper::GpioOutputWrapper(GpioOutputInterface &gpio_intf)
    : gpio_intf_(gpio_intf) {}

void GpioOutputWrapper::setOutput(GpioState val) {
  gpio_intf_.setOutput(static_cast<bool>(val));
  curr_state = val;
}

GpioOutputWrapper::GpioState GpioOutputWrapper::currgpioval() {
  return curr_state;
}