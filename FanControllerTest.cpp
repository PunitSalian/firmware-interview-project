#include "FanController.h"
#include "Drivers.h"
#include "MockClass.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::SaveArg;

class FanControllerTest : public testing::Test {
public:
  MockVoltageSensor temp_sensor_raw;
  MockGpioOutput sensor_power_enable;
  MockGpioOutput fan_relay_enable;
  MockPwmOutput fan_output_raw;
  std::shared_ptr<FanController> controller;
  std::shared_ptr<MockLinuxCallIntf> linux_call;
  void SetUp() override {
    linux_call = std::make_shared<MockLinuxCallIntf>();
    controller = std::make_shared<FanController>(
        temp_sensor_raw, sensor_power_enable, fan_relay_enable, fan_output_raw,
        linux_call);
  }

  void TearDown() override {}
};

TEST_F(FanControllerTest, FanController_Init_Test) {

  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(*linux_call, currepochtime()).Times(1);
  EXPECT_CALL(sensor_power_enable, setOutput(true)).Times(1);
  controller->init();
}

TEST_F(FanControllerTest, FanController_Init_Not_Done_Test) {
  EXPECT_CALL(temp_sensor_raw, getVoltage()).Times(0);
  EXPECT_CALL(fan_relay_enable, setOutput(_)).Times(0);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);
  controller->loop();
}

TEST_F(FanControllerTest, FanController_Init_Done_Sensor_Initializing_Test) {

  // called 2 times one on init and once getTemperature is called returning
  // 0 and 2 to suggest sensor is Initializing as the time diff from init is
  // less than 5 seconds
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(2));
  // enabling sensor in init
  EXPECT_CALL(sensor_power_enable, setOutput(true)).Times(1);

  // disabling fan on init
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);

  // None of this call should take place as sensor is initializing
  EXPECT_CALL(temp_sensor_raw, getVoltage()).Times(0);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);

  controller->init();
  controller->loop();
}

TEST_F(FanControllerTest, FanRelayOff_minus_50C_Test) {

  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));

  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(0.25f));
  // Times 1 for fan enable being called off in init
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);

  // We will not set PWM as fan is off below 50c and fan is off
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);
  controller->init();
  controller->loop();
}

TEST_F(FanControllerTest, FanRelayOn_150C_100_percent_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));

  // 150C
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(4.75f));

  // from the init call
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  // from the loop call
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);

  // Expect pwm to be 100
  EXPECT_CALL(fan_output_raw, setOutputDuty(100)).Times(1);
  controller->init();
  controller->loop();
}

TEST_F(FanControllerTest, FanRelayOn_50C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));

  // from the init call
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  // 50C
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(2.5f));
  // Fan is in a off state so we not calling setOutput
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);
  controller->init();
  controller->loop();
}

TEST_F(FanControllerTest, FanRelayOn_65C_Duty_Cycle_Test) {

  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));

  // 65C
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(2.8375f));

  float pwm_val = 0;
  // from the init call
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);

  // enable the fan above 60C
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 7.5f);
}

TEST_F(FanControllerTest, FanRelayOn_99C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(3.6025f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 24.5f);
}

TEST_F(FanControllerTest, FanRelayOn_100C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(3.9625f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 54.0f);
}

TEST_F(FanControllerTest, FanRelayOn_115C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(3.9625f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 54.0f);
}

TEST_F(FanControllerTest, FanRelayOn_129C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(4.2775f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 82.0f);
}

TEST_F(FanControllerTest, FanRelayOn_130C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(4.3f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);

  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 90.0f);
}

TEST_F(FanControllerTest, FanRelayOn_135C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(4.4125f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);

  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 90.0f);
}

TEST_F(FanControllerTest, FanRelayOn_140C_Duty_Cycle_Test) {
  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(4.525f));
  float pwm_val = 0;
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);

  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).WillOnce(SaveArg<0>(&pwm_val));
  controller->init();
  controller->loop();
  EXPECT_FLOAT_EQ(pwm_val, 90.0f);
}

TEST_F(FanControllerTest, Sequential_Temp_Variation_Integration_Test) {

  // called 2 times one on init and once getTemperature is called returning
  // 0 and 5 to suggest sensor is powered on
  EXPECT_CALL(*linux_call, currepochtime())
      .Times(2)
      .WillOnce(testing::Return(0))
      .WillOnce(testing::Return(5));
  /*
      temperature -50 C expectation is the fan should be off
  */
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(0.25f));
  controller->init();
  controller->loop();

  /*
      temperature changes from -50 C to 54.44C
      Fan still remains off and we don't set pwm yet
  */
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(2.6f));
  EXPECT_CALL(fan_relay_enable, setOutput(_)).Times(0);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);
  controller->loop();

  /*
       temperature changes from 54.44 to 99C
       Fan will turn on with a set pwm.
   */
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(3.6f));
  EXPECT_CALL(fan_relay_enable, setOutput(true)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(1);
  controller->loop();

  /*
       temperature changes from 99C to 54.44C
       Fan will remain on with a set pwm.
   */
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(2.6f));
  EXPECT_CALL(fan_relay_enable, setOutput(_)).Times(0);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(1);
  controller->loop();

  /*
       temperature changes from 54.44C to -50C
       Fan will turn off
   */
  EXPECT_CALL(temp_sensor_raw, getVoltage()).WillOnce(testing::Return(0.25f));
  EXPECT_CALL(fan_relay_enable, setOutput(false)).Times(1);
  EXPECT_CALL(fan_output_raw, setOutputDuty(_)).Times(0);
  controller->loop();
}