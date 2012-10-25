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
      {
        try
          {
            etoile::wall::Object::discard(this->_id);
          }
        catch (...)
          {
            // Ignore all the exceptions: we do not want exceptions to get out
            // as we may already be throwing an exception, which would have
            // called the Ward destructor.
          }
      }
  }

  /*--------.
  | Methods |
  `--------*/

  void Ward::release()
  {
    this->_clean = false;
  }

}
