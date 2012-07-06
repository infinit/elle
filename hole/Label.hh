#ifndef HOLE_LABEL_HH
# define HOLE_LABEL_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/cryptography/fwd.hh>

# include <elle/idiom/Open.hh>

namespace hole
{

  ///
  /// this class represents the identifier of a node.
  ///
  /// note that blocks are identified by an Address. in order to locate
  /// the node responsible for storing a block replicas, an algorithm is
  /// run, picking some of the network nodes for that purpose. this
  /// algorithm needs to compare the address with the node identifiers.
  ///
  /// in order to make things easier, both addresses and node identifiers i.e
  /// labels lie in the same namespace. indeed, both are digests which
  /// can be easily turned into hexadecimal strings for instance.
  ///
  class Label:
    public elle::radix::Object
  {
  public:
    //
    // constants
    //
    static const Label          Null;

    //
    // constructors & destructors
    //
    Label();
    Label(const Label&);
    ~Label();

    //
    // methods
    //
    template <typename T>
    elle::Status        Create(const T&);

    //
    // interfaces
    //

    // object
    declare(Label);
    elle::Boolean       operator==(const Label&) const;
    elle::Boolean       operator<(const Label&) const;

    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;

    //
    // attributes
    //
    elle::cryptography::Digest*       digest;
  };

}

# include <hole/Label.hxx>

#endif
