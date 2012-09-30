#ifndef NUCLEUS_PROTON_HISTORY_HH
# define NUCLEUS_PROTON_HISTORY_HH

# include <elle/concept/Fileable.hh>
# include <elle/serialize/Serializable.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Revision.hh>

# include <elle/idiom/Close.hh>
#  include <vector>
# include <elle/idiom/Open.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class contains all the revision numbers associated with
    /// a given mutable block.
    ///
    class History:
      public elle::serialize::SerializableMixin<History>,
      public elle::concept::Fileable<>,
      private boost::noncopyable
    {
    public:
      //
      // constants
      //
      static const elle::String         Extension;

      //
      // types
      //
      typedef std::vector<Revision>                      Container;
      typedef typename Container::iterator              Iterator;
      typedef typename Container::const_iterator        Scoutor;

      //
      // methods
      //
      elle::Status      Register(const Revision&);

      elle::Status      Select(const Revision::Type,
                               Revision&) const;

      elle::Status      Size(Revision::Type&) const;

    private:
      /// XXX
      static
      elle::io::Path
      _path(Network const& network,
            Address const& address);

      //
      // operators
      //
    public:
      elle::Boolean
      operator ==(History const& other) const;
      ELLE_OPERATOR_NEQ(History);

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;
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
    private:
      ELLE_ATTRIBUTE(Container, container);
    };

  }
}

# include <nucleus/proton/History.hxx>

#endif
