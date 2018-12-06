//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
//

#include "socket.h"
#include "response.h"
#include <base/sha160.h>
#include <base/base64.h>
#include <base/endian.h>
#include <bitset>

namespace web
{

////////////////////////////////////////

socket::socket( request &req, net::tcp_socket &&client )
	: _socket( std::move( client ) )
{
	if ( req["Connection"] != "Upgrade" )
		throw_runtime( "Request is not an upgrade connection (got {0})", req["Connection"] );
	if ( req["Upgrade"] != "websocket" )
		throw_runtime( "Request is not for a WebSocket upgrade (got {0})", req["Upgrade"] );

	std::string key = req["Sec-WebSocket-Key"];
	key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	base::sha160 hash;
	hash( key );
	auto h = hash.get_hash();

	response resp;
	resp.set_status_code( status_code::PROTOCOL_SWITCH );
	resp.set_header( "Upgrade", "websocket" );
	resp.set_header( "Connection", "upgrade" );
	resp.set_header( "Sec-WebSocket-Accept", base::base64_encode( h.data(), h.size() ) );
	resp.send( _socket );
	_masked = true;
}

////////////////////////////////////////

socket::socket( base::uri &server, const std::string &agent, double timeout )
{
	precondition( server.scheme() == "ws", "not WebSocket scheme: {0}", server.pretty() );
	_socket.connect( server, 80, timeout );


	std::string nonce;
	nonce.resize( 16 );
	for( size_t i = 0; i < 16; ++i )
		nonce[i] = static_cast<char>( random_byte() );

	request req( "GET", base::uri( server.full_path() ) );
	req.set_header( "User-Agent", agent );
	req.set_header( "Host", server.host() + ":" + base::to_string( server.port( 80 ) ) );
	req.set_header( "Origin", server.pretty() );
	req.set_header( "Connection", "Upgrade" );
	req.set_header( "Upgrade", "websocket" );
	req.set_header( "Sec-WebSocket-Version", "13" );
	req.set_header( "Sec-WebSocket-Key", base::base64_encode( nonce ) );

	_socket.connect( server, 80, timeout );
	req.send( _socket );

	response resp( _socket );
	_masked = false;
}

////////////////////////////////////////

socket::~socket( void )
{
}

////////////////////////////////////////

void socket::run( void )
{
	std::string message;
	bool bin = false;

	while ( wait( message, bin ) )
	{
		when_message( message, bin );
		message.clear();
	}
}

////////////////////////////////////////

bool socket::wait( std::string &message, bool &bin )
{
	bool done = false;
	bool gotmsg = false;
	bin = false;

	while ( !done )
	{
		uint8_t bits;
		_socket.read( &bits, sizeof(bits) );
		bool eom = bits & 0x80;

		// opcode
		switch ( bits & 0x0f )
		{
			case 0:
				// continuation
				break;

			case 1:
				// text frame
				bin = false;
				break;

			case 2:
				// binary frame
				bin = true;
				break;

			case 8:
				// connection close
				close();
				done = true;
				break;

			case 9:
				// ping
				break;

			case 10:
				// pong
				break;

			default:
				// reserved
				throw_runtime( "unknown WebSocket opcode" );
				break;
		}
		_socket.read( &bits, sizeof(bits) );
		bool masked = bits & 0x80;
		if ( masked != _masked )
			throw_runtime( "WebSocket mask mismatch" );

		uint64_t bytes = bits & 0x7f;

		if ( bytes == 126 )
		{
			uint16_t b16;
			_socket.read( &b16, sizeof(b16) );
			bytes = base::byteswap( b16 );
		}
		else if ( bytes == 127 )
		{
			_socket.read( &bytes, sizeof(bytes) );
			bytes = base::byteswap( bytes );
		}

		uint8_t key[4] = {};
		if ( masked )
			_socket.read( &key, sizeof(key) );

		size_t mlen = message.size();
		message.resize( mlen + bytes );
		_socket.read( &message[0] + mlen, bytes );
		if ( masked )
		{
			for ( size_t i = 0; i < bytes; ++i )
				message[i + mlen] ^= key[i%4];
		}

		if ( eom && !done )
		{
			done = true;
			gotmsg = true;
		}
	}

	return gotmsg;
}

////////////////////////////////////////

void socket::send( const char *msg, size_t len )
{
	uint8_t bits;
	bits = 0x80 | 0x1;
	_socket.write( &bits, 1 );

	if( len < 126 )
	{
		bits = uint8_t( len );
		if ( !_masked )
			bits |= 0x80;
		_socket.write( &bits, 1 );
	}
	else if ( len < 65536 )
	{
		bits = 126;
		if ( !_masked )
			bits |= 0x80;
		_socket.write( &bits, 1 );
		/// @todo Deal with endianess here.
		uint16_t l = base::byteswap( uint16_t( len ) );
		_socket.write( &l, sizeof(l) );
	}
	else
	{
		bits = 127;
		if ( !_masked )
			bits |= 0x80;
		_socket.write( &bits, 1 );
		uint64_t l = base::byteswap( uint64_t( len ) );
		_socket.write( &l, sizeof(l) );
	}

	if ( !_masked )
	{
		uint8_t key[4] = {};
		for ( size_t i = 0; i < 4; ++i )
			key[i] = random_byte();
		_socket.write( key, sizeof( key ) );

		std::string tmp;
		tmp.reserve( len );
		for ( size_t i = 0; i < len; ++i )
			tmp.push_back( msg[i] ^ key[i%4] );
		_socket.write( tmp.c_str(), tmp.size() );
	}
	else
		_socket.write( msg, len );
}

////////////////////////////////////////

void socket::send( const base::json &msg )
{
	std::string tmp;
	tmp << msg;
	send( tmp );
}

////////////////////////////////////////

void socket::close( void )
{
	if ( !_closed )
	{
		uint8_t bits;
		bits = 0x80 | 0x8;
		_socket.write( &bits, 1 );

		bits = 0;
		if ( !_masked )
			bits |= 0x80;
		_socket.write( &bits, 1 );

		if ( !_masked )
		{
			uint8_t key[4] = {};
			for ( size_t i = 0; i < 4; ++i )
				key[i] = random_byte();
			_socket.write( key, sizeof( key ) );
		}
	}
	_closed = true;
}

////////////////////////////////////////

}

