#ifndef HORIZON_LINUX_CRIB_HH
# define HORIZON_LINUX_CRIB_HH

# include <elle/types.hh>

# include <horizon/linux/Handle.hh>

# include <elle/idiom/Close.hh>
#  include <map>
# include <elle/idiom/Open.hh>

namespace horizon
{
  namespace linux
  {

    ///
    /// this class holds the relations between created files' way and handle.
    ///
    class Crib
    {
    public:
      //
      // types
      //
      typedef std::map<const elle::String, Handle*>     Container;
      typedef Container::iterator                       Iterator;

      //
      // static methods
      //
      static elle::Status       Add(const elle::String&,
                                    Handle*);
      static elle::Status       Exist(const elle::String&);
      static elle::Status       Retrieve(const elle::String&,
                                         Handle*&);
      static elle::Status       Remove(const elle::String&);

      static elle::Status       Show(const elle::Natural32 = 0);

      //
      // static attributes
      //
      static Container          Handles;
    };

  }
}

#endif
