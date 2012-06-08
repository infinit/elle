#ifndef ELLE_SERIALIZE_INIARCHIVE_HH
# define ELLE_SERIALIZE_INIARCHIVE_HH

# include <elle/format/ini.fwd.hh>

# include "detail/MergeArchive.hh"

# include "ArchiveMode.hh"
# include "BaseArchive.hh"

namespace elle { namespace serialize {

      class OutputIniArchive
        : protected BaseArchive<ArchiveMode::Output, OutputIniArchive>
      {
      protected:
        typedef BaseArchive<ArchiveMode::Output, OutputIniArchive> BaseClass;
        friend class BaseClass::Access;

      public:
        OutputIniArchive(StreamType& stream);
        OutputIniArchive(StreamType& stream, elle::format::ini::File const& ini_file);

      public:
        OutputIniArchive& operator <<(elle::format::ini::File const& ini_file);
        OutputIniArchive& operator <<(elle::format::ini::Section const& section);
      };

      class InputIniArchive
        : protected BaseArchive<ArchiveMode::Input, InputIniArchive>
      {
      protected:
        typedef BaseArchive<ArchiveMode::Input, InputIniArchive> BaseClass;
        friend class BaseClass::Access;

      public:
        InputIniArchive(StreamType& stream);

      public:
        InputIniArchive& operator >>(elle::format::ini::File& file);
      };

      ELLE_SERIALIZE_MERGE_ARCHIVES(IniArchive, InputIniArchive, OutputIniArchive);
}}



#endif /* ! INIARCHIVE_HH */


