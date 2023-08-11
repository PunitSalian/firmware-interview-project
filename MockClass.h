#pragma once
#include "Drivers.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

// Mock class for VoltageSensorInterface
class MockVoltageSensor : public VoltageSensorInterface {
public:
  MOCK_METHOD(float, getVoltage, (), (override));
};

// Mock class for PwmOutputInterface
class MockPwmOutput : public PwmOutputInterface {
public:
  MOCK_METHOD(void, setOutputDuty, (float duty), (override));
};

// Mock class for GpioOutputInterface
class MockGpioOutput : public GpioOutputInterface {
public:
  MOCK_METHOD(void, setOutput, (bool output), (override));
};

class MockLinuxCallIntf : public LinuxCallIntf {
public:
  MOCK_METHOD(std::time_t, currepochtime, (), (override));
};