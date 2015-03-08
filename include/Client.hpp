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

#ifndef PONG_CLIENT_HPP
#define PONG_CLIENT_HPP

#include <Bit/Build.hpp>
#include <Bit/Network/net/Client.hpp>
#include <Ball.hpp>
#include <Player.hpp>

namespace Pong
{

	// Forward declarations
	class Server;

	////////////////////////////////////////////////////////////////
	/// \ingroup Pong
	/// \brief Pong client class
	///
	////////////////////////////////////////////////////////////////
	class Client : public Bit::Net::Client
	{

	public:

		////////////////////////////////////////////////////////////////
		/// \brief Default constructor.
		///
		////////////////////////////////////////////////////////////////
		Client( );

		////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		///
		////////////////////////////////////////////////////////////////
		~Client( );

		////////////////////////////////////////////////////////////////
		/// \brief Connect to server.
		///
		////////////////////////////////////////////////////////////////
		Bit::Bool Join(	Server * p_pServer,
						const Bit::Address & p_Address,
						const Bit::Uint16 p_Port,
						const Bit::Time & p_Timeout );

		////////////////////////////////////////////////////////////////
		/// \brief Run the game client.
		///
		////////////////////////////////////////////////////////////////
		Bit::Bool Run( );

	private:

		Server *	m_pServer;
		Ball *		m_pBall;
		Player *	m_pPlayers[ 2 ];

	};

}

#endif