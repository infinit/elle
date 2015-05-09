#ifndef SAMPLE_HH
# define SAMPLE_HH

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/Serializer.hh>

# include <iostream>

/// A test class for encrypting a complex type.
class Sample
{
  /*-------------.
  | Construction |
  `-------------*/
public:
  Sample():
    _salt(0.0)
  {
    // XXX[virer pour le load constructor]
  }

  Sample(elle::Real const& salt,
         elle::String const& string):
    _salt(salt),
    _string(string)
  {
  }

  Sample(Sample const&) = default;

  /*----------.
  | Operators |
  `----------*/
public:
  elle::Boolean
  operator ==(Sample const& other) const
  {
    return ((this->_salt == other._salt) &&
            (this->_string == other._string));
  }
  ELLE_OPERATOR_NEQ(Sample);

  /*-----------.
  | Interfaces |
  `-----------*/
public:
  // serializable
  ELLE_SERIALIZE_FRIEND_FOR(Sample);

  /*-----------.
  | Attributes |
  `-----------*/
private:
  ELLE_ATTRIBUTE_R(elle::Real, salt);
  ELLE_ATTRIBUTE_R(elle::String, string);
};

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(Sample,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  archive & value._salt;
  archive & value._string;
}

/*----------.
| Operators |
`----------*/

static
std::ostream&
operator <<(std::ostream& stream,
            Sample const& x)
{
  stream << "(" << x.salt() << ", " << x.string() << ")";

  return (stream);
}

#endif
