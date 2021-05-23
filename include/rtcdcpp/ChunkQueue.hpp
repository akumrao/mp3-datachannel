

/**
 * Simple blocking thread-safe queue.
 */

#pragma once

#include "Chunk.hpp"

#include <mutex>
#include <queue>

namespace rtcdcpp {

/**
 * Thread-Safe Queue of DataChunks
 */
class ChunkQueue {
 private:
  mutable std::mutex mut;
  std::queue<ChunkPtr> chunk_queue;
  std::condition_variable data_cond;
  bool stopping;

 public:
  ChunkQueue() : chunk_queue(), stopping(false) {}

  void Stop() {
    std::lock_guard<std::mutex> lock(mut);
    stopping = true;
    data_cond.notify_all();
  }

  void push(ChunkPtr chunk) {
    std::lock_guard<std::mutex> lock(mut);
    if (stopping) {
      return;
    }
    chunk_queue.push(chunk);
    data_cond.notify_one();
  }

  ChunkPtr wait_and_pop() {
    std::unique_lock<std::mutex> lock(mut);
    while (!stopping && chunk_queue.empty()) {
      data_cond.wait(lock);
    }

    if (stopping) {
      return ChunkPtr();
    }

    ChunkPtr res = chunk_queue.front();
    chunk_queue.pop();
    return res;
  }

  bool empty() const {
    std::lock_guard<std::mutex> lock(mut);
    return chunk_queue.empty();
  }
};
}
