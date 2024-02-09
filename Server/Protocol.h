#include "ArrayQueue.h"
#include <stdint.h>

namespace Protocol {

struct Payload {
  uint8_t channel;
  uint8_t value;

  Payload() {}

  Payload(uint8_t channel, uint8_t value) : channel(channel), value(value) {}

  uint8_t checksum() { return channel ^ value; }
};

class Encoder {
  ArrayQueue<uint8_t, 10> _send_buf;

public:
  /// Adds the [data] to the send buffer.
  ///
  /// Returns the number of bytes added to the send buffer.
  size_t enqueue(Payload data) {
    if (_send_buf.size() - _send_buf.count() < 3) {
      return 0;
    }

    _send_buf.enqueue(data.channel);
    _send_buf.enqueue(data.value);
    _send_buf.enqueue(data.checksum());

    return 3;
  }

  /// Removes the data of [size] from the send buffer to the [dest].
  ///
  /// Returns the number of the data actually removed.
  size_t dequeue(uint8_t *dest, size_t size) {
    size_t count = size < _send_buf.count() ? size : _send_buf.count();

    for (size_t i = 0; i < count; ++i) {
      dest[i] = _send_buf.dequeue();
    }

    return count;
  }

  /// Returns the number of the bytes in the send buffer.
  size_t count() { return _send_buf.count(); }
};

class Decoder {
  ArrayQueue<uint8_t, 3> _raw_buf;
  ArrayQueue<Payload, 10> _receive_buf;

public:
  /// Adds the [data] to the receive buffer.
  ///
  /// Returns the number of data added to the receive buffer.
  size_t enqueue(uint8_t data) {
    if (_receive_buf.size() - _receive_buf.count() < 1) {
      return 0;
    }

    _raw_buf.enqueue(data);

    if (_raw_buf.count() < 3) {
      return 0;
    }

    Payload decoded;
    decoded.channel = _raw_buf.peek(0);
    decoded.value = _raw_buf.peek(1);
    uint8_t checksum = _raw_buf.peek(2);

    if (decoded.checksum() != checksum) {
      _raw_buf.dequeue();
      return 0;
    }

    _raw_buf.dequeue();
    _raw_buf.dequeue();
    _raw_buf.dequeue();

    _receive_buf.enqueue(decoded);

    return 1;
  }

  /// Removes the data of [size] from the receive buffer to the [dest].
  ///
  /// Returns the number of the data actually removed.
  size_t dequeue(Payload *dest, size_t size) {
    size_t count = size < _receive_buf.count() ? size : _receive_buf.count();

    for (size_t i = 0; i < count; ++i) {
      dest[i] = _receive_buf.dequeue();
    }

    return count;
  }

  /// Returns the number of the data in the receive buffer.
  size_t count() { return _receive_buf.count(); }
};

} // namespace Protocol
