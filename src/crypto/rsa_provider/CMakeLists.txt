#
# Copyright Quadrivium LLC
# All Rights Reserved
# SPDX-License-Identifier: Apache-2.0
#

libp2p_add_library(p2p_rsa_provider
    rsa_provider_impl.cpp
    )

target_link_libraries(p2p_rsa_provider
    p2p_sha
    p2p_crypto_error
    OpenSSL::Crypto
    )
