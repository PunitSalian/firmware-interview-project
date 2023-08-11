#include "TempSensor.h"
#include "MockClass.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class TempSensorTest : public testing::Test {
public:
  std::unique_ptr<TempSensor> temp_sensor;
  MockGpioOutput sensor_power_enable;
  MockVoltageSensor temp_sensor_raw;
  std::shared_ptr<MockLinuxCallIntf> linux_call;
  void SetUp() override {
    linux_call = std::make_shared<MockLinuxCallIntf>();
    temp_sensor = std::make_unique<TempSensor>(temp_sensor_raw,
                                               sensor_power_enable, linux_call);
  }

  void TearDown() override {}
};

TEST_F(TempSensorTest, Initiailzing_Get_Temperature_Test) {
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(2));
  temp_sensor->SensorOn();

  auto val = temp_sensor->getTemperature();

  EXPECT_EQ(std::get<0>(val), TempSensor::SensorState::INITIALIZING);
}

TEST_F(TempSensorTest, Powered_On_Get_Temperature_Test) {
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(sensor_power_enable, setOutput(true)).Times(1);
  // -50C
  EXPECT_CALL(temp_sensor_raw, getVoltage())
      .Times(1)
      .WillOnce(testing::Return(0.25f));

  temp_sensor->SensorOn();

  auto val = temp_sensor->getTemperature();

  EXPECT_FLOAT_EQ(std::get<1>(val), -50.0f);
}

TEST_F(TempSensorTest, PowerOff_Get_Temperature_Test) {

  temp_sensor->SensorOff();

  auto val = temp_sensor->getTemperature();

  EXPECT_EQ(std::get<0>(val), TempSensor::SensorState::POWEROFF);
}
