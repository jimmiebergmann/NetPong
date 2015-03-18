// Copyright (C) 2013 Jimmie Bergmann - jimmiebergmann@gmail.com
//
// This software is provided 'as-is', without any express or
// implied warranty. In no event will the authors be held
// liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute
// it freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but
//    is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any
//    source distribution.
// ///////////////////////////////////////////////////////////////////////////

#ifndef PONG_INIT_MESSAGE_LISTENER_HPP
#define PONG_INIT_MESSAGE_LISTENER_HPP

#include <Bit/Network/Net/HostMessageListener.hpp>

namespace Pong
{

	// Forward declarations
	class Client;

	// Player user message
	class InitMessageListener : public Bit::Net::HostMessageListener
	{

	public:

		InitMessageListener( Client * p_pClient );

		virtual void HandleMessage( Bit::Net::HostMessageDecoder & p_Message );

	private:

		Client * m_pClient;

	};

}

#endif