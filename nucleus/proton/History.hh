#ifndef NUCLEUS_PROTON_HISTORY_HH
# define NUCLEUS_PROTON_HISTORY_HH

# include <elle/concept/Fileable.hh>
# include <elle/concept/Serializable.hh>
# include <elle/radix/Object.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Version.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class contains all the version numbers associated with
    /// a given mutable block.
    ///
    class History
      : public elle::radix::Object
      , public elle::concept::Serializable<History>
      , public elle::concept::Fileable<>
    {
    public:
      //
      // constants
      //
      static const elle::String         Extension;

      //
      // types
      //
      typedef std::vector<Version>                      Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // methods
      //
      elle::Status      Register(const Version&);

      elle::Status      Select(const Version::Type,
                               Version&) const;

      elle::Status      Size(Version::Type&) const;

      //
      // interfaces
      //

      // object
      declare(History);
      elle::Boolean     operator==(const History&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      using elle::concept::Fileable<>::Load;
      using elle::concept::Fileable<>::Store;

      elle::Status      Load(const Network&,
                             const Address&);
      elle::Status      Store(const Network&,
                              const Address&) const;
      elle::Status      Erase(const Network&,
                              const Address&) const;
      elle::Status      Exist(const Network&,
                              const Address&) const;

      //
      // attributes
      //
      Container         container;
    };

  }
}

# include <nucleus/proton/History.hxx>

#endif
