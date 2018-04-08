//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licenced under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#include "dso.h"
#include "contract.h"
#ifndef _WIN32
#define HAS_CXXABI
#define HAS_ELF_DLFCN
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cxxabi.h>
#include <elf.h>
#include <link.h>
#include <dlfcn.h>
#define HAS_BACKTRACE
#include <execinfo.h>
#endif
#include "memory_map.h"

////////////////////////////////////////

namespace {

#ifdef HAS_CXXABI
std::string
demangle(const char *sym)
{
    if ( sym && sym[0] != '\0' )
    {
        if ( sym[0] == '_' )
        {
            int status = -42;
            std::unique_ptr<char, void(*)(void*)> realname {
                abi::__cxa_demangle( sym, nullptr, nullptr, &status ),
                std::free
            };

            if ( status == 0 )
                return std::string( realname.get() );
        }

        return std::string( sym );
    }
    return std::string();
}
#endif

}

////////////////////////////////////////

namespace base
{

////////////////////////////////////////

dso::dso( const char *fn, bool makeGlobal )
{
    load( fn, makeGlobal );
}

////////////////////////////////////////

dso::~dso( void )
{
    reset();
}

////////////////////////////////////////

dso::dso( dso &&o )
    : _fn( std::move( o._fn ) ), _last_err( std::move( o._last_err ) ),
      _handle( base::exchange( o._handle, nullptr ) )
{
}

////////////////////////////////////////

dso &dso::operator=( dso &&o )
{
    std::swap( _fn, o._fn );
    std::swap( _last_err, o._last_err );
    std::swap( _handle, o._handle );
    return *this;
}

////////////////////////////////////////

void dso::reset( void )
{
#ifdef HAS_ELF_DLFCN
    if ( _handle )
        dlclose( _handle );
#else
# ifdef _WIN32
    if ( _handle )
        FreeLibrary( (HMODULE)_handle );
# else
#  error Unknown dynamic object system
# endif
#endif
    _handle = nullptr;
    _fn.clear();
    _last_err.clear();
}

////////////////////////////////////////

void dso::load( const char *fn, bool makeGlobal )
{
    reset();
    if ( fn )
        _fn = fn;
#ifdef HAS_ELF_DLFCN
    int flags = RTLD_NOW|RTLD_DEEPBIND;
    if ( makeGlobal )
        flags |= RTLD_GLOBAL;
    _handle = dlopen( fn, flags );
    if ( ! _handle )
        _last_err = dlerror();
#else
# ifdef _WIN32
    HMODULE t = nullptr;
    bool haderr = false;
    if ( fn )
    {
        t = LoadLibrary( fn );
        haderr = (t == nullptr);
    }
    else
        haderr = ! GetModuleHandleEx( 0, NULL, &t );
    
    if ( haderr )
    {
        char *msg = nullptr;
        FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, GetLastError(), 0,
                       &msg, 0, nullptr );
        if ( msg )
            _last_err = msg;

        LocalFree( msg );
    }
# else
#  error Unknown dynamic object system
# endif
#endif
}


////////////////////////////////////////

void *
dso::find( const char *symn, const char *symver )
{
    precondition( _handle, "dso failed to load, check valid state" );
    void *ret = nullptr;
    if ( _handle )
    {
#ifdef HAS_ELF_DLFCN
        if ( symver )
            ret = dlvsym( _handle, symn, symver );
        else
            ret = dlsym( _handle, symn );
        if ( ! ret )
            _last_err = dlerror();
#else
# ifdef _WIN32
        // todo symbol versioning???
        ret = GetProcAddress( _handle, symn );
        if ( ! ret )
        {
            char *msg = nullptr;
            FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, GetLastError(), 0,
                           &msg, 0, nullptr );
            if ( msg )
                _last_err = msg;
            LocalFree( msg );
        }
# else
#  error Unknown dynamic object system
# endif
#endif
    }
    return ret;
}

////////////////////////////////////////

namespace dl_extra
{


#ifdef HAS_ELF_DLFCN

namespace {

class symbol_table
{
public:
    void reset(int fd, const ElfW(Shdr) &st, const ElfW(Shdr) *segHdrs, size_t nSegs)
    {
        // precondition( st.sh_type == SHT_SYMTAB || st.sh_type == SHT_DYNSYM, "valid segment" );
        _syms.reset( fd, st.sh_offset, st.sh_size );
        _num_syms = st.sh_size / st.sh_entsize;
        _last_local = st.sh_info;

        size_t strings = st.sh_link;
        if ( strings != SHN_UNDEF )
        {
            if ( strings >= nSegs )
            {
                std::cerr << "string table link " << strings << " beyond on number of segments " << nSegs << std::endl;
                throw std::runtime_error( "String table beyond end of segment header list, suspect corrupt file" );
            }

            const ElfW(Shdr) &strTab = segHdrs[strings];
            if ( strTab.sh_type != SHT_STRTAB )
            {
                std::cerr << "Invalid string table " << strings << " numsegs " << nSegs << " for symbol table, type " << strTab.sh_type << " (" << std::hex << strTab.sh_type << std::dec << ")" << std::endl;
            }

            _strings.reset( fd, segHdrs[strings].sh_offset, segHdrs[strings].sh_size );
        }
    }

    size_t size( void ) const { return _num_syms; }

    const ElfW(Sym) &symbol( size_t x ) const { return _syms[x]; }
    const char *name( size_t x ) const { return _strings.get() + symbol( x ).st_name; }

    const ElfW(Sym) &operator[]( size_t x ) const { return symbol( x ); }
private:
    read_memory_map<ElfW(Sym)> _syms;
    read_memory_map<char> _strings;
    size_t _num_syms = 0;
    size_t _last_local = 0;
};

}

class active_shared_object::impl
{
public:
    impl( const char *name, uintptr_t base_addr )
        : _base( base_addr )
    {
        load_syms( name );
    }

    explicit impl( struct dl_phdr_info *phdr )
        : _base( phdr->dlpi_addr )
    {
        load_syms( phdr->dlpi_name );

//        for ( size_t i = 0; i < phdr->dlpi_phnum; ++i )
//        {
//            auto &phd = phdr->dlpi_phdr[i];
//            if ( phd.p_type == PT_LOAD )
//            {
//                _vaddr_to_seg_fileoffset[phd.p_vaddr] = phd.p_offset;
//                _seg_fileoffset_to_vaddr[phd.p_offset] = phd.p_vaddr;
//            }
//        }
    }

    ~impl( void )
    {
        if ( _fd >= 0 )
            ::close( _fd );
    }

    inline const std::string &path( void ) const { return _path; }
    inline const std::string &name( void ) const { return _name; }
    inline uintptr_t base( void ) const { return _base; }

    bool is_closest( void *addr, const impl *curdso )
    {
        uintptr_t aint = reinterpret_cast<uintptr_t>(addr);
        if ( _base < aint )
        {
            uintptr_t off = aint - _base;
            if ( curdso )
            {
                if ( curdso->base() < aint )
                {
                    uintptr_t ooff = aint - curdso->base();
                    return ( off < ooff );
                }
            }
            return true;
        }
        return false;
    }

    void load_syms( const char *fn )
    {
        if ( fn[0] == '\0' )
            fn = "/proc/self/exe";

        char *fullpath = realpath( fn, nullptr );
        if ( fullpath )
        {
            _path = fullpath;
            free( fullpath );
        }
        else
            _path = fn;

        if ( _path.find( "linux-vdso" ) != std::string::npos )
        {
            // provided by the kernel, won't find the binary anywhere...
            //std::cout << " skipping " << _path << std::endl;
            return;
        }
        _fd = open( _path.c_str(), O_RDONLY | O_CLOEXEC );
        if ( _fd < 0 )
            throw std::runtime_error( "Unable to open dso '" + _path + "' for read" );

        std::string::size_type ls = _path.find_last_of('/');
        if ( ls != std::string::npos )
            _name = _path.substr( ls + 1 );
        else
            _name = _path;

        //std::cout << "loading symbols from: '" << _path << "'..." << std::endl;
        read_memory_map<ElfW(Ehdr)> hdr( _fd, 0, sizeof(ElfW(Ehdr)) );

        size_t numSegHdrs = hdr->e_shnum;
        off64_t segHdrOff = hdr->e_shoff;
        size_t segHdrSize = numSegHdrs * sizeof(ElfW(Shdr));
        bool isElf = ( 0 == memcmp( hdr->e_ident, ELFMAG, SELFMAG ) );

        if ( ! isElf )
            throw std::runtime_error( "Entry in link map does not seem to be an elf file" );

        const unsigned char nativeClass = __ELF_NATIVE_CLASS == 64 ? ELFCLASS64 : ELFCLASS32;
        if ( nativeClass != hdr->e_ident[EI_CLASS] )
            throw std::runtime_error( "Elf class is not for native elf binary size (32/64) bit mismatch" );

        // don't need the header anymore...
        hdr.reset();
        
        //std::cout << " numSegHdrs: " << numSegHdrs << " hdroff: " << std::hex << segHdrOff << std::dec << " hdrsz: " << segHdrSize << std::endl;
        read_memory_map<ElfW(Shdr)> segHdrs( _fd, segHdrOff, segHdrSize );

        // there should only be one of each of these (non dynamic and dynamic symbol tables)
        size_t numSymTabs = 0;
        size_t numDynSymTabs = 0;
        for ( size_t i = 0; i != numSegHdrs; ++i )
        {
            //std::cout << _path << " seg " << i << ": type " << std::hex << segHdrs[i].sh_type << std::dec << " (" << segHdrs[i].sh_type << ") link " << segHdrs[i].sh_link << std::endl;
            switch ( segHdrs[i].sh_type )
            {
                // there should only be one of each of these, although the spec does say
                // that may be relaxed in the future.
                // os/x fat binaries?
                // for now, just keep a count and error out if we get more than the two
                case SHT_SYMTAB:
                    if ( numSymTabs > 0 )
                        throw std::runtime_error( "Binary format looks like this has multiple symbol tables - the spec allows for that as a future expansion, but not yet implemented" );
                    ++numSymTabs;
                    _nondyn_syms.reset( _fd, segHdrs[i], segHdrs.get(), numSegHdrs );
                    break;
                case SHT_DYNSYM:
                    if ( numSymTabs > 0 )
                        throw std::runtime_error( "Binary format looks like this has multiple dynamic symbol tables - the spec allows for that as a future expansion, but not yet implemented" );
                    ++numDynSymTabs;
                    _dyn_syms.reset( _fd, segHdrs[i], segHdrs.get(), numSegHdrs );
                    break;
                case SHT_PROGBITS:
                    
                    break;
            }
        }
    }

    std::string find_symbol( void *addr, bool include_offset = true )
    {
        uintptr_t off = uintptr_t(addr) - _base;
        std::string ret;

        uintptr_t symoff = 0;
        for ( size_t s = 0; ret.empty() && s < _dyn_syms.size(); ++s )
        {
            const ElfW(Sym) &t = _dyn_syms[s];
            if ( t.st_name &&
                 ( ELF64_ST_TYPE( t.st_info ) == STT_FUNC ||
                   ELF64_ST_TYPE( t.st_info ) == STT_OBJECT ) &&
                 t.st_shndx != SHN_UNDEF &&
                 t.st_size > 0 )
            {
                if ( t.st_value <= off && off <= (t.st_value + t.st_size) )
                {
                    ret = demangle(_dyn_syms.name( s ));
                    symoff = off - t.st_value;
                }
            }
        }
        
        for ( size_t s = 0; ret.empty() && s < _nondyn_syms.size(); ++s )
        {
            const ElfW(Sym) &t = _nondyn_syms[s];
            if ( t.st_name &&
                 ( ELF64_ST_TYPE( t.st_info ) == STT_FUNC ||
                   ELF64_ST_TYPE( t.st_info ) == STT_OBJECT ) &&
                 t.st_shndx != SHN_UNDEF &&
                 t.st_size > 0 )
            {
                if ( t.st_value <= off && off <= (t.st_value + t.st_size) )
                {
                    ret = demangle(_nondyn_syms.name( s ));
                    symoff = off - t.st_value;
                }
            }
        }

        if ( include_offset && symoff > 0 )
        {
            std::stringstream sbuf;
            sbuf << " [+0x" << std::hex << symoff << ']';
            ret += sbuf.str();
        }

        return ret;
    }

    std::string _path;
    std::string _name;
    uintptr_t _base = 0;
    int _fd = -1;
    symbol_table _dyn_syms;
    symbol_table _nondyn_syms;
};

namespace {

static int fillActiveCB( struct dl_phdr_info *phdr, size_t size, void *data )
{
    using listtype = std::vector<std::shared_ptr<active_shared_object>>;
    listtype *dsos = static_cast<listtype *>( data );
    if ( dsos )
    {
        dsos->emplace_back( std::make_shared<active_shared_object>( phdr ) );
        return 0;
    }
    return 1;
}

}

#endif // HAS_ELF_DLFCN

////////////////////////////////////////

active_shared_object::active_shared_object( const char *name, uintptr_t base_addr )
#ifdef HAS_ELF_DLFCN
    : _impl( new impl( name, base_addr ) )
#endif
{
}

////////////////////////////////////////

active_shared_object::active_shared_object( void *phdr )
#ifdef HAS_ELF_DLFCN
    : _impl( new impl( static_cast<struct dl_phdr_info *>( phdr ) ) )
#endif
{
}

////////////////////////////////////////

const std::string &active_shared_object::path( void ) const
{
    return _impl->path();
}

////////////////////////////////////////

const std::string &active_shared_object::name( void ) const
{
    return _impl->name();
}

////////////////////////////////////////

uintptr_t active_shared_object::base( void ) const
{
    return _impl->base();
}

////////////////////////////////////////

bool active_shared_object::is_closest( void *addr, const active_shared_object *curdso )
{
    return _impl->is_closest( addr, curdso->_impl.get() );
}

////////////////////////////////////////

std::string active_shared_object::find_symbol( void *addr, bool include_offset )
{
    return _impl->find_symbol( addr, include_offset );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

active_objects::active_objects()
{
#ifdef HAS_ELF_DLFCN
    dl_iterate_phdr( &fillActiveCB, &_dsos );
#else
# error "NYI"
#endif
}

////////////////////////////////////////

std::shared_ptr<active_shared_object> active_objects::find_dso( void *addr ) const
{
    std::shared_ptr<active_shared_object> cur;
    for ( const auto &i: _dsos )
    {
        if ( i->is_closest( addr, cur.get() ) )
            cur = i;
    }
    return cur;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void *find_next( const char *sig, const char *ver )
{
#ifdef HAS_ELF_DLFCN
    if ( ver )
        return dlvsym( RTLD_NEXT, sig, ver );

    return dlsym( RTLD_NEXT, sig );
#else
# error "NYI"
#endif
}

} // namespace dl_extra

////////////////////////////////////////

} // namespace base



