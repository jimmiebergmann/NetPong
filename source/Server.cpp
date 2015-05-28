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
			else if (p_Message.GetName() == "StopMove")
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
		m_EntityManager.RegisterVariable( "Ball",	"Position",		&Ball::Position);
		m_EntityManager.RegisterVariable( "Ball",	"Rotation",		&Ball::Rotation);
		m_EntityManager.RegisterVariable( "Ball",	"Size",			&Ball::Size );
		m_EntityManager.RegisterVariable( "Ball",	"Direction",	&Ball::Direction );

		// Link and register player class
		m_EntityManager.LinkEntity<Player>( "Player" );
		m_EntityManager.RegisterVariable( "Player", "Position", &Player::Position );
		m_EntityManager.RegisterVariable( "Player", "Size",		&Player::Size );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );
		m_pBall->Position.Set( Bit::Vector2f32( 3.0f, 1.50f ) );
		m_pBall->Size.Set( Bit::Vector2f32( 0.20f, 0.20f ) );
		m_pBall->Direction.Set( Bit::Vector2f32( 1.0f, 0.0f ) );

		// Create the players
		m_pPlayers[ 0 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 0 ]->Position.Set( Bit::Vector2f32( 0.50f, 1.50f ) );
		m_pPlayers[ 0 ]->Size.Set( Bit::Vector2f32( 0.20f, 0.64f ) );
		m_pPlayers[ 1 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 1 ]->Position.Set( Bit::Vector2f32( 5.50f, 1.50f ) );
		m_pPlayers[1]->Size.Set(Bit::Vector2f32(0.20f, 0.64f));
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
		if (Start(p_Port, 2, 24, "NetPong") == false)
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
			HookUserMessage(&playerMessageListener, "StopMove");

			// Set up the scene
			// Create a physic shape
			Bit::Phys2::Circle circle(m_pBall->Size.Get().x);
			Bit::Phys2::Rectangle rectangle(Bit::Vector2f32( m_pPlayers[ 0 ]->Size.Get() ) );

			// Clear and create the bodies.
			m_Scene.Clear();

			// Add players and ball bodies
			m_pBodies[0] = m_Scene.Add(&rectangle, m_pPlayers[0]->Position.Get(), Bit::Phys2::Material(0.0f, 1.0, 0.3f, 0.1f));
			m_pBodies[1] = m_Scene.Add(&rectangle, m_pPlayers[1]->Position.Get(), Bit::Phys2::Material(0.0f, 1.0, 0.3f, 0.1f));
			m_pBodies[2] = m_Scene.Add(&circle, m_pBall->Position.Get(), Bit::Phys2::Material(1.0f, 1.0, 0.3f, 0.1f));
			m_pBodies[2]->ApplyForce(Bit::Vector2f32(-0.2f, 0.0f));

			// Add border bodies
			Bit::Phys2::Rectangle borderHor(Bit::Vector2f32( 20.0f, 0.2f ));
			m_Scene.Add(&borderHor, Bit::Vector2f32(0.0f, 0.0f), Bit::Phys2::Material(0.0f, 1.0, 0.3f, 0.1f));
			m_Scene.Add(&borderHor, Bit::Vector2f32(0.0f, 3.0f), Bit::Phys2::Material(0.0f, 1.0, 0.3f, 0.1f));

			// Turn the main update function into a timestep function.
			Bit::Time updateTime = Bit::Seconds( 1.0f / 60.0f );
			Bit::Timestep timestep;


			// Main loop
			while( IsRunning( ) )
			{
				// Execute the timestep.
				timestep.Execute(updateTime, [this, updateTime]()
				{
					m_Scene.Step(updateTime, 6, 4);

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
							//Bit::Vector2f32 newPosition = pPlayer->Position.Get();
							if (pPlayer->Direction == eDirection::Up)
							{
								// can not apply force to static objects, change position.
								Bit::Vector2f32 newPosition = m_pBodies[i]->GetPosition();
								newPosition.y += 2.0f * updateTime.AsSeconds();
								m_pBodies[i]->SetPosition(newPosition);
							}
							else
							{
								// can not apply force to static objects, change position.
								Bit::Vector2f32 newPosition = m_pBodies[i]->GetPosition();
								newPosition.y -= 2.0f * updateTime.AsSeconds();
								m_pBodies[i]->SetPosition(newPosition);
							}

							
							
						}

						// Set the position
						pPlayer->Position.Set(m_pBodies[i]->GetPosition());
					}

					

					if (m_pBall->Position.Get().x + m_pBall->Size.Get().x <= 0 ||
						m_pBall->Position.Get().x - m_pBall->Size.Get().x >= 6.0f)
					{
						m_pBodies[2]->SetPosition(Bit::Vector2f32(3.0f, 1.50f));
						m_pBall->Position.Set(m_pBodies[2]->GetPosition());
					}
					else
					{
						m_pBall->Position.Set(m_pBodies[2]->GetPosition());
					}

					m_pBall->Rotation.Set(m_pBodies[2]->GetOrientation().AsRadians());


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
		std::cout << "Client connected: " << p_UserId << std::endl;

		// Create message and filter
		/*Bit::Net::HostMessage * pMessage = CreateHostMessage( "Initialize" );
		Bit::Net::HostRecipientFilter * pFilter = CreateRecipientFilter( );

		// Add the receiver.
		pFilter->AddUser( p_UserId );

		// Add the user id to the message
		pMessage->WriteInt( static_cast<Bit::Int32>( p_UserId ) );

		// Send the message
		pMessage->Send( pFilter );

		// Clean up the poitners
		delete pFilter;
		delete pMessage;*/
	}
		
	void Server::OnDisconnection( const Bit::Uint16 p_UserId )
	{
		std::cout << "Client disconnected: " << p_UserId << std::endl;
	}

}