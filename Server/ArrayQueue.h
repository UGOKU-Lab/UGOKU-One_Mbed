#include <stddef.h>
#include <string.h>

template <typename T, size_t N> class ArrayQueue {
  T _array[N];
  size_t _array_count;

public:
  ArrayQueue() : _array_count(0) {}

  void enqueue(T data) {
    if (is_full()) {
      return;
    }

    _array[_array_count] = data;
    _array_count++;
  }

  T dequeue() {
    if (is_empty()) {
      return T();
    }

    T result = _array[0];

    _array_count--;
    memcpy(_array, _array + 1, _array_count);

    return result;
  }

  T peek(int index) { return _array[index]; }

  bool is_full() { return _array_count >= N; }

  bool is_empty() { return _array_count <= 0; }

  size_t count() { return _array_count; }

  size_t size() { return N; }
};
