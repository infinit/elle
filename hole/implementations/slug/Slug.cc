#include <hole/implementations/slug/Slug.hh>
#include <hole/implementations/slug/Machine.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- definitions -----------------------------------------------------
//

      ///
      /// this value defines the component's name.
      ///
      const elle::Character             Component[] = "slug";

      ///
      /// XXX
      ///
      Machine*                          Slug::Computer = NULL;

    }
  }
}
