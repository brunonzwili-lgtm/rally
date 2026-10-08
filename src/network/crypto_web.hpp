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
//  No-op Crypto backend for the WebAssembly build.
//
//  Neither mbedTLS nor OpenSSL is available as an Emscripten port, so the
//  encrypted online-multiplayer transport cannot be built for the web. This
//  backend keeps the single-player/offline game compiling and running; any
//  attempt to establish an encrypted connection to a server fails cleanly.

#ifndef HEADER_CRYPTO_WEB_HPP
#define HEADER_CRYPTO_WEB_HPP

#include <enet/enet.h>

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class BareNetworkString;
class NetworkString;

class Crypto
{
private:
    static std::string m_client_key;
    static std::string m_client_iv;

    size_t m_tag_size;

public:
    Crypto(const std::vector<uint8_t>& key,
           const std::vector<uint8_t>& iv,
           size_t tag_size = 4) : m_tag_size(tag_size) {}
    ~Crypto() {}
    // ------------------------------------------------------------------------
    static std::string base64(const std::vector<uint8_t>& input);
    // ------------------------------------------------------------------------
    static std::vector<uint8_t> decode64(std::string input);
    // ------------------------------------------------------------------------
    static std::array<uint8_t, 32> sha256(const std::string& input);
    // ------------------------------------------------------------------------
    static std::unique_ptr<Crypto> getClientCrypto(size_t tag_size = 4)
    {
        // No key exchange is possible without a crypto backend.
        return std::unique_ptr<Crypto>(nullptr);
    }
    // ------------------------------------------------------------------------
    static void initClientAES() {}
    // ------------------------------------------------------------------------
    static void resetClientAES()
    {
        m_client_key = "";
        m_client_iv = "";
    }
    // ------------------------------------------------------------------------
    static const std::string& getClientKey()           { return m_client_key; }
    // ------------------------------------------------------------------------
    static const std::string& getClientIV()             { return m_client_iv; }
    // ------------------------------------------------------------------------
    bool encryptConnectionRequest(BareNetworkString& ns);
    // ------------------------------------------------------------------------
    bool decryptConnectionRequest(BareNetworkString& ns);
    // ------------------------------------------------------------------------
    ENetPacket* encryptSend(BareNetworkString& ns, bool reliable);
    // ------------------------------------------------------------------------
    NetworkString* decryptRecieve(ENetPacket* p);

}; // Crypto

#endif // HEADER_CRYPTO_WEB_HPP