#ifndef NUCLEUS_NEUTRON_DATA_HH
# define NUCLEUS_NEUTRON_DATA_HH

# include <elle/types.hh>
# include <elle/standalone/Region.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/neutron/fwd.hh>

# include <boost/noncopyable.hpp>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a file's content.
    ///
    /// note that the Data does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    class Data:
      private boost::noncopyable
    {
      //
      // constants
      //
    public:
      static const Component component = ComponentData;

    public:
      //
      // constructors & destructors
      //
      Data(proton::Contents<Data>&);

      //
      // methods
      //
      elle::Status      Create();

      elle::Status      Write(const Offset&,
                              const elle::standalone::Region&);
      elle::Status      Read(const Offset&,
                             const Size&,
                             elle::standalone::Region&) const;
      elle::Status      Adjust(const Size&);

      elle::Status      Capacity(Size&) const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      proton::Contents<Data>&           contents;

      elle::standalone::Region                      region;
    };

  }
}

#include <nucleus/neutron/Data.hxx>

#endif
