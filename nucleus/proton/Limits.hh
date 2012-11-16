#ifndef NUCLEUS_PROTON_LIMITS_HH
# define NUCLEUS_PROTON_LIMITS_HH

# include <elle/types.hh>
# include <elle/io/Dumpable.hh>

namespace nucleus
{
  namespace proton
  {

    /// XXX bien expliquer que par exemple un extent de 512 octets avec 0.5 et 0.2
    /// ca fera que un nodule avec une seule entree soit au dessus de la limite basse.
    /// donc l'algo ne traiterait pas ces nodules. attention donc d'utiliser des nombres
    /// qui ont du sens. -> cela dit le systeme doit s'en premunir.
    class Limits:
      public elle::io::Dumpable
    {
      //
      // constructors & destructor
      //
    public:
      Limits();
      Limits(elle::Natural32 const extent,
             elle::Real const contention,
             elle::Real const balancing);

      //
      // methods
      //
    public:
      /// XXX
      elle::Natural32
      extent() const;
      /// XXX
      elle::Real
      contention() const;
      /// XXX
      elle::Real
      balancing() const;

      //
      // interfaces
      //
    public:
      // dumpable
      elle::Status Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
    private:
      /// XXX
      elle::Natural32 _extent;
      /// XXX
      elle::Real _contention;
      /// XXX
      elle::Real _balancing;
    };

  }
}

#endif
