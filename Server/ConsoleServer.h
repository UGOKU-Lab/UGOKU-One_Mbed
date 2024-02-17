#include "Protocol.h"
#include "mbed.h"
#include <cstdint>

template <size_t CH_MAX = 256> class ConsoleServer {
  BufferedSerial _esp_serial;

  Protocol::Encoder _encoder;
  Protocol::Decoder _decoder;

  Callback<void(uint8_t, uint8_t)> _callback;

  uint8_t _buf[CH_MAX + 1];

public:
  int fails;

  /// Creates a server.
  ConsoleServer() : _esp_serial(PA_9, PA_10, 115200), _callback() {}

  /// Reads the latest value on the [channel].
  uint8_t read(uint8_t channel) {
    if (channel > CH_MAX)
      return 0;

    return _buf[channel];
  }

  /// Reads the ratio (0~1) of the latest value on the [channel].
  float read_ratio(uint8_t channel) { return (float)read(channel) / 255; }

  /// Sends the value currently on the [channel].
  void send(uint8_t channel, uint8_t value) {
    Protocol::Payload data(channel, value);
    _encoder.enqueue(data);

    size_t count = _encoder.count();
    uint8_t buf[count];
    uint8_t buf_size = _encoder.dequeue(buf, count);
    uint8_t buf_offset = 0;

    while (buf_size > 0) {
      ssize_t written = _esp_serial.write(buf + buf_offset, buf_size);

      if (written < 1) {
        continue;
      }

      buf_size -= written;
      buf_offset += written;
    }

    _buf[channel] = value;
  }

  /// Attaches the [callback] called on valid data received.
  void attach(Callback<void(uint8_t, uint8_t)> callback) {
    _callback = callback;
  }

  /// Directs the server to process the current task once.
  void task() {
    uint8_t receive_buf[1];

    while (_esp_serial.readable()) {
      int read = _esp_serial.read(receive_buf, 1);

      if (read < 0) {
        continue;
      }

      _decoder.enqueue(receive_buf[0]);

      while (_decoder.count() > 0) {
        Protocol::Payload decoded[1];
        size_t poped = _decoder.dequeue(decoded, 1);

        if (poped > 0) {
          uint8_t channel = decoded[0].channel;
          uint8_t value = decoded[0].value;

          if (_callback)
            _callback.call(channel, value);

          _buf[channel] = value;
        }
      }
    }
  }
};
