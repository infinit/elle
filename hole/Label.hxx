#ifndef HOLE_LABEL_HXX
# define HOLE_LABEL_HXX

# include <elle/idiom/Open.hh>

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

# include <elle/idiom/Close.hh>
#  include <cassert>
# include <elle/idiom/Open.hh>

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
