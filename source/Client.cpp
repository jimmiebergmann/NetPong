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
#include <Bit/System/Sleep.hpp>
#include <iostream>
#include <Bit/System/MemoryLeak.hpp>

namespace Pong
{

	// Client class
	Client::Client( ) :
		m_pServer( NULL ),
		m_pBall( NULL ),
		//m_UserId( 0 ),
		m_Initialized( false ),
		m_InitMessageListener( this ),
		m_pWindow( NULL ),
		m_pBallShape( NULL )
	{
		// Set the player shapes to NULL
		m_pPlayerShapes[ 0 ] = NULL;
		m_pPlayerShapes[ 1 ] = NULL;

		// Hook initialization host message
		HookHostMessage( &m_InitMessageListener, "Initialize" );

		// Link and register ball class
		m_EntityManager.LinkEntity<Ball>( "Ball" );
		m_EntityManager.RegisterVariable( "Ball", "Position",	&Ball::Position );
		m_EntityManager.RegisterVariable( "Ball", "Rotation",	&Ball::Rotation);
		m_EntityManager.RegisterVariable( "Ball", "Size",		&Ball::Size );
		m_EntityManager.RegisterVariable( "Ball", "Direction",	&Ball::Direction );

		// Link and register player class
		m_EntityManager.LinkEntity<Player>( "Player" );
		m_EntityManager.RegisterVariable( "Player", "Position", &Player::Position );
		m_EntityManager.RegisterVariable( "Player", "Size",		&Player::Size );

		// Create a ball
		m_pBall = reinterpret_cast<Ball *>( m_EntityManager.CreateEntityByName( "Ball" ) );

		// Create the players
		m_pPlayers[ 0 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
		m_pPlayers[ 1 ] = reinterpret_cast<Player *>( m_EntityManager.CreateEntityByName( "Player" ) );
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
		//m_Initialized.Set( false );

		// Connect to the server
		Bit::Net::Client::eStatus status;
		status = Connect( p_Address, p_Port, p_Timeout, "NetPong" );

		// Failed to connect
		if( status != Bit::Net::Client::Succeeded )
		{
			m_pServer = NULL;
			return false;
		}

		// Wait for the initialize message from the server
		//m_InitSemaphore.Wait( p_Timeout );

		// Check if we received the initialize message
		/*if( m_Initialized.Get( ) == false )
		{
			return false;
		}
		*/
		//std::cout << "User id: " << m_UserId.Get( ) << std::endl;

		// Succeeded to connect
		m_pServer = p_pServer;
		return true;
	}

	Bit::Bool Client::Run( )
	{
		// Create graphics.
		CreateGraphics( );

		// Capture the lapsed time.
		Bit::Timer lapsedTime;
		lapsedTime.Start();

		// Main loop
		while( IsConnected( ) && m_pWindow->IsOpen( ) )
		{
			// Update the window
			m_pWindow->Update( );

			// Handle window events.
			Bit::Event wEvent;
			while( m_pWindow->PollEvent( wEvent ) )
			{
				// Check the window envent.
				switch( wEvent.Type )
				{
					// Key press events
					case Bit::Event::KeyJustPressed:
					{
						if( wEvent.Key == Bit::Keyboard::W )
						{
							// Pre-store the messages??
							Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
							pMessage->WriteByte( eDirection::Up );
							pMessage->Send( );
							delete pMessage;
						}
						else if( wEvent.Key == Bit::Keyboard::S )
						{
							// Pre-store the messages??
							Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
							pMessage->WriteByte( eDirection::Down );
							pMessage->Send( );
							delete pMessage;
						}
					}
					break;
					case Bit::Event::KeyJustReleased:
					{

						if (wEvent.Key == Bit::Keyboard::W)
						{

							// Pre-store the messages??
							Bit::Net::UserMessage * pMessage = CreateUserMessage("StopMove");
							pMessage->Send();
							delete pMessage;
						}
						else if (wEvent.Key == Bit::Keyboard::S)
						{
							// Pre-store the messages??
							Bit::Net::UserMessage * pMessage = CreateUserMessage("StopMove");
							pMessage->Send();
							delete pMessage;
						}
						else if( wEvent.Key == Bit::Keyboard::Num2 )
						{
							m_pWindow->Close( );
						}
					}
					break;
					case Bit::Event::Closed:
					{
						m_pWindow->Close( );
					}
					break;

				default:
					break;
				}

			}

			// Check again if the window is open
			if( m_pWindow->IsOpen( ) == false )
			{
				break;
			}

			// Render the shapes
			for( Bit::SizeType i = 0; i < 2; i++ )
			{
				m_pPlayerShapes[i]->SetPosition(m_pPlayers[i]->Position.Get() * 100.0f);
				m_pWindow->Draw(m_pPlayerShapes[i], Bit::PrimitiveMode::LineStrip);
			}
			m_pBallShape->SetPosition(m_pBall->Position.Get() * 100.0f);
			m_pBallShape->SetRotation(Bit::Radians(m_pBall->Rotation.Get()));
			m_pWindow->Draw(m_pBallShape, Bit::PrimitiveMode::LineStrip);

			// Present the window, graphics.
			m_pWindow->Present( );

			//std::cout << "Player: " << m_pPlayers[ m_UserId.Get( ) ]->Position.Get( ).x << "   " << m_pPlayers[ m_UserId.Get( ) ]->Position.Get( ).y << std::endl;
		}

		lapsedTime.Stop();
		std::cout << "Ran game for " << lapsedTime.GetTime().AsSeconds() << " seconds.\n";


		// Destroy the graphics
		DestroyGraphics( );

		return true;
	}

	Bit::Bool Client::CreateGraphics( )
	{
		// Create the window
		m_pWindow = new Bit::SimpleRenderWindow( Bit::VideoMode( Bit::Vector2u32( 600, 300 ) ) );

		// Create the shapes
		for( Bit::SizeType i = 0; i < 2; i++ )
		{
			m_pPlayerShapes[ i ] = m_pWindow->CreateRectangleShape( );
			m_pPlayerShapes[ i ]->SetPosition(m_pPlayers[i]->Position.Get() * 100.0f);
			m_pPlayerShapes[i]->SetSize(m_pPlayers[i]->Size.Get() * 100.0f);
		}

		m_pBallShape = m_pWindow->CreateCircleShape(30); 
		m_pBallShape->SetPosition(m_pBall->Position.Get() * 100.0f);
		m_pBallShape->SetSize( m_pBall->Size.Get( ) * 100.0f );
		
		return true;
	}

	void Client::DestroyGraphics( )
	{
		if( m_pWindow )
		{
			for( Bit::SizeType i = 0; i < 2; i++ )
			{
				if( m_pPlayerShapes[ i ] )
				{
					m_pWindow->DestroyShape( m_pPlayerShapes[ i ] );
				}
			}
			m_pWindow->DestroyShape( m_pBallShape );

			delete m_pWindow;
			m_pWindow = NULL;
		}
	}

}