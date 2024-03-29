/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <functional>
#include <memory>

#include <libp2p/outcome/outcome.hpp>
#include <libp2p/peer/protocol.hpp>

namespace libp2p::connection {
  class Stream;
}  // namespace libp2p::connection

namespace libp2p {
  /**
   * Stream pointer and protocol.
   * Used by `Host::newStream`, `Dialer::newStream`, `Host::setProtocolHandler`,
   * `Router::setProtocolHandler`.
   */
  struct StreamAndProtocol {
    std::shared_ptr<connection::Stream> stream;
    peer::ProtocolName protocol;
  };

  using StreamAndProtocolOrError = outcome::result<StreamAndProtocol>;

  /**
   * Callback for `newStream`.
   */
  using StreamAndProtocolOrErrorCb =
      std::function<void(StreamAndProtocolOrError)>;

  /**
   * Callback for `setProtocolHandler`.
   */
  using StreamAndProtocolCb = std::function<void(StreamAndProtocol)>;
}  // namespace libp2p
