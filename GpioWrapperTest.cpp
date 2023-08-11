#include "GpioWrapper.h"
#include "MockClass.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class GpioWrapperTest : public testing::Test {
public:
  MockGpioOutput sensor_power_enable;
  std::unique_ptr<GpioOutputWrapper> gpio;
  void SetUp() override {
    gpio = std::make_unique<GpioOutputWrapper>(sensor_power_enable);
  }

  void TearDown() override {}
};

TEST_F(GpioWrapperTest, TurnOn_Test) {
  gpio->setOutput(GpioOutputWrapper::GpioState::ON);
  EXPECT_EQ(gpio->currgpioval(), GpioOutputWrapper::GpioState::ON);
}

TEST_F(GpioWrapperTest, TurnOff_Test) {
  gpio->setOutput(GpioOutputWrapper::GpioState::OFF);
  EXPECT_EQ(gpio->currgpioval(), GpioOutputWrapper::GpioState::OFF);
}