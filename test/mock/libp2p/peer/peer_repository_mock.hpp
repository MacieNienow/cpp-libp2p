/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <libp2p/peer/peer_repository.hpp>

#include <gmock/gmock.h>

namespace libp2p::peer {
  struct PeerRepositoryMock : public PeerRepository {
    ~PeerRepositoryMock() override = default;

    MOCK_METHOD0(getAddressRepository, AddressRepository &());

    MOCK_METHOD0(getKeyRepository, KeyRepository &());

    MOCK_METHOD0(getProtocolRepository, ProtocolRepository &());

    MOCK_CONST_METHOD0(getPeers, std::unordered_set<PeerId>());

    MOCK_CONST_METHOD1(getPeerInfo, PeerInfo(const PeerId &));
  };
}  // namespace libp2p::peer
