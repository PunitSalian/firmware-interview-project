#pragma once
#include <ctime>
/**
 * @brief Interface for getting a raw voltage sensor measurement
 */
class VoltageSensorInterface {

public:
  /**
   * @brief Get the most recent voltage measurement
   * @return Latest voltage measurement [0 12]
   */
  virtual float getVoltage() { return 0.0f; };
};

/**
 * @brief Interface for commanding a PWM output
 */
class PwmOutputInterface {

public:
  /**
   * @brief Set the Output Duty object
   * @param duty Duty cycle in percent [0 100]
   */
  virtual void setOutputDuty(float duty){};
};

/**
 * @brief Interface for commanding a GPIO output
 */
class GpioOutputInterface {

public:
  /**
   * @brief Set the Output object
   * @param output True to enable output, False to disable output
   */
  virtual void setOutput(bool output){};
};

/**
 * @brief Mock class for Linux call this is used so that we can mock
 *        Linux calls inside our logic
 */
class LinuxCallIntf {
public:
  virtual std::time_t currepochtime() = 0;
};

class LinuxCallProd : public LinuxCallIntf {
  std::time_t currepochtime() override { return std::time(nullptr); }
};
