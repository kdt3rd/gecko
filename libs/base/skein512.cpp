/***********************************************************************
**
** Implementation of the Skein hash function.
**
** Source code author: Doug Whiting, 2008.
**
** This algorithm and source code is released to the public domain.
**
************************************************************************/

#include "skein512.h" /* get the Skein API definitions   */

#include <string.h> /* get the memcpy/memset functions */

#define Skein_Put64_LSB_First( dst08, src64, blocksize )                       \
    memcpy( dst08, src64, blocksize )
#define Skein_Get64_LSB_First( dst64, src08, wCnt )                            \
    memcpy( dst64, src08, 8 * ( wCnt ) )
#define Skein_Swap64( w64 ) ( w64 )

#define SKEIN_T1_FLAG_FIRST ( uint64_t( 1 ) << 62 )
#define SKEIN_T1_FLAG_FINAL ( uint64_t( 1 ) << 63 )
#define SKEIN_T1_BLK_TYPE_MSG ( uint64_t( 48 ) << 56 )
#define SKEIN_T1_BLK_TYPE_OUT ( uint64_t( 63 ) << 56 )
#define SKEIN_T1_BLK_TYPE_OUT_FINAL                                            \
    ( SKEIN_T1_BLK_TYPE_OUT | SKEIN_T1_FLAG_FINAL )

namespace base
{
/* blkSize =  512 bits. hashSize =  512 bits */
const uint64_t SKEIN_512_IV_512[] = { 0xA8D47980544A6E32, 0x847511533E9B1A8A,
                                      0x6FAEE870D8E81A00, 0x58B0D9D6CB557F92,
                                      0x9BBC0051DAC1D4E9, 0xB744E2B1D189E7CA,
                                      0x979350FA709C5EF3, 0x0350125A92067BCD };

#define KW_TWK_BASE ( 0 )
#define KW_KEY_BASE ( 3 )
#define ks ( kw + KW_KEY_BASE )
#define ts ( kw + KW_TWK_BASE )

enum
{
    R_512_0_0 = 38,
    R_512_0_1 = 30,
    R_512_0_2 = 50,
    R_512_0_3 = 53,
    R_512_1_0 = 48,
    R_512_1_1 = 20,
    R_512_1_2 = 43,
    R_512_1_3 = 31,
    R_512_2_0 = 34,
    R_512_2_1 = 14,
    R_512_2_2 = 15,
    R_512_2_3 = 27,
    R_512_3_0 = 26,
    R_512_3_1 = 12,
    R_512_3_2 = 58,
    R_512_3_3 = 7,
    R_512_4_0 = 33,
    R_512_4_1 = 49,
    R_512_4_2 = 8,
    R_512_4_3 = 42,
    R_512_5_0 = 39,
    R_512_5_1 = 27,
    R_512_5_2 = 41,
    R_512_5_3 = 14,
    R_512_6_0 = 29,
    R_512_6_1 = 26,
    R_512_6_2 = 11,
    R_512_6_3 = 9,
    R_512_7_0 = 33,
    R_512_7_1 = 51,
    R_512_7_2 = 39,
    R_512_7_3 = 35,
};

inline uint64_t RotL_64( uint64_t x, int n )
{
    return ( ( ( x ) << ( n ) ) | ( ( x ) >> ( 64 - ( n ) ) ) );
}

/*****************************  Skein_512 ******************************/
void skein512::process(
    const uint8_t *blkPtr, size_t blkCnt, size_t byteCntAdd )
{
    uint64_t kw[8 + 4]; /* key schedule words : chaining vars + tweak */
    uint64_t X0, X1, X2, X3, X4, X5, X6, X7; /* local copy of vars, for speed */
    uint64_t w[8];                           /* local copy of input block */

    ts[0] = _T[0];
    ts[1] = _T[1];
    do
    {
        /* this implementation only supports 2**64 input bytes (no carry out here) */
        ts[0] += byteCntAdd; /* update processed length */

        //        /* precompute the key schedule for this block */
        memcpy( ks, _X, sizeof( uint64_t ) * 8 );
        ks[8] = ks[0] ^ ks[1] ^ ks[2] ^ ks[3] ^ ks[4] ^ ks[5] ^ ks[6] ^ ks[7] ^
                0x5555555555555555;

        ts[2] = ts[0] ^ ts[1];

        Skein_Get64_LSB_First(
            w, blkPtr, 8 ); /* get input block in little-endian format */

        X0 = w[0] + ks[0]; /* do the first full key injection */
        X1 = w[1] + ks[1];
        X2 = w[2] + ks[2];
        X3 = w[3] + ks[3];
        X4 = w[4] + ks[4];
        X5 = w[5] + ks[5] + ts[0];
        X6 = w[6] + ks[6] + ts[1];
        X7 = w[7] + ks[7];

        blkPtr += 64;

        /* run the rounds */
#define R512( p0, p1, p2, p3, p4, p5, p6, p7, ROT, rNum )                      \
    X##p0 += X##p1;                                                            \
    X##p1 = RotL_64( X##p1, ROT##_0 );                                         \
    X##p1 ^= X##p0;                                                            \
    X##p2 += X##p3;                                                            \
    X##p3 = RotL_64( X##p3, ROT##_1 );                                         \
    X##p3 ^= X##p2;                                                            \
    X##p4 += X##p5;                                                            \
    X##p5 = RotL_64( X##p5, ROT##_2 );                                         \
    X##p5 ^= X##p4;                                                            \
    X##p6 += X##p7;                                                            \
    X##p7 = RotL_64( X##p7, ROT##_3 );                                         \
    X##p7 ^= X##p6;

#define I512( R )                                                              \
    X0 += ks[( ( R ) + 1 ) % 9]; /* inject the key schedule value */           \
    X1 += ks[( ( R ) + 2 ) % 9];                                               \
    X2 += ks[( ( R ) + 3 ) % 9];                                               \
    X3 += ks[( ( R ) + 4 ) % 9];                                               \
    X4 += ks[( ( R ) + 5 ) % 9];                                               \
    X5 += ks[( ( R ) + 6 ) % 9] + ts[( ( R ) + 1 ) % 3];                       \
    X6 += ks[( ( R ) + 7 ) % 9] + ts[( ( R ) + 2 ) % 3];                       \
    X7 += ks[( ( R ) + 8 ) % 9] + ( R ) + 1;
        {
#define R512_8_rounds( R ) /* do 8 full rounds */                              \
    R512( 0, 1, 2, 3, 4, 5, 6, 7, R_512_0, 8 * ( R ) + 1 );                    \
    R512( 2, 1, 4, 7, 6, 5, 0, 3, R_512_1, 8 * ( R ) + 2 );                    \
    R512( 4, 1, 6, 3, 0, 5, 2, 7, R_512_2, 8 * ( R ) + 3 );                    \
    R512( 6, 1, 0, 7, 2, 5, 4, 3, R_512_3, 8 * ( R ) + 4 );                    \
    I512( 2 * ( R ) );                                                         \
    R512( 0, 1, 2, 3, 4, 5, 6, 7, R_512_4, 8 * ( R ) + 5 );                    \
    R512( 2, 1, 4, 7, 6, 5, 0, 3, R_512_5, 8 * ( R ) + 6 );                    \
    R512( 4, 1, 6, 3, 0, 5, 2, 7, R_512_6, 8 * ( R ) + 7 );                    \
    R512( 6, 1, 0, 7, 2, 5, 4, 3, R_512_7, 8 * ( R ) + 8 );                    \
    I512( 2 * ( R ) + 1 ); /* and key injection */

            R512_8_rounds( 0 );
            R512_8_rounds( 1 );
            R512_8_rounds( 2 );
            R512_8_rounds( 3 );
            R512_8_rounds( 4 );
            R512_8_rounds( 5 );
            R512_8_rounds( 6 );
            R512_8_rounds( 7 );
            R512_8_rounds( 8 );
        }

        /* do the final "feedforward" xor, update context chaining vars */
        _X[0] = X0 ^ w[0];
        _X[1] = X1 ^ w[1];
        _X[2] = X2 ^ w[2];
        _X[3] = X3 ^ w[3];
        _X[4] = X4 ^ w[4];
        _X[5] = X5 ^ w[5];
        _X[6] = X6 ^ w[6];
        _X[7] = X7 ^ w[7];

        ts[1] &= ~SKEIN_T1_FLAG_FIRST;
    } while ( --blkCnt );
    _T[0] = ts[0];
    _T[1] = ts[1];
}

void skein512::start_new_type( uint64_t block_type )
{
    _T[0]       = 0;
    _T[1]       = SKEIN_T1_FLAG_FIRST | block_type;
    _block_size = 0;
}

/*****************************************************************/
/*     512-bit Skein                                             */
/*****************************************************************/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* init the context for a straight hashing operation  */
skein512::skein512( void )
{
    memcpy( _X, SKEIN_512_IV_512, sizeof( _X ) );
    start_new_type( SKEIN_T1_BLK_TYPE_MSG );
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* process the input bytes */
void skein512::update( const void *message, size_t msgByteCnt )
{
    const uint8_t *msg = static_cast<const uint8_t *>( message );
    size_t         n;

    /* process full blocks, if any */
    if ( msgByteCnt + _block_size > 64 )
    {
        if ( _block_size ) /* finish up any buffered message data */
        {
            n = 64 - _block_size; /* # bytes free in buffer b[] */
            if ( n )
            {
                memcpy( &_block[_block_size], msg, n );
                msgByteCnt -= n;
                msg += n;
                _block_size += n;
            }
            process( _block, 1, 64 );
            _block_size = 0;
        }
        /* now process any remaining full blocks, directly from input message data */
        if ( msgByteCnt > 64 )
        {
            n = ( msgByteCnt - 1 ) / 64; /* number of full blocks to process */
            process( msg, n, 64 );
            msgByteCnt -= n * 64;
            msg += n * 64;
        }
    }

    /* copy any remaining source message data bytes into b[] */
    if ( msgByteCnt )
    {
        memcpy( &_block[_block_size], msg, msgByteCnt );
        _block_size += msgByteCnt;
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* finalize the hash computation and output the result */
void skein512::finalize( hash &hashVal )
{
    size_t   n, byteCnt;
    uint64_t Y[8];

    _T[1] |= SKEIN_T1_FLAG_FINAL; /* tag as the final block */
    if ( _block_size < 64 )       /* zero pad b[] if necessary */
        memset( &_block[_block_size], 0, 64 - _block_size );

    process( _block, 1, _block_size ); /* process the final block */

    /* now output the result */
    byteCnt = ( 512 + 7 ) >> 3; /* total number of output bytes */

    /* run Threefish in "counter mode" to generate output */
    memset(
        _block,
        0,
        sizeof( _block ) ); /* zero out b[], so it can hold the counter */
    memcpy( Y, _X, sizeof( _X ) ); /* keep a local copy of counter mode "key" */
    for ( size_t i = 0; i * 64 < byteCnt; i++ )
    {
        _block64[0] =
            Skein_Swap64( uint64_t( i ) ); /* build the counter block */

        start_new_type( SKEIN_T1_BLK_TYPE_OUT_FINAL );
        process( _block, 1, sizeof( uint64_t ) ); /* run "counter mode" */
        n = byteCnt - i * 64; /* number of output bytes left to go */
        if ( n >= 64 )
            n = 64;
        Skein_Put64_LSB_First(
            hashVal.data() + i * 64, _X, n ); /* "output" the ctr mode bytes */
        memcpy(
            _X,
            Y,
            sizeof( _X ) ); /* restore the counter mode key for next time */
    }
}

////////////////////////////////////////

skein512::hash skein512::get_hash( void )
{
    if ( !_finalized )
        finalize( _hash );
    return _hash;
}

////////////////////////////////////////

std::string skein512::hash_string( void )
{
    if ( !_finalized )
        finalize( _hash );

    static char hex_table[16]{ '0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    std::string result;
    result.reserve( _hash.size() * 2 );

    for ( size_t i = 0; i < _hash.size(); ++i )
    {
        uint8_t hv = _hash[i];
        result.push_back( hex_table[hv >> 4] );
        result.push_back( hex_table[hv & 0xf] );
    }

    return result;
}

////////////////////////////////////////

} // namespace base
