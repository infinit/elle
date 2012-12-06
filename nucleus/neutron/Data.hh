#ifndef NUCLEUS_NEUTRON_DATA_HH
# define NUCLEUS_NEUTRON_DATA_HH

# include <elle/types.hh>
# include <elle/standalone/Region.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/neutron/Offset.hh>

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
    /// XXX rewrite
    ///
    class Data:
      public proton::Value,
      public elle::serialize::SerializableMixin<Data>
    {
    public:
      //
      // types
      //
      typedef Offset            K;

      //
      // constants
      //
    public:
      struct Constants
      {
        static proton::Nature const seam;
        static proton::Nature const quill;
        static proton::Nature const value;
        static proton::Nature const nature;
      };

      //
      // constructors & destructors
      //
      Data();

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
      // printable
      virtual
      void
      print(std::ostream& stream) const;

      // value
      elle::Boolean
      empty() const { return true; } // XXX
      elle::String
      mayor() const { return elle::String(); } // XXX

      // serialize
      ELLE_SERIALIZE_FRIEND_FOR(Data);

      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Data);

      //
      // attributes
      //
      elle::standalone::Region region;
    };

  }
}

# include <nucleus/neutron/Data.hxx>

#endif
