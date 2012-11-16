#ifndef NUCLEUS_NEUTRON_COMPONENT_HH
# define NUCLEUS_NEUTRON_COMPONENT_HH

# include <iosfwd>

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this enumeration defines the block's logical component.
    ///
    enum Component
      {
        ComponentUnknown = 0, // XXX

        ComponentObject,
        ComponentContents,
        ComponentAccess,
        ComponentGroup,
        ComponentEnsemble,

        ComponentSeamCatalog,
        ComponentQuillCatalog,
        // XXX

        Components // XXX
      };

    std::ostream&
    operator <<(std::ostream& stream,
                Component const component);

  }
}

#endif
