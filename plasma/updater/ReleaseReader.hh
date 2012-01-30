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

#include <QDomDocument>

namespace plasma {
    namespace updater {

//
// ---------- classes ---------------------------------------------------------
//

    class ReleaseReader
    {
    private:
      QDomDocument _document;

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


