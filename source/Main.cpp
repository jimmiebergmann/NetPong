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

#include <iostream>
#include <Client.hpp>
#include <Server.hpp>
#include <Bit/Network/Net/Client.hpp>
#include <Bit/System/MemoryLeak.hpp>

// Global variables
static Pong::Server *		g_pServer	= NULL;

// Global functions
static int CloseApplication( );

// Main function
int main( int argc, char ** argv )
{
	// Init memory check.
	BitInitMemoryLeak( NULL );

	// Local variables
	const Bit::Address	address	= Bit::Address::Localhost;
	const Bit::Uint16	port		= 1338;
	const Bit::Time		timeout	= Bit::Seconds( 2.0f );

	// Try to connect to the server
	Pong::Client client;
	if( client.Join( g_pServer, address, port, timeout ) == false )
	{
		std::cout << "Failed to connect to server." << std::endl;

		// Host your own game if you can't connect
		g_pServer = new Pong::Server;
		if( g_pServer->Host( port ) == false )
		{
			std::cout << "Failed to host server." << std::endl;
			return CloseApplication( );
		}

		std::cout << "Hosted server." << std::endl;
		
		// Try to connect to the server once again.
		if( client.Join( g_pServer, address, port, timeout ) == false )
		{
			std::cout << "Failed to connect to server once again." << std::endl;
			return CloseApplication( );
		}
	}

	// Successfully connected to server
	std::cout << "Connected to server." << std::endl;

	// Run the client
	std::cout << "Running client." << std::endl;
	client.Run( );

	return CloseApplication( );
}

int CloseApplication( )
{
	// Delete the server if needed.
	if( g_pServer )
	{
		delete g_pServer;
		g_pServer = NULL;
	}

	std::cout << "Cleaned up the application." << std::endl;

	return 0;
}