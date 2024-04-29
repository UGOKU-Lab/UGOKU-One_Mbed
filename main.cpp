#include "Server/ConsoleServer.h"
#include "channel.h"
#include "mbed.h"
#include "pin.h"
#include <chrono>
#include <cstdint>
#include "motor_driver.h"

PwmOut led(PIN_LED1);
PwmOut servo(PIN_SERVO1);
AnalogIn sensor(PIN_ANALOG1);

// Motor pins V.0.4.4 and Later
PinName pwm_pin = PB_0, forward_pin = PA_5, backward_pin = PA_8;
PinName pwm_pin_2 = PB_1, forward_pin_2 = PB_4, backward_pin_2 = PB_5;

/*
// Motor pins V.0.4.3 
PinName pwm_pin = PB_1, forward_pin = PB_4, backward_pin = PA_5;
PinName pwm_pin_2 = PB_0, forward_pin_2 = PA_8, backward_pin_2 = PB_5;
*/

ConsoleServer<CH_MAX> server;

// Motor driver.
MotorDriver driver(forward_pin, backward_pin, pwm_pin);
MotorDriver driver2(forward_pin_2, backward_pin_2, pwm_pin_2);


bool is_connected() {
  return server.read(CH_CONNECTED) != 0;
}

inline void setup() {
  servo.period_ms(20);
  servo.pulsewidth_us(1500);
}

int main() {
  setup();

  while (true) {
    server.task();

    if (!is_connected()) {
      led.write(1);  // status: disconnected
      servo.pulsewidth_us(1500);
      driver.drive(0);
      driver.drive(0);
      continue;
    }

    led.write(0);  // status: connected
    servo.pulsewidth_us((int)((float)server.read(CH_SERVO1)/90 * 1000 + 500));
    driver.drive(2 * server.read_ratio(CH_MOTOR1) - 1);
    driver2.drive(2 * server.read_ratio(CH_MOTOR2) - 1);
    server.send(CH_SENSOR, 1/sensor.read()*7);
    wait_us(1000);
  }
}
