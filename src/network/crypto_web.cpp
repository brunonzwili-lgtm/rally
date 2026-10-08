//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2018 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation, either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//  No-op Crypto backend for the WebAssembly build. See crypto_web.hpp.

#include "network/crypto_web.hpp"

#include "network/network_string.hpp"
#include "utils/log.hpp"

#include <cstring>

std::string Crypto::m_client_key;
std::string Crypto::m_client_iv;

// ------------------------------------------------------------------------
/** Encodes data as base64. Small self-contained implementation so the web
 *  build does not depend on mbedTLS.
 */
std::string Crypto::base64(const std::vector<uint8_t>& input)
{
    static const char table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string result;
    result.reserve(((input.size() + 2) / 3) * 4);

    size_t i = 0;
    while (i + 2 < input.size())
    {
        uint32_t triple = (uint32_t(input[i]) << 16) |
                          (uint32_t(input[i + 1]) << 8) |
                           uint32_t(input[i + 2]);
        result += table[(triple >> 18) & 0x3F];
        result += table[(triple >> 12) & 0x3F];
        result += table[(triple >> 6) & 0x3F];
        result += table[triple & 0x3F];
        i += 3;
    }

    const size_t remaining = input.size() - i;
    if (remaining == 1)
    {
        uint32_t triple = uint32_t(input[i]) << 16;
        result += table[(triple >> 18) & 0x3F];
        result += table[(triple >> 12) & 0x3F];
        result += "==";
    }
    else if (remaining == 2)
    {
        uint32_t triple = (uint32_t(input[i]) << 16) | (uint32_t(input[i + 1]) << 8);
        result += table[(triple >> 18) & 0x3F];
        result += table[(triple >> 12) & 0x3F];
        result += table[(triple >> 6) & 0x3F];
        result += "=";
    }
    return result;
}   // base64

// ------------------------------------------------------------------------
/** Decodes a base64 string. Returns an empty vector on invalid input. */
std::vector<uint8_t> Crypto::decode64(std::string input)
{
    auto value_of = [](char c) -> int
    {
        if (c >= 'A' && c <= 'Z') return c - 'A';
        if (c >= 'a' && c <= 'z') return c - 'a' + 26;
        if (c >= '0' && c <= '9') return c - '0' + 52;
        if (c == '+') return 62;
        if (c == '/') return 63;
        return -1;
    };

    std::vector<uint8_t> result;
    uint32_t buffer = 0;
    int bits = 0;
    for (char c : input)
    {
        if (c == '=' || c == '\n' || c == '\r') continue;
        const int v = value_of(c);
        if (v < 0) return std::vector<uint8_t>();
        buffer = (buffer << 6) | uint32_t(v);
        bits += 6;
        if (bits >= 8)
        {
            bits -= 8;
            result.push_back(uint8_t((buffer >> bits) & 0xFF));
        }
    }
    return result;
}   // decode64

// ------------------------------------------------------------------------
/** Not a real SHA-256: the web build has no crypto backend, so this returns
 *  a deterministic 32-byte digest derived from the input. It must never be
 *  used for security purposes.
 */
std::array<uint8_t, 32> Crypto::sha256(const std::string& input)
{
    std::array<uint8_t, 32> result{};
    uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis
    for (char c : input)
    {
        hash ^= uint8_t(c);
        hash *= 1099511628211ULL;            // FNV-1a prime
    }
    for (size_t i = 0; i < result.size(); i++)
    {
        hash ^= hash >> 33;
        result[i] = uint8_t((hash >> ((i % 8) * 8)) & 0xFF);
    }
    return result;
}   // sha256

// ------------------------------------------------------------------------
/** Without a cipher backend there is nothing to encrypt with, so refuse. */
bool Crypto::encryptConnectionRequest(BareNetworkString& /*ns*/)
{
    Log::warn("crypto", "Encryption is unavailable in the web build; "
            "online multiplayer is disabled.");
    return false;
}   // encryptConnectionRequest

// ------------------------------------------------------------------------
bool Crypto::decryptConnectionRequest(BareNetworkString& /*ns*/)
{
    return false;
}   // decryptConnectionRequest

// ------------------------------------------------------------------------
ENetPacket* Crypto::encryptSend(BareNetworkString& /*ns*/, bool /*reliable*/)
{
    return NULL;
}   // encryptSend

// ------------------------------------------------------------------------
NetworkString* Crypto::decryptRecieve(ENetPacket* /*p*/)
{
    return NULL;
}   // decryptRecieve