/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <libp2p/transport/transport_listener.hpp>

#include <gmock/gmock.h>

namespace libp2p::transport {

  struct TransportListenerMock : public TransportListener {
    ~TransportListenerMock() override = default;

    MOCK_METHOD1(listen, outcome::result<void>(const multi::Multiaddress &));

    MOCK_CONST_METHOD1(canListen, bool(const multi::Multiaddress &address));

    MOCK_CONST_METHOD0(getListenMultiaddr,
                       outcome::result<multi::Multiaddress>());

    MOCK_CONST_METHOD0(isClosed, bool());

    MOCK_METHOD0(close, outcome::result<void>());
  };

}  // namespace libp2p::transport
