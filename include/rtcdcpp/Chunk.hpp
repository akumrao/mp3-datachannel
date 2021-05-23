
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>

#include <cstring>
#include <condition_variable>
namespace rtcdcpp {

// Utility class for passing messages around
class Chunk {
 private:
  size_t len{0};
  uint8_t *data{nullptr};

 public:
  // TODO memory pool?
  // XXX should we just use a vector?

  // Makes a copy of data
  Chunk(const void *dataToCopy, size_t dataLen) : len(dataLen), data(new uint8_t[len]) { memcpy(data, dataToCopy, dataLen); }

  // Copy constructor
  Chunk(const Chunk &other) : len(other.len), data(new uint8_t[len]) { memcpy(data, other.data, other.len); }

  // Assignment operator
  Chunk &operator=(const Chunk &other) {
    if (data) {
      len = 0;
      delete[] data;
    }
    len = other.len;
    data = new uint8_t[len];
    memcpy(data, other.data, other.len);
    return *this;
  }

  ~Chunk() { delete[] data; }

  size_t Size() const { return len; }
  size_t Length() const { return Size(); }
  uint8_t *Data() const { return data; }
};

using ChunkPtr = std::shared_ptr<Chunk>;
}
