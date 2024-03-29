/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <memory>

#include <libp2p/crypto/crypto_provider.hpp>
#include <libp2p/crypto/key_validator.hpp>

namespace libp2p::crypto::validator {
  class KeyValidatorImpl : public KeyValidator {
   public:
    explicit KeyValidatorImpl(std::shared_ptr<CryptoProvider> crypto_provider);

    outcome::result<void> validate(const PrivateKey &key) const override;

    outcome::result<void> validate(const PublicKey &key) const override;

    outcome::result<void> validate(const KeyPair &keys) const override;

   private:
    outcome::result<void> validateRsa(const PrivateKey &key) const;
    outcome::result<void> validateRsa(const PublicKey &key) const;

    outcome::result<void> validateEd25519(const PrivateKey &key) const;
    outcome::result<void> validateEd25519(const PublicKey &key) const;

    outcome::result<void> validateSecp256k1(const PrivateKey &key) const;
    outcome::result<void> validateSecp256k1(const PublicKey &key) const;

    outcome::result<void> validateEcdsa(const PrivateKey &key) const;
    outcome::result<void> validateEcdsa(const PublicKey &key) const;

    std::shared_ptr<CryptoProvider> crypto_provider_;
  };
}  // namespace libp2p::crypto::validator
