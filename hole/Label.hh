#ifndef HOLE_LABEL_HH
# define HOLE_LABEL_HH

# include <elle/types.hh>
# include <elle/serialize/fwd.hh>
# include <elle/io/Dumpable.hh>

# include <cryptography/fwd.hh>
# include <cryptography/Digest.hh>
# include <cryptography/oneway.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

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
    public elle::io::Dumpable
  {
    /*----------.
    | Constants |
    `----------*/
  public:
    struct Algorithms
    {
      static const cryptography::oneway::Algorithm oneway;
    };

    /*-------------.
    | Construction |
    `-------------*/
  public:
    Label(); // XXX[deserialize]
    /// Create the label based on an element by computing its digest.
    template <typename T>
    Label(T const& element);

    /*----------.
    | Operators |
    `----------*/
  public:
    elle::Boolean
    operator ==(Label const& other) const;
    elle::Boolean
    operator <(Label const& other) const;
    elle::Boolean
    operator <=(Label const& other) const;
    ELLE_OPERATOR_GT(Label);
    ELLE_OPERATOR_GTE(Label);
    ELLE_OPERATOR_NO_ASSIGNMENT(Label);

    /*-----------.
    | Interfaces |
    `-----------*/
  public:
    // dumpable
    elle::Status        Dump(const elle::Natural32 = 0) const;
    // serializable
    ELLE_SERIALIZE_FRIEND_FOR(Label);

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    ELLE_ATTRIBUTE(cryptography::Digest, digest);
  };

}

# include <hole/Label.hxx>

#endif
