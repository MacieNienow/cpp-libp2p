/**
 * Copyright Quadrivium LLC
 * All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 */

#include <libp2p/multi/converters/converter_utils.hpp>

#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <libp2p/common/hexutil.hpp>
#include <libp2p/common/types.hpp>
#include <libp2p/multi/converters/conversion_error.hpp>
#include <libp2p/multi/converters/dns_converter.hpp>
#include <libp2p/multi/converters/ip_v4_converter.hpp>
#include <libp2p/multi/converters/ip_v6_converter.hpp>
#include <libp2p/multi/converters/ipfs_converter.hpp>
#include <libp2p/multi/converters/tcp_converter.hpp>
#include <libp2p/multi/converters/udp_converter.hpp>
#include <libp2p/multi/multiaddress_protocol_list.hpp>
#include <libp2p/multi/multibase_codec/multibase_codec_impl.hpp>
#include <libp2p/multi/uvarint.hpp>
#include <libp2p/outcome/outcome.hpp>

using libp2p::common::hex_upper;
using libp2p::common::unhex;

namespace libp2p::multi::converters {

  outcome::result<Bytes> multiaddrToBytes(std::string_view str) {
    if (str.empty() || str[0] != '/') {
      return ConversionError::ADDRESS_DOES_NOT_BEGIN_WITH_SLASH;
    }

    str.remove_prefix(1);
    if (str.empty()) {
      return ConversionError::EMPTY_PROTOCOL;
    }

    if (str.back() == '/') {
      // for split does not recognize an empty token in the end
      str.remove_suffix(1);
    }

    Bytes processed;

    enum class WordType { PROTOCOL, ADDRESS };
    WordType type = WordType::PROTOCOL;

    const Protocol *protx = nullptr;

    std::list<std::string> tokens;
    boost::algorithm::split(tokens, str, boost::algorithm::is_any_of("/"));

    for (auto &word : tokens) {
      if (type == WordType::PROTOCOL) {
        if (word.empty()) {
          return ConversionError::EMPTY_PROTOCOL;
        }
        protx = ProtocolList::get(word);
        if (protx != nullptr) {
          auto code = UVarint(static_cast<uint64_t>(protx->code)).toVector();
          for (auto byte : code) {
            processed.emplace_back(byte);
          }
          if (protx->size != 0) {
            type = WordType::ADDRESS;  // Since the next word will be an address
          }
        } else {
          return ConversionError::NO_SUCH_PROTOCOL;
        }
      } else {
        if (word.empty()) {
          return ConversionError::EMPTY_ADDRESS;
        }
        OUTCOME_TRY(bytes, addressToBytes(*protx, word));
        for (auto byte : bytes) {
          processed.emplace_back(byte);
        }
        protx = nullptr;  // Since right now it doesn't need that
        // assignment anymore.
        type = WordType::PROTOCOL;  // Since the next word will be an protocol
      }
    }

    if (type == WordType::ADDRESS) {
      return ConversionError::EMPTY_ADDRESS;
    }

    return processed;
  }

  outcome::result<Bytes> addressToBytes(const Protocol &protocol,
                                        std::string_view addr) {
    // TODO(Akvinikym) 25.02.19 PRE-49: add more protocols
    switch (protocol.code) {
      case Protocol::Code::IP4:
        return IPv4Converter::addressToBytes(addr);
      case Protocol::Code::IP6:
        return IPv6Converter::addressToBytes(addr);
      case Protocol::Code::TCP:
        return TcpConverter::addressToBytes(addr);
      case Protocol::Code::UDP:
        return UdpConverter::addressToBytes(addr);
      case Protocol::Code::P2P:
        return IpfsConverter::addressToBytes(addr);

      case Protocol::Code::DNS:
      case Protocol::Code::DNS4:
      case Protocol::Code::DNS6:
      case Protocol::Code::DNS_ADDR:
      case Protocol::Code::UNIX:
      case Protocol::Code::X_PARITY_WS:
      case Protocol::Code::X_PARITY_WSS:
        return DnsConverter::addressToBytes(addr);

      case Protocol::Code::IP6_ZONE:
      case Protocol::Code::ONION3:
      case Protocol::Code::GARLIC64:
      case Protocol::Code::QUIC:
      case Protocol::Code::WS:
      case Protocol::Code::WSS:
      case Protocol::Code::P2P_WEBSOCKET_STAR:
      case Protocol::Code::P2P_STARDUST:
      case Protocol::Code::P2P_WEBRTC_DIRECT:
      case Protocol::Code::P2P_CIRCUIT:
        return ConversionError::NOT_IMPLEMENTED;
      default:
        return ConversionError::NO_SUCH_PROTOCOL;
    }
  }

  outcome::result<std::string> addressToHex(const Protocol &protocol,
                                            std::string_view addr) {
    OUTCOME_TRY(bytes, addressToBytes(protocol, addr));
    std::string hex;
    hex.reserve(bytes.size() * 2);
    boost::algorithm::hex_lower(
        bytes.begin(), bytes.end(), std::back_inserter(hex));
    return std::move(hex);
  }

  outcome::result<std::string> bytesToMultiaddrString(BytesIn bytes) {
    std::string results;

    size_t lastpos = 0;

    // set up variables
    const std::string hex = hex_upper(bytes);

    // Process Hex String
    while (lastpos < bytes.size() * 2) {
      BytesIn pid_bytes{bytes};
      auto protocol_int = UVarint(pid_bytes.subspan(lastpos / 2)).toUInt64();
      const Protocol *protocol =
          ProtocolList::get(static_cast<Protocol::Code>(protocol_int));
      if (protocol == nullptr) {
        return ConversionError::NO_SUCH_PROTOCOL;
      }

      if (protocol->code != Protocol::Code::P2P) {
        lastpos += (UVarint::calculateSize(pid_bytes.subspan(lastpos / 2)) * 2);
        std::string address;
        address = hex.substr(lastpos, protocol->size / 4);

        lastpos = lastpos + (protocol->size / 4);

        results += "/";
        results += protocol->name;

        if (protocol->size == 0) {
          continue;
        }

        try {
          switch (protocol->code) {
            case Protocol::Code::DNS:
            case Protocol::Code::DNS4:
            case Protocol::Code::DNS6:
            case Protocol::Code::DNS_ADDR: {
              // fetch the size of the address based on the varint prefix
              auto prefixedvarint = hex.substr(lastpos, 2);
              OUTCOME_TRY(prefixBytes, unhex(prefixedvarint));

              auto addrsize = UVarint(prefixBytes).toUInt64();

              // get the ipfs address as hex values
              auto hex_domain_name = hex.substr(lastpos + 2, addrsize * 2);
              OUTCOME_TRY(domain_name, unhex(hex_domain_name));

              lastpos += addrsize * 2 + 2;

              // Add domain name
              results += "/";
              results += std::string(domain_name.begin(), domain_name.end());

              auto i = std::find_if_not(
                  domain_name.begin(), domain_name.end(), [](auto c) {
                    return std::isalnum(c) || c == '-' || c == '.';
                  });
              if (i != domain_name.end()) {
                return ConversionError::INVALID_ADDRESS;
              }
              break;
            }

            case Protocol::Code::X_PARITY_WS:
            case Protocol::Code::X_PARITY_WSS: {
              // fetch the size of the address based on the varint prefix
              auto prefixedvarint = hex.substr(lastpos, 2);
              OUTCOME_TRY(prefixBytes, unhex(prefixedvarint));

              auto addrsize = UVarint(prefixBytes).toUInt64();

              // get the ipfs address as hex values
              auto hex_domain_name = hex.substr(lastpos + 2, addrsize * 2);
              OUTCOME_TRY(domain_name, unhex(hex_domain_name));

              lastpos += addrsize * 2 + 2;

              results += "/";
              results += std::string(domain_name.begin(), domain_name.end());
              break;
            }

            case Protocol::Code::IP4: {
              // Add IP
              results += "/";
              results += boost::asio::ip::make_address_v4(
                             std::stoul(address, nullptr, 16))
                             .to_string();
              break;
            }

            case Protocol::Code::IP6: {
              // Add IP
              OUTCOME_TRY(addr_bytes, unhex(address));
              std::array<uint8_t, 16> arr{};
              std::copy_n(addr_bytes.begin(), 16, arr.begin());
              results += "/";
              results += boost::asio::ip::make_address_v6(arr).to_string();
              break;
            }

            case Protocol::Code::TCP:
            case Protocol::Code::UDP: {
              // Add port
              results += "/";
              results += std::to_string(std::stoul(address, nullptr, 16));
              break;
            }

            case Protocol::Code::QUIC:
            case Protocol::Code::WS:
            case Protocol::Code::WSS:
              // No details
              break;

            default:
              return ConversionError::NOT_IMPLEMENTED;
          }
        } catch (const std::exception &e) {
          return ConversionError::INVALID_ADDRESS;
        }

      } else {
        lastpos = lastpos + 4;
        // fetch the size of the address based on the varint prefix
        auto prefixedvarint = hex.substr(lastpos, 2);
        OUTCOME_TRY(prefixBytes, unhex(prefixedvarint));

        auto addrsize = UVarint(prefixBytes).toUInt64();
        // get the ipfs address as hex values
        auto ipfsAddr = hex.substr(lastpos + 2, addrsize * 2);

        // convert the address from hex values to a binary array
        OUTCOME_TRY(addrbuf, unhex(ipfsAddr));
        auto encode_res = MultibaseCodecImpl{}.encode(
            addrbuf, MultibaseCodecImpl::Encoding::BASE58);
        encode_res.erase(0, 1);  // because multibase contains a char that
                                 // denotes which base is used
        results += "/p2p/" + encode_res;
        lastpos += addrsize * 2 + 2;
      }
    }

    return results;
  }

  outcome::result<std::string> bytesToMultiaddrString(const Bytes &bytes) {
    return bytesToMultiaddrString(BytesIn(bytes));
  }
}  // namespace libp2p::multi::converters
