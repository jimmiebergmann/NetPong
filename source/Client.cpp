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

#include <Client.hpp>
#include <Bit/System/Keyboard.hpp>
#include <Bit/System/Sleep.hpp>
#include <iostream>
#include <Bit/System/MemoryLeak.hpp>

namespace Pong
{

	Client::Client( ) :
		m_pServer( NULL ),
		m_pBall( NULL )
	{
		// Link and register ball class
		m_EntityManager.LinkEntity<Ball>( "Ball" );
		m_EntityManager.RegisterVariable( "Ball", "Position", &Ball::Position );
		m_EntityManager.RegisterVariable( "Ball", "Direction", &Ball::Direction );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );
	}

	Client::~Client( )
	{
		// Delete the ball
		if( m_pBall )
		{
			delete m_pBall;
		}
	}

	Bit::Bool Client::Join(	Server * p_pServer,
							const Bit::Address & p_Address,
							const Bit::Uint16 p_Port,
							const Bit::Time & p_Timeout )
	{

		// Connect to the server
		Bit::Net::Client::eStatus status;
		status = Connect( p_Address, p_Port, p_Timeout );

		// Failed to connect
		if( status != Bit::Net::Client::Succeeded )
		{
			m_pServer = NULL;
			return false;
		}

		// Succeeded to connect
		m_pServer = p_pServer;
		return true;
	}

	Bit::Bool Client::Run( )
	{

		Bit::Keyboard keyboard;

		// Main loop
		while( IsConnected( ) )
		{
			// Sleep for some time
			Bit::Sleep( Bit::Milliseconds( 10 ) );

			// Update the keyboard
			keyboard.Update( );

			// Check keyboard input
			if( keyboard.KeyIsJustReleased( Bit::Keyboard::D ) )
			{
				break;
			}

			//std::cout << "Client: " << m_pBall->Position.Get( ).x << "   " << m_pBall->Position.Get( ).y << std::endl;
		}

		return true;
	}

}