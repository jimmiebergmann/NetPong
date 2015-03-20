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

	// Player user message
	class PlayerMessageListener : public Bit::Net::UserMessageListener
	{

	public:

		PlayerMessageListener( Server * p_pServer ) :
			m_pServer( p_pServer )
		{
		}

		virtual void HandleMessage( Bit::Net::UserMessageDecoder & p_Message )
		{
			// Error check the user id.
			if( p_Message.GetUser( ) > 1 )
			{
				return;
			}

			// Move message
			if( p_Message.GetName( ) == "Move" )
			{
				// Get the player
				Player * pPlayer = m_pServer->m_pPlayers[ p_Message.GetUser( ) ];

				// Read the direction
				Bit::Uint8 direction = p_Message.ReadByte( );

				// Move the player
				Bit::Vector2f32 newPosition = pPlayer->Position.Get( );
				
				if( direction == 0 )
				{
					newPosition.y += 2.0f + ( 2.0f * p_Message.GetUser( ) );
				}
				else
				{
					newPosition.y -= 2.0f + ( 2.0f * p_Message.GetUser( ) );
				}

				// Set the new position
				pPlayer->Position.Set( newPosition );
			}

		}

		Server * m_pServer;

	};


	Server::Server( ) :
		m_pBall( NULL )
	{
		// Link and register ball class
		m_EntityManager.LinkEntity<Ball>( "Ball" );
		m_EntityManager.RegisterVariable( "Ball", "Position",	&Ball::Position );
		m_EntityManager.RegisterVariable( "Ball", "Size",		&Ball::Size );
		m_EntityManager.RegisterVariable( "Ball", "Direction",	&Ball::Direction );

		// Link and register player class
		m_EntityManager.LinkEntity<Player>( "Player" );
		m_EntityManager.RegisterVariable( "Player", "Position", &Player::Position );
		m_EntityManager.RegisterVariable( "Player", "Size",		&Player::Size );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );
		m_pBall->Position.Set( Bit::Vector2f32( 300.0f, 150.0f ) );
		m_pBall->Size.Set( Bit::Vector2f32( 20.0f, 20.0f ) );
		m_pBall->Direction.Set( Bit::Vector2f32( 1.0f, 0.0f ) );

		// Create the players
		m_pPlayers[ 0 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 0 ]->Position.Set( Bit::Vector2f32( 50.0f, 150.0f ) );
		m_pPlayers[ 0 ]->Size.Set( Bit::Vector2f32( 20.0f, 64.0f ) );
		m_pPlayers[ 1 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 1 ]->Position.Set( Bit::Vector2f32( 550.0f, 150.0f ) );
		m_pPlayers[ 1 ]->Size.Set( Bit::Vector2f32( 20.0f, 64.0f ) );
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
		m_MainThread.Execute( [ this ] ( )
		{
			Bit::Keyboard keyboard;
			PlayerMessageListener playerMessageListener( this );

			// Hook the user message
			HookUserMessage( &playerMessageListener, "Move" );

			// Main loop
			while( IsRunning( ) )
			{
				// Sleep for some time
				Bit::Sleep( Bit::Milliseconds( 10 ) );

				// Update the keyboard
				keyboard.Update( );

				// Check keyboard input
				if( keyboard.KeyIsJustReleased( Bit::Keyboard::Num1 ) )
				{
					Stop( );
					break;
				}

			}
		}
		);
	
		// Succeeded
		return true;
	}

	void Server::OnConnection( const Bit::Uint16 p_UserId )
	{
		// Create message and filter
		Bit::Net::HostMessage * pMessage = CreateHostMessage( "Initialize" );
		Bit::Net::HostRecipientFilter * pFilter = CreateRecipientFilter( );

		// Add the receiver.
		pFilter->AddUser( p_UserId );

		// Add the user id to the message
		pMessage->WriteInt( static_cast<Bit::Int32>( p_UserId ) );
/*
		for( Bit::SizeType i = 0; i < 2; i++ )
		{
			pMessage->WriteFloat( m_pPlayers[ i ]->Position.Get( ).x );
			pMessage->WriteFloat( m_pPlayers[ i ]->Position.Get( ).y );
			pMessage->WriteFloat( m_pPlayers[ i ]->Size.Get( ).x );
			pMessage->WriteFloat( m_pPlayers[ i ]->Size.Get( ).y );
		}
		pMessage->WriteFloat( m_pBall->Position.Get( ).x );
		pMessage->WriteFloat( m_pBall->Position.Get( ).y );
		pMessage->WriteFloat( m_pBall->Size.Get( ).x );
		pMessage->WriteFloat( m_pBall->Size.Get( ).y );
		*/
		// Send the message
		pMessage->Send( pFilter );

		// Clean up the poitners
		delete pFilter;
		delete pMessage;
	}
		
	void Server::OnDisconnection( const Bit::Uint16 p_UserId )
	{
	}

}