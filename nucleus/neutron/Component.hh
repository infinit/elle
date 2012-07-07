#ifndef NUCLEUS_NEUTRON_COMPONENT_HH
# define NUCLEUS_NEUTRON_COMPONENT_HH

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this enumeration defines the block's logical component.
    ///
    enum Component
      {
        ComponentUnknown = 0,

        ComponentObject,
        ComponentContents,
        ComponentData,
        ComponentCatalog,
        ComponentReference,
        ComponentAccess,
        ComponentUser,
        ComponentGroup,

        ComponentSeamCatalog,
        ComponentQuillCatalog,

        Components = ComponentAccess + 1
      };

  }
}

#endif
