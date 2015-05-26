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

#include <InitMessageListener.hpp>
#include <Client.hpp>
#include <Bit/System/MemoryLeak.hpp>

namespace Pong
{
	InitMessageListener::InitMessageListener( Client * p_pClient ) :
		m_pClient( p_pClient )
	{
	}

	void InitMessageListener::HandleMessage( Bit::Net::HostMessageDecoder & p_Message )
	{
		// Ignore the message if already initialized.
		if( m_pClient->m_Initialized.Get( ) == true )
		{
			return;
		}

		// Error check the message size
		if( p_Message.GetMessageSize( ) < 4 )
		{
			return;
		}
		
		// Read the user id
		Bit::Int32 userId = p_Message.ReadInt( );

		// Error check the user id
		if( userId < 0 || userId > 1 )
		{
			m_pClient->m_Initialized.Set( false );
			m_pClient->m_InitSemaphore.Release( );
			return;
		}

		m_pClient->m_UserId.Set( static_cast<Bit::Uint16>( userId ) );

		// Set the initialized flag and release the semaphore
		m_pClient->m_Initialized.Set( true );
		m_pClient->m_InitSemaphore.Release( );
	}

}