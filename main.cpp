#include "Server/ConsoleServer.h"
#include "channel.h"
#include "mbed.h"
#include "pin.h"
#include <chrono>
#include <cstdint>

PwmOut led(PIN_LED1);
PwmOut servo(PIN_SERVO1);
PwmOut motor(PIN_MOTOR1);
AnalogIn sensor(PIN_ANALOG1);

ConsoleServer<CH_MAX> server;

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
      motor.write(0);
      continue;
    }

    led.write(0);  // status: connected
    servo.pulsewidth_us(server.read_ratio(CH_SERVO1) * 1000 + 1000);
    motor.write(server.read(CH_MOTOR1));

    server.send(CH_SENSOR, sensor.read() * 255);

    wait_us(100000);
  }
}
