#ifndef ELLE_SERIALIZE_BINARYARCHIVE_FWD_HH
# define ELLE_SERIALIZE_BINARYARCHIVE_FWD_HH

# include <elle/serialize/ArchiveMode.hh>

namespace elle
{
  namespace serialize
  {
    typedef char DefaultCharType;
    template<ArchiveMode, typename> struct DefaultStreamTypeSelect;

    template<
        ArchiveMode mode
    > class BinaryArchive;

    typedef BinaryArchive<ArchiveMode::Output> OutputBinaryArchive;
    typedef BinaryArchive<ArchiveMode::Input>  InputBinaryArchive;

  }
} // !namespace elle::serialize

#endif /* ! BINARYARCHIVE_FWD_HH */


