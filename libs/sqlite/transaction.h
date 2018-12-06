//
// Copyright (c) 2015 Ian Godin
// SPDX-License-Identifier: MIT
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
