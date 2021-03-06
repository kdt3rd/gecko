// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include "transaction.h"

#include <base/contract.h>

namespace sqlite
{
////////////////////////////////////////

transaction::transaction( database &db ) : _db( db ) { _db.exec( "BEGIN" ); }

////////////////////////////////////////

transaction::~transaction( void )
{
    if ( !_committed )
        _db.exec( "ROLLBACK" );
}

////////////////////////////////////////

void transaction::commit( void )
{
    precondition( !_committed, "transaction already committed" );
    _db.exec( "COMMIT" );
    _committed = true;
}

////////////////////////////////////////

} // namespace sqlite
