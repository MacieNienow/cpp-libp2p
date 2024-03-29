/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <boost/signals2.hpp>

namespace libp2p::event {

  /**
   * Subscription to some event
   */
  class Subscription {
   public:
    explicit Subscription(boost::signals2::connection conn)  // NOLINT (moved)
        : connection_{std::move(conn)} {}

    /**
     * Unsubscribe from the event
     */
    void unsubscribe() {
      connection_.disconnect();
    }

   private:
    boost::signals2::connection connection_;
  };

}  // namespace libp2p::event
