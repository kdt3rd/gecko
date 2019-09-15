// SPDX-License-Identifier: MIT
// Copyright contributors to the gecko project.

#include <signal.h>

int main( void ) { raise( SIGSEGV ); }
