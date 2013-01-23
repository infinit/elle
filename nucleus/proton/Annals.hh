#ifndef NUCLEUS_PROTON_ANNALS_HH
# define NUCLEUS_PROTON_ANNALS_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Printable.hh>

# include <nucleus/proton/fwd.hh>

# include <boost/noncopyable.hpp>

# include <vector>

namespace nucleus
{
  namespace proton
  {
    /// XXX
    class Annals:
      public elle::Printable,
      private boost::noncopyable
    {
      /*------.
      | Types |
      `------*/
    public:
      typedef std::vector<std::unique_ptr<Clef const>> Container;
      typedef typename Container::iterator Iterator;
      typedef typename Container::const_iterator Scoutor;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      Annals() = default;

      /*--------.
      | Methods |
      `--------*/
    public:
      /// XXX
      void
      record(std::unique_ptr<Clef const>&& clef);

      /*----------.
      | Operators |
      `----------*/
    public:
      ELLE_OPERATOR_NO_ASSIGNMENT(Annals);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // printable
      virtual
      void
      print(std::ostream& stream) const;
      // iterable
      Scoutor
      begin() const;
      Scoutor
      end() const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE(Container, container);
    };
  }
}

#endif
