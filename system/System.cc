//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/system/System.cc
//
// created       julien quintard   [mon jan 26 20:23:52 2009]
// updated       julien quintard   [wed mar 24 22:14:56 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/System.hh>

///
/// these includes are placed here in order to prevent
/// pre-processing conflicts.
///
#include <elle/misc/Report.hh>
#include <elle/misc/Maid.hh>

namespace elle
{
  namespace system
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable defines the byte ordering of the running system.
    ///
    System::Order		System::Endianness;

    ///
    /// these variables defines some path-related constants.
    ///
    Character			System::Path::Separator =
      ::QDir::separator().toAscii();
    String			System::Path::Root =
      ::QDir::root().canonicalPath().toStdString();
    String			System::Path::Current =
      ::QDir::current().canonicalPath().toStdString();
    String			System::Path::Home =
      ::QDir::home().canonicalPath().toStdString();

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the system module.
    ///
    Status		System::Initialize()
    {
      enter();

      // define the endianness.
      switch (::QSysInfo::ByteOrder)
	{
	case ::QSysInfo::LittleEndian:
	  {
	    System::Endianness = System::OrderLittle;

	    break;
	  }
	case ::QSysInfo::BigEndian:
	  {
	    System::Endianness = System::OrderBig;

	    break;
	  }
	}

      leave();
    }

    ///
    /// this method cleans the system module.
    ///
    Status		System::Clean()
    {
      enter();

      // nothing to do

      leave();
    }

  }
}
