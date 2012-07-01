#ifndef ELLE_STANDALONE_LOG_HH
#define ELLE_STANDALONE_LOG_HH

#include <elle/types.hh>

#include <elle/radix/Meta.hh>

#include <elle/idiom/Close.hh>
# include <fstream>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace standalone
  {

    ///
    /// this class provides methods for recording errors, warnings etc.
    /// so that these are stored directly in a log file.
    ///
    class Log:
      public radix::Meta
    {
    public:
      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();

      static Status     Setup(const String&);

      static Status     Instance(Log*&);

      //
      // static attributes
      //
      static Log*               Current;

      //
      // constructors & destructors
      //
      Log(const String&);
      ~Log();

      //
      // methods
      //
      Void              Record(const String&,
                               const String&,
                               const String&);

      //
      // attributes
      //
      std::ofstream     stream_;
    };

  }
}

#endif
