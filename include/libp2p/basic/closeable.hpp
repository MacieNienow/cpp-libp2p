/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>

#include <libp2p/outcome/outcome.hpp>

namespace libp2p::basic {

  class Closeable {
   public:
    virtual ~Closeable() = default;

    /**
     * @brief Function that is used to check if current object is closed.
     * @return true if closed, false otherwise
     */
    virtual bool isClosed() const = 0;

    /**
     * @brief Closes current object
     * @return nothing or error
     */
    virtual outcome::result<void> close() = 0;
  };

}  // namespace libp2p::basic
