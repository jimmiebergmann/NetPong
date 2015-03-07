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

#include <Server.hpp>
#include <Bit/System/Keyboard.hpp>
#include <Bit/System/Sleep.hpp>
#include <Bit/System/MemoryLeak.hpp>

namespace Pong
{

	Server::Server( ) :
		m_pBall( NULL )
	{
		// Link and register ball class
		m_EntityManager.LinkEntity<Ball>( "Ball" );
		m_EntityManager.RegisterVariable( "Ball", "Position", &Ball::Position );
		m_EntityManager.RegisterVariable( "Ball", "Direction", &Ball::Direction );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );
		m_pBall->Position.Set( Bit::Vector2f32( 0.0f, 0.0f ) );
		m_pBall->Direction.Set( Bit::Vector2f32( 1.0f, 0.0f ) );

		// Create players
	}

	Server::~Server( )
	{
		// Stop the server
		Stop( );
		m_MainThread.Finish( );

		// Delete the ball
		if( m_pBall )
		{
			delete m_pBall;
		}
	}

	Bit::Bool Server::Host( const Bit::Uint16 p_Port )
	{
		// Start the server
		if( Start( p_Port, 2 ) == false )
		{
			return false;
		}

		// Start the main thread
		m_MainThread.Execute( [ this] ( )
		{
			Bit::Keyboard keyboard;

			// Main loop
			while( IsRunning( ) )
			{
				// Sleep for some time
				Bit::Sleep( Bit::Milliseconds( 10 ) );

				// Update the keyboard
				keyboard.Update( );

				// Check keyboard input
				if( keyboard.KeyIsJustReleased( Bit::Keyboard::S ) )
				{
					Stop( );
					break;
				}

				// Update ball position and direction
				Bit::Vector2f32 newPosition = m_pBall->Position.Get( );
				newPosition.x += 1.0f;
				newPosition.y += 2.0f;
				m_pBall->Position.Set( newPosition );
			}
		}
		);
	
		// Succeeded
		return true;
	}

	void Server::OnConnection( const Bit::Uint16 p_UserId )
	{
	}
		
	void Server::OnDisconnection( const Bit::Uint16 p_UserId )
	{
	}

}