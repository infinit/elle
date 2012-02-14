//
// ---------- header ----------------------------------------------------------
//
// project       @FIXME@
//
// license       infinit
//
// author        Raphael Londeix   [Mon 30 Jan 2012 12:47:30 PM CET]
//

#ifndef PLASMA_UPDATER_RELEASEREADER_HH
#define PLASMA_UPDATER_RELEASEREADER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <string>
#include <vector>

#include <QDomDocument>

namespace plasma {
    namespace updater {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// Convert a XML release description file into a simple queue of File
    ///
    class ReleaseReader
    {
    public:
      ///
      /// Represent a file entry
      ///
      struct File
      {
      public:
        enum Type
        {
          Executable = 0,
        };

      public:
        Type        type;
        QString relpath;
        QString md5sum;
      };

    public:
      /// This attribute is filled with the Feed() method
      std::vector<File>        files;
    private:
      QDomDocument            _document;

    public:
      ReleaseReader();
      /// Feed the reader with the raw XML data
      /// No check, nor states, are saved accross multiple calls
      /// This method grows the file attribute
      bool Feed(QByteArray const& data);
    private:
      void _ReadElement(QDomElement const& element);
    };

//
// ---------- templates -------------------------------------------------------
//

  }
}
#endif /* ! RELEASEREADER_HH */


