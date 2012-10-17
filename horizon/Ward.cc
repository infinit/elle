#include <horizon/Ward.hh>

#include <etoile/wall/Object.hh>

namespace horizon
{

  /*-------------.
  | Construction |
  `-------------*/

  Ward::Ward(etoile::gear::Identifier const& id):
    _id(id),
    _clean(true)
  {
  }

  Ward::~Ward()
  {
    if (this->_clean)
      etoile::wall::Object::Discard(this->_id);
  }

  /*--------.
  | Methods |
  `--------*/

  void Ward::release()
  {
    this->_clean = false;
  }

}
