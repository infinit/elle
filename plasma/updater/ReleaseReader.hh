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
#include <queue>

#include <QDomDocument>

namespace plasma {
    namespace updater {

//
// ---------- classes ---------------------------------------------------------
//

    class ReleaseReader
    {
    private:
      struct File
      {
        std::string relpath;
        std::string md5sum;
      };

    private:
      std::queue<File>        _to_download;
      QDomDocument            _document;

    public:
      ReleaseReader();
      bool Feed(QByteArray const& data);
    };

//
// ---------- templates -------------------------------------------------------
//

  }
}
#endif /* ! RELEASEREADER_HH */


