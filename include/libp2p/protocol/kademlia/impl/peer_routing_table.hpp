/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <libp2p/event/bus.hpp>
#include <libp2p/protocol/kademlia/common.hpp>
#include <libp2p/protocol/kademlia/config.hpp>
#include <libp2p/protocol/kademlia/node_id.hpp>

namespace libp2p::event::protocol::kademlia {

  using PeerAddedChannel = channel_decl<struct PeerAdded, libp2p::peer::PeerId>;

  using PeerRemovedChannel =
      channel_decl<struct PeerRemoved, libp2p::peer::PeerId>;

}  // namespace libp2p::event::protocol::kademlia

namespace libp2p::protocol::kademlia {

  /**
   * @class PeerRoutingTable
   */
  struct PeerRoutingTable {
    virtual ~PeerRoutingTable() = default;

    /**
     * Adds/moves the given @param peer to the front of its respective bucket
     * @returns true/false if it was newadded peer or not
     */
    virtual outcome::result<bool> update(const peer::PeerId &peer,
                                         bool is_permanent,
                                         bool is_connected = false) = 0;

    /// Removes a peer with given @param peer from the routing table.
    /// This is to be used when we are sure a node has disconnected completely.
    virtual void remove(const peer::PeerId &peer) = 0;

    /// Returns all known peers
    virtual std::vector<peer::PeerId> getAllPeers() const = 0;

    /// Returns upto @param count closest peers to the given @param node
    virtual std::vector<peer::PeerId> getNearestPeers(const NodeId &node,
                                                      size_t count) = 0;

    /// Returns the total number of peers in the routing table
    virtual size_t size() const = 0;
  };

}  // namespace libp2p::protocol::kademlia
