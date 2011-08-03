//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/standalone/Log.hh
//
// created       julien quintard   [wed jul 27 09:24:44 2011]
// updated       julien quintard   [mon aug  1 10:27:22 2011]
//

#ifndef ELLE_STANDALONE_LOG_HH
#define ELLE_STANDALONE_LOG_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>
#include <elle/core/String.hh>

#include <elle/radix/Meta.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace standalone
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Log:
      public Meta
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Setup(const String&);

      static Status	Instance(Log*&);

      //
      // static attributes
      //
      static Log*		Current;

      //
      // constructors & destructors
      //
      Log(const String&);
      ~Log();

      //
      // methods
      //
      Void		Record(const String&,
			       const String&,
			       const String&);

      //
      // attributes
      //
      int		fd;
    };

  }
}

#endif
