#include "Protocol.h"
#include "mbed.h"

class ConsoleServer {
  UnbufferedSerial _esp_serial;

  Protocol::Encoder _encoder;
  Protocol::Decoder _decoder;

  Callback<void(uint8_t, uint8_t)> _callback;

public:
  /// Creates a server.
  ConsoleServer() : _esp_serial(PA_9, PA_10, 115200), _callback() {
    _esp_serial.attach(Callback<void()>(this, &ConsoleServer::_task));
  }

  /// Sends the [value] as data on the [channel].
  void send(uint8_t channel, uint8_t value) {
    Protocol::Payload data(channel, value);
    _encoder.enqueue(data);

    size_t count = _encoder.count();
    uint8_t buf[count];
    uint8_t buf_size = _encoder.dequeue(buf, count);

    _esp_serial.write(buf, buf_size);
  }

  /// Attaches the [callback] called on valid data received.
  void attach(Callback<void(uint8_t, uint8_t)> callback) {
    _callback = callback;
  }

private:
  void _task() {
    uint8_t receive_buf[1];

    while (_esp_serial.readable()) {
      _esp_serial.read(receive_buf, 1);
      _decoder.enqueue(receive_buf[0]);

      while (_decoder.count() > 0) {
        Protocol::Payload decoded[1];
        size_t poped = _decoder.dequeue(decoded, 1);

        if (poped > 0 && _callback) {
          _callback.call(decoded[0].channel, decoded[0].value);
        }
      }
    }
  }
};
