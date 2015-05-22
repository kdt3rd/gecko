
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
	: _client( std::move( client ) )
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
	resp.send( _client );
	_masked = true;
}

////////////////////////////////////////

socket::~socket( void )
{
}

////////////////////////////////////////

void socket::run( void )
{
//	on_open( *this );

	std::string message;
	bool bin = false;
	bool done = false;

	while ( !done )
	{
		uint8_t bits;
		_client.read( &bits, sizeof(bits) );
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

		_client.read( &bits, sizeof(bits) );
		bool masked = bits & 0x80;
		if ( masked != _masked )
			throw_runtime( "WebSocket mask mismatch" );

		uint64_t bytes = bits & 0x7f;

		if ( bytes == 126 )
		{
			uint16_t b16;
			_client.read( &b16, sizeof(b16) );
			bytes = base::byteswap( b16 );
		}
		else if ( bytes == 127 )
		{
			_client.read( &bytes, sizeof(bytes) );
			bytes = base::byteswap( bytes );
		}

		uint8_t key[4] = {};
		if ( masked )
			_client.read( &key, sizeof(key) );

		size_t mlen = message.size();
		message.resize( mlen + bytes );
		_client.read( &message[0] + mlen, bytes );
		if ( masked )
		{
			for ( size_t i = 0; i < bytes; ++i )
				message[i + mlen] ^= key[i%4];
		}

		if ( eom )
		{
			on_message( message, bin );
			message.clear();
		}
	}

//	on_close( *this );
}

////////////////////////////////////////

void socket::send( const std::string &msg )
{
	uint8_t bits;
	bits = 0x80 | 0x1;
	_client.write( &bits, 1 );

	if( msg.size() < 126 )
	{
		bits = uint8_t( msg.size() );
		_client.write( &bits, 1 );
	}
	else if ( msg.size() < 65536 )
	{
		bits = 126;
		_client.write( &bits, 1 );
		uint16_t len = base::byteswap( uint16_t( msg.size() ) );
		_client.write( &len, sizeof(len) );
	}
	else
	{
		bits = 127;
		_client.write( &bits, 1 );
		uint64_t len = base::byteswap( uint64_t( msg.size() ) );
		_client.write( &len, sizeof(len) );
	}

	_client.write( msg.c_str(), msg.size() );
}

////////////////////////////////////////

}

