#ifndef HOLE_LABEL_HXX
# define HOLE_LABEL_HXX

namespace hole
{

  /*-------------.
  | Construction |
  `-------------*/

  template <typename T>
  Label::Label(T const& element):
    _digest(cryptography::oneway::hash(element,
                                       Label::Constants::oneway_algorithm))
  {
  }

}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Pointer.hh>
# include <cryptography/Digest.hh>

ELLE_SERIALIZE_SIMPLE(hole::Label,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._digest;
}

#endif
