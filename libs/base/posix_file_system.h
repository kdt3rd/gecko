// Copyright (c) 2015-2016 Ian Godin and Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

#include "file_system.h"

namespace base
{

////////////////////////////////////////

class posix_file_system : public file_system
{
public:
	constexpr static size_t page_size = 4096;

	uri current_path( void ) const override;

	void lstat( const uri &path, struct stat *buf ) override;
	void statfs( const uri &path, struct statvfs *s ) override;
	uri readlink( const uri &path, size_t sz = 0 ) override;
	bool access( const uri &path, int mode ) override;

	directory_iterator readdir( const uri &path ) override;

	void mkdir( const uri &path, mode_t mode = mode_t(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) ) override;
	void mkdir_all( const uri &path, mode_t mode = mode_t(S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH) ) override;

	void rmdir( const uri &path ) override;
	void unlink( const uri &path ) override;
	void symlink( const uri &curpath, const uri &newpath ) override;
	void link( const uri &curpath, const uri &newpath ) override;
	void rename( const uri &oldpath, const uri &newpath ) override;

	istream open_read( const base::uri &path, std::ios_base::openmode m = file_system::file_read_mode ) override;
	ostream open_write( const base::uri &path, std::ios_base::openmode m = file_system::file_write_mode ) override;
	iostream open( const base::uri &path,
				   std::ios_base::openmode m = (file_read_mode|file_write_mode) ) override;

	fs_watch watch( const base::uri &path,
					const fs_watch::event_handler &evtcb,
					fs_event evt_mask, bool recursive ) override;
};

////////////////////////////////////////

}

