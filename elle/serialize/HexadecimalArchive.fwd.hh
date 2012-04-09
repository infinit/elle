#ifndef ELLE_SERIALIZE_HEXADECIMALARCHIVE_FWD_HH
# define ELLE_SERIALIZE_HEXADECIMALARCHIVE_FWD_HH

# include "ArchiveMode.hh"

namespace elle { namespace serialize {

    template<ArchiveMode mode> class HexadecimalArchive;

    typedef HexadecimalArchive<ArchiveMode::Output> OutputHexadecimalArchive;
    typedef HexadecimalArchive<ArchiveMode::Input> InputHexadecimalArchive;

}}

#endif /* ! HEXADECIMALARCHIVE_FWD_HH */


