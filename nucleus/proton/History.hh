#ifndef NUCLEUS_PROTON_HISTORY_HH
# define NUCLEUS_PROTON_HISTORY_HH

# include <elle/concept/Fileable.hh>
# include <elle/serialize/Serializable.hh>
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
    class History:
      public elle::radix::Object,
      public elle::serialize::SerializableMixin<History>,
      public elle::concept::Fileable<>
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

    private:
      /// XXX
      static
      elle::io::Path
      _path(Network const& network,
            Address const& address);

      //
      // interfaces
      //
    public:
      // object
      declare(History);
      elle::Boolean     operator==(const History&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // fileable
      ELLE_CONCEPT_FILEABLE_METHODS();

      void
      load(Network const& network,
           Address const& address);
      void
      store(Network const& network,
            Address const& address) const;
      static
      void
      erase(Network const& network,
            Address const& address);
      static
      elle::Boolean
      exists(Network const& network,
             Address const& address);

      //
      // attributes
      //
      Container         container;
    };

  }
}

# include <nucleus/proton/History.hxx>

#endif
