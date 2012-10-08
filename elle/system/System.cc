#include <elle/system/System.hh>

#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
#include <boost/detail/endian.hpp>
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
    System::Order               System::Endianness;

    ///

/// this variable defines some path-related constants.
    /// old
    ///
    Character                   System::Path::Separator;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initializes the system module.
    ///
    Status              System::Initialize()
    {
      //
      // set the static definitions.
      //

      System::Path::Separator = '/';

      // define the endianness.
#if defined(BOOST_BIG_ENDIAN)
        System::Endianness = System::OrderBig;
#elif defined(BOOST_LITTLE_ENDIAN)
        System::Endianness = System::OrderLittle;
#elif defined(BOOST_PDP_ENDIAN)
        System::Endianness = System::OrderMiddle;
#endif

      return Status::Ok;
    }

    ///
    /// this method cleans the system module.
    ///
    Status              System::Clean()
    {
      // nothing to do

      return Status::Ok;
    }

  }
}
