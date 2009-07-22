#ifndef HOLE_KEY_HH
# define HOLE_KEY_HH

# include "hole/FreeList.hh"

namespace hole
{
  class Key : public FreeList<Key>
  {
  };
}

#endif /* !HOLE_KEY_HH */
