/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <libp2p/crypto/crypto_provider.hpp>
#include <libp2p/crypto/key.hpp>
#include <libp2p/crypto/key_marshaller.hpp>
#include <libp2p/log/logger.hpp>
#include <libp2p/security/security_adaptor.hpp>

namespace libp2p::security {

  class Noise : public SecurityAdaptor,
                public std::enable_shared_from_this<Noise> {
   public:
    static constexpr auto kProtocolId = "/noise";

    Noise(crypto::KeyPair local_key,
          std::shared_ptr<crypto::CryptoProvider> crypto_provider,
          std::shared_ptr<crypto::marshaller::KeyMarshaller> key_marshaller);

    ~Noise() override = default;

    peer::ProtocolName getProtocolId() const override;

    void secureInbound(std::shared_ptr<connection::LayerConnection> inbound,
                       SecConnCallbackFunc cb) override;

    void secureOutbound(std::shared_ptr<connection::LayerConnection> outbound,
                        const peer::PeerId &p,
                        SecConnCallbackFunc cb) override;

   private:
    log::Logger log_ = log::createLogger("Noise");
    libp2p::crypto::KeyPair local_key_;
    std::shared_ptr<crypto::CryptoProvider> crypto_provider_;
    std::shared_ptr<crypto::marshaller::KeyMarshaller> key_marshaller_;
  };

}  // namespace libp2p::security
