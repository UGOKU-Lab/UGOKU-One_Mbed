#include "Server/ConsoleServer.h"
#include "mbed.h"

PwmOut led(PA_7);

// Data map.
uint8_t values[256] = {};

// A function callbacked on data received.
void on_receive(uint8_t channel, uint8_t value) {
  values[channel] = value;
}

int main() {
  ConsoleServer server;
  server.attach(&on_receive);

  while (true) {
    // Write LED with a value on channel 0.
    led.write((float)values[0] / 255);

    // Echo back to channel 1.
    values[1] = 255 - values[0];
    server.send(1, values[1]);

    printf("c, v: %d, %d\r\n", 0, values[0]);
    wait_us(100000);
  }
}
