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

#include <iostream>

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
  std::cout << "Release " << root.attribute("major").toStdString()
            << "."
            << root.attribute("minor").toStdString() << std::endl;
  QDomNode node = root.firstChild();
  while (!node.isNull())
    {
      QDomElement element = node.toElement();
      if (element.tagName() == "executable")
        {
          this->_to_download.push(File{
              element.attribute("path").toStdString(),
              element.attribute("md5sum").toStdString()
          });
          std::cout << "Found "
                    << this->_to_download.front().relpath << " ("
                    << this->_to_download.front().md5sum << ")\n";
        }
      else
        {
          std::cerr << "Unknown tag name '" << element.tagName().toStdString() << "'\n";
        }
      node = node.nextSibling();
    }
  return true;
}
