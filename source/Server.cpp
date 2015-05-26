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
#include <iostream>
#include <Bit/System/Sleep.hpp>
#include <Bit/System/Timestep.hpp>
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
				eDirection direction = static_cast<eDirection>(p_Message.ReadByte());

				// Set direction and if the player is moving
				pPlayer->Direction = direction;
				pPlayer->IsMoving = true;
			}
			else if (p_Message.GetName() == "Stop")
			{
				Player * pPlayer = m_pServer->m_pPlayers[p_Message.GetUser()];
				pPlayer->IsMoving = false;
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
			HookUserMessage(&playerMessageListener, "Move");
			HookUserMessage(&playerMessageListener, "Stop");

			// Turn the main update function into a timestep function.
			Bit::Time updateTime = Bit::Seconds( 1.0f / 30.0f );
			Bit::Timestep timestep;

			// Main loop
			while( IsRunning( ) )
			{
				// Execute the timestep.
				timestep.Execute( updateTime, [ this ] ( )
				{
					// Update the keyboard
					m_Keyboard.Update( );

					// Check keyboard input
					if( m_Keyboard.KeyIsJustReleased( Bit::Keyboard::Num1 ) )
					{
						Stop( );
					}

					// Update the players
					for (Bit::SizeType i = 0; i < 2; i++)
					{
						// Get the player
						Player * pPlayer = m_pPlayers[i];

						// Check if the player is moving.
						if (pPlayer->IsMoving)
						{
							Bit::Vector2f32 newPosition = pPlayer->Position.Get();
							if (pPlayer->Direction == eDirection::Up)
							{
								newPosition.y += 1.0f;
							}
							else
							{
								newPosition.y -= 1.0f;
							}

							// Set the position
							pPlayer->Position.Set(newPosition);
						}
					}

					// Update the ball
					Bit::Vector2f32 ballPos = m_pBall->Position.Get( );
					ballPos.x -= 2.0f;
					m_pBall->Position.Set( ballPos );



				} );
			}
		}
		);
	
		// Succeeded
		return true;
	}

	void Server::MainUpdate( )
	{
		
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

		// Send the message
		pMessage->Send( pFilter );

		// Clean up the poitners
		delete pFilter;
		delete pMessage;
	}
		
	void Server::OnDisconnection( const Bit::Uint16 p_UserId )
	{
		std::cout << "Client disconnected." << std::endl;
	}

}