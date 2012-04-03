#ifndef ELLE_SERIALIZE_BINARYARCHIVE_HH
# define ELLE_SERIALIZE_BINARYARCHIVE_HH

# include "BaseArchive.hh"

# include "BinaryArchive.fwd.hh"

namespace elle { namespace serialize {

    template<ArchiveMode mode>
      class BinaryArchive :
        public BaseArchive<mode, BinaryArchive<mode>>
      {
      private:
        typedef BaseArchive<mode, BinaryArchive<mode>> BaseClass;
        typedef typename BaseClass::StreamType StreamType;

      public:
        BinaryArchive(StreamType& stream) : BaseClass(stream) {}
      };

}} // !elle::serialize

#endif /* ! BINARYARCHIVE_HH */


