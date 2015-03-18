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
		m_UserId( 0 ),
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
		m_Initialized.Set( false );

		// Connect to the server
		Bit::Net::Client::eStatus status;
		status = Connect( p_Address, p_Port, p_Timeout );

		// Failed to connect
		if( status != Bit::Net::Client::Succeeded )
		{
			m_pServer = NULL;
			return false;
		}

		// Wait for the initialize message from the server
		m_InitSemaphore.Wait( p_Timeout );

		// Check if we received the initialize message
		if( m_Initialized.Get( ) == false )
		{
			return false;
		}

		std::cout << "User id: " << m_UserId.Get( ) << std::endl;

		// Succeeded to connect
		m_pServer = p_pServer;
		return true;
	}

	Bit::Bool Client::Run( )
	{
		// Create graphics.
		CreateGraphics( );

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
					case Bit::Event::KeyPressed:
					{
						if( wEvent.Key == Bit::Keyboard::W )
						{
							Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
							pMessage->WriteByte( 0 );
							pMessage->Send( );
							delete pMessage;
						}
						else if( wEvent.Key == Bit::Keyboard::S )
						{
							Bit::Net::UserMessage * pMessage = CreateUserMessage( "Move" );
							pMessage->WriteByte( 1 );
							pMessage->Send( );
							delete pMessage;
						}
					}
					break;
					case Bit::Event::KeyJustReleased:
					{
						if( wEvent.Key == Bit::Keyboard::Num2 )
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
			m_pPlayerShapes[ 0 ]->SetPosition( m_pPlayers[ 0 ]->Position.Get( ) );
			m_pPlayerShapes[ 1 ]->SetPosition( m_pPlayers[ 1 ]->Position.Get( ) );
			m_pWindow->Draw( m_pPlayerShapes[ 0 ] );
			m_pWindow->Draw( m_pPlayerShapes[ 1 ] );


			// Present the window, graphics.
			m_pWindow->Present( );

			//std::cout << "Player: " << m_pPlayers[ m_UserId.Get( ) ]->Position.Get( ).x << "   " << m_pPlayers[ m_UserId.Get( ) ]->Position.Get( ).y << std::endl;
		}

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
			m_pPlayerShapes[ i ] = m_pWindow->CreateShape( true );
			m_pPlayerShapes[ i ]->SetSize( Bit::Vector2f32( 32.0f, 32.0f ) );
			m_pPlayerShapes[ i ]->SetPosition( m_pPlayers[ i ]->Position.Get( ) );
		}
		
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

			delete m_pWindow;
			m_pWindow = NULL;
		}
	}

}