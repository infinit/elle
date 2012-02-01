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
      this->_ReadElement(node.toElement());
      node = node.nextSibling();
    }
  return this->files.size() > 0;
}

void ReleaseReader::_ReadElement(QDomElement const& element)
{
  if (element.tagName() == "executable")
    {
      File file = {
          File::Type::Executable,
          element.attribute("path").toStdString(),
          element.attribute("md5sum").toStdString()
      };
      std::cout << "Found " << file.relpath << " (" << file.md5sum << ")\n";
      this->files.push_back(file);
    }
  else
    {
      std::cerr << "Unknown tag name '"
                << element.tagName().toStdString() << "'\n";
    }
}
