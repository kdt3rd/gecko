//
// Copyright (c) 2015 Ian Godin
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

#include "database.h"

namespace sqlite
{

////////////////////////////////////////

class transaction
{
public:
	transaction( database &db );
	~transaction( void );

	void commit( void );

private:
	database &_db;
	bool _committed = false;
};

////////////////////////////////////////

}
