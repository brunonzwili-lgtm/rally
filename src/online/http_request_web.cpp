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
//  HTTP backend for the WebAssembly build.
//
//  Emscripten has no libcurl port, so this backend reports every request as
//  failed. That keeps the game running offline; anything that needs the
//  network (addons, server lists, news) is unavailable in the browser build.

#ifndef APPLE_NETWORK_LIBRARIES

#include "online/http_request.hpp"

#include "online/request_manager.hpp"
#include "utils/log.hpp"

// ============================================================================
bool Online::globalHTTPRequestInit()
{
    return true;
}   // globalHTTPRequestInit

// ============================================================================
void Online::globalHTTPRequestCleanup()
{
}   // globalHTTPRequestCleanup

// ----------------------------------------------------------------------------
/** Reports the request as finished and unsuccessful. */
void Online::HTTPRequest::operation()
{
    Log::error("HTTPRequest", "HTTP requests are unavailable in the web build "
            "(no libcurl backend): %s", getDownloadErrorMessage());
    setProgress(-1.0f);
}   // operation

// ----------------------------------------------------------------------------
const char* Online::HTTPRequest::getDownloadErrorMessage() const
{
    return "Network requests are not available in the web build.";
}   // getDownloadErrorMessage

#endif // !APPLE_NETWORK_LIBRARIES