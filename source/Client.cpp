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

		// Link and register player class
		m_EntityManager.LinkEntity<Ball>( "Player" );
		m_EntityManager.RegisterVariable( "Player", "Position", &Player::Position );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );

		// Create the players
		m_pPlayers[ 0 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 0 ]->Position.Set( Bit::Vector2f32( 100.0f, 300.0f ) );
		m_pPlayers[ 1 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 1 ]->Position.Set( Bit::Vector2f32( 600.0f, 300.0f ) );
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
			
			
			if( keyboard.KeyIsJustReleased( Bit::Keyboard::Num2 ) )
			{
				break;
			}
			else if( keyboard.KeyIsDown( Bit::Keyboard::W ) )
			{
				Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
				pMessage->WriteByte( 0 );
				pMessage->Send( );
				delete pMessage;
			}
			else if( keyboard.KeyIsDown( Bit::Keyboard::S ) )
			{
				Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
				pMessage->WriteByte( 1 );
				pMessage->Send( );
				delete pMessage;
			}

			std::cout << "Player: " << m_pPlayers[ 0 ]->Position.Get( ).x << "   " << m_pPlayers[ 0 ]->Position.Get( ).y << std::endl;
		}

		return true;
	}

}