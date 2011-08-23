//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/system/System.cc
//
// created       julien quintard   [mon jan 26 20:23:52 2009]
// updated       julien quintard   [fri aug 12 14:01:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/System.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <QSysInfo>
# include <QDir>
#include <elle/idiom/Open.hh>

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
    Character			System::Path::Separator;
    String			System::Path::Root;
    String			System::Path::Current;
    String			System::Path::Home;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the system module.
    ///
    Status		System::Initialize()
    {
      enter();

      //
      // set the static definitions.
      //

      System::Path::Separator =
	::QDir::separator().toAscii();

      System::Path::Root =
	::QDir::root().canonicalPath().toStdString();

      System::Path::Current =
	::QDir::current().canonicalPath().toStdString();

      if (::getenv("INFINIT_HOME") == NULL)
	{
	  System::Path::Home =
	    ::QDir::home().canonicalPath().toStdString();
	}
      else
	{
	  System::Path::Home.assign(::getenv("INFINIT_HOME"));
	}

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
