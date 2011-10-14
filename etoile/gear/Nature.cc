//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        Alexandre Bique   [wed oct 12 18:25:21 2011]
//

#include <etoile/gear/Nature.hh>

namespace etoile
{
  namespace gear
  {
    const char * NatureName(Nature nature)
    {
      switch (nature)
        {
        case NatureUnknown:   return "NatureUnknonw";
        case NatureObject:    return "NatureObject";
        case NatureFile:      return "NatureFile";
        case NatureDirectory: return "NatureDirectory";
        case NatureLink:      return "NatureLink";
        default:              return "NatureInvalid";
        }
    }
  }
}
