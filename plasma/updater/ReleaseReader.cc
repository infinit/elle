//
// ---------- header ----------------------------------------------------------
//
// project       plasma/updater
//
// license       infinit
//
// author        Raphael Londeix   [Mon 30 Jan 2012 12:51:40 PM CET]
//

//
// ---------- includes --------------------------------------------------------
//

#include "ReleaseReader.hh"

using namespace plasma::updater;

//
// ---------- constructors & destructors --------------------------------------
//

ReleaseReader::ReleaseReader() :
  _document("ReleaseDescription")
{}

//
// ---------- methods ---------------------------------------------------------
//

bool ReleaseReader::Feed(QByteArray const& data)
{
  if (!this->_document.setContent(data))
    return false;
  QDomElement root = this->_document.documentElement();
  if (root.tagName() != "release")
    return false;
}
