#ifndef NUCLEUS_NEUTRON_REFERENCE_HH
# define NUCLEUS_NEUTRON_REFERENCE_HH

# include <elle/types.hh>

# include <nucleus/proton/fwd.hh>
# include <nucleus/proton/Value.hh>
# include <nucleus/proton/Contents.hh>
# include <nucleus/neutron/fwd.hh>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this class represents a link-specific content.
    ///
    /// note that the Reference does not derive the Block class. indeed, the
    /// Contents class represents the container for genre-specific content:
    /// Catalog for directories, Data for files etc.
    ///
    /// XXX rewrite
    ///
    /// XXX no split for references
    ///
    class Reference:
      public proton::Value,
      public elle::serialize::SerializableMixin<Reference>
    {
    public:
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
      Reference();

      //
      // methods
      //
      elle::Status      Create();

      elle::Status      Target(const elle::String&);
      elle::Status      Resolve(elle::String&) const;

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
      ELLE_SERIALIZE_FRIEND_FOR(Reference);

      ELLE_SERIALIZE_SERIALIZABLE_METHODS(Reference);

      //
      // attributes
      //
      elle::String      target;
    };

  }
}

# include <nucleus/neutron/Reference.hxx>

#endif
