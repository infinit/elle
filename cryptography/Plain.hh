#ifndef ELLE_CRYPTOGRAPHY_PLAIN_HH
# define ELLE_CRYPTOGRAPHY_PLAIN_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/Buffer.hh>

# include <elle/io/Dumpable.hh>

# include <elle/cryptography/fwd.hh>

# include <boost/noncopyable.hpp>

namespace elle
{
  namespace cryptography
  {

    class Plain:
      public elle::io::Dumpable,
      private boost::noncopyable
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      explicit
      Plain(WeakBuffer const& buffer);

      /*----------.
      | Operators |
      `----------*/
    public:
      Boolean
      operator ==(Plain const& other) const;
      ELLE_OPERATOR_NEQ(Plain);
      ELLE_OPERATOR_NO_ASSIGNMENT(Plain);

      Boolean
      operator ==(Clear const& other) const;
      ELLE_OPERATOR_NEQ(Clear);

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      // dumpable
      elle::Status
      Dump(const elle::Natural32 = 0) const;

      /*-----------.
      | Attributes |
      `-----------*/
    private:
      ELLE_ATTRIBUTE_R(WeakBuffer const&, buffer);
    };

  }
}

#endif
