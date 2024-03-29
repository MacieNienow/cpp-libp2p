/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <deque>
#include <vector>

#include <boost/optional.hpp>

#include <libp2p/common/types.hpp>

namespace libp2p::basic {

  class ReadBuffer {
   public:
    static constexpr size_t kDefaultAllocGranularity = 65536;

    ReadBuffer(const ReadBuffer &) = delete;
    ReadBuffer &operator=(const ReadBuffer &) = delete;

    ~ReadBuffer() = default;
    ReadBuffer(ReadBuffer &&) = default;
    ReadBuffer &operator=(ReadBuffer &&) = default;

    explicit ReadBuffer(size_t alloc_granularity = kDefaultAllocGranularity);

    size_t size() const {
      return total_size_;
    }

    bool empty() const {
      return total_size_ == 0;
    }

    /// Adds new data to the buffer
    void add(BytesIn bytes);

    /// Returns # of bytes actually copied into out
    size_t consume(BytesOut out);

    /// Returns # of bytes actually copied into out
    size_t addAndConsume(BytesIn in, BytesOut out);

    /// Clears and deallocates
    void clear();

   private:
    using Fragment = std::vector<uint8_t>;

    /// Consumes all data into out
    size_t consumeAll(BytesOut out);

    /// Consumes the 1st fragment or part of it
    size_t consumePart(uint8_t *out, size_t n);

    /// Granularity for coarse allocation
    size_t alloc_granularity_;

    /// Total size of unconsumed bytes
    size_t total_size_;

    /// The 1st fragment may advance
    size_t first_byte_offset_;

    /// Available allocated bytes remains in the last fragment
    size_t capacity_remains_;

    /// Fragments allocated
    std::deque<Fragment> fragments_;
  };

  /// Temporary buffer for incoming messages, filled from incoming (network)
  /// data up to expected size
  class FixedBufferCollector {
   public:
    using Buffer = std::vector<uint8_t>;

    static constexpr size_t kDefaultMemoryThreshold = 65536;

    explicit FixedBufferCollector(
        size_t expected_size = 0,
        size_t memory_threshold = kDefaultMemoryThreshold);

    /// Expects the next message of a given size, if the current one is
    /// not read to the end, it will be discarded
    void expect(size_t size);

    /// Fills the buffer (if read partially) with head bytes of data,
    /// returns data if filled up to expected size or empty option if not,
    /// modifies data (cuts head)
    /// Data returned is valid until next expect() call && data is live
    boost::optional<BytesIn> add(BytesIn &data);
    boost::optional<BytesOut> add(BytesOut &data);

    /// Resets to initial state
    void reset();

   private:
    /// If buffer memory allocated is above this threshold,
    /// it will be freed on the next expect() call
    size_t memory_threshold_;

    /// Size expected
    size_t expected_size_;

    /// The buffer
    Buffer buffer_;
  };

}  // namespace libp2p::basic
