#ifndef HORIZON_WARD_HH
# define HORIZON_WARD_HH

# include <etoile/gear/Identifier.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>

namespace horizon
{

  /// Provides a helper for making sure objects are discarded on errors.
  class Ward
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    Ward(etoile::gear::Identifier const& id);
    ~Ward();

    /*--------.
    | Methods |
    `--------*/
  public:
    void
    release();

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    ELLE_ATTRIBUTE(etoile::gear::Identifier, id);
    ELLE_ATTRIBUTE(elle::Boolean, clean);
  };

}

#endif
