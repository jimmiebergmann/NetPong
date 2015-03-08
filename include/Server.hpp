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

#ifndef PONG_SERVER_HPP
#define PONG_SERVER_HPP

#include <Bit/Build.hpp>
#include <Bit/Network/net/Server.hpp>
#include <Bit/System/Thread.hpp>
#include <Ball.hpp>
#include <Player.hpp>

namespace Pong
{

	////////////////////////////////////////////////////////////////
	/// \ingroup Pong
	/// \brief Pong server class
	///
	////////////////////////////////////////////////////////////////
	class Server : public Bit::Net::Server
	{

	public:

		// Friend classes
		friend class PlayerMessageListener;

		////////////////////////////////////////////////////////////////
		/// \brief Default constructor.
		///
		////////////////////////////////////////////////////////////////
		Server( );

		////////////////////////////////////////////////////////////////
		/// \brief Destructor.
		///
		////////////////////////////////////////////////////////////////
		~Server( );

		////////////////////////////////////////////////////////////////
		/// \brief Host the server.
		///
		////////////////////////////////////////////////////////////////
		Bit::Bool Host( const Bit::Uint16 p_Port );

	protected:

		////////////////////////////////////////////////////////////////
		/// \brief On client connection
		///
		////////////////////////////////////////////////////////////////
		virtual void OnConnection( const Bit::Uint16 p_UserId );
		
		////////////////////////////////////////////////////////////////
		/// \brief On client disconnection
		///
		////////////////////////////////////////////////////////////////
		virtual void OnDisconnection( const Bit::Uint16 p_UserId );

	private:

		// Private variables
		Bit::Thread		m_MainThread;
		Ball *			m_pBall;
		Player *		m_pPlayers[ 2 ];

	};

}

#endif