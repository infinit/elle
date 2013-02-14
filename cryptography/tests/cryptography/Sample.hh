#ifndef SAMPLE_HH
# define SAMPLE_HH

# include <cryptography/random.hh>

# include <elle/types.hh>
# include <elle/attribute.hh>
# include <elle/operator.hh>
# include <elle/serialize/Serializer.hh>

# include <iostream>

/// A test class for encrypting a complex type.
class Class
{
  /*-------------.
  | Construction |
  `-------------*/
public:
  Class():
    _salt(0.0)
  {
    // XXX[virer pour le load constructor]
  }

  Class(elle::Real const& salt,
        elle::String const& string):
    _salt(salt),
    _string(string)
  {
  }

  Class(Class const&) = default;

  /*----------.
  | Operators |
  `----------*/
public:
  elle::Boolean
  operator ==(Class const& other) const
  {
    return ((this->_salt == other._salt) &&
            (this->_string == other._string));
  }
  ELLE_OPERATOR_NEQ(Class);

  /*-----------.
  | Interfaces |
  `-----------*/
public:
  // serializable
  ELLE_SERIALIZE_FRIEND_FOR(Class);

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

ELLE_SERIALIZE_SIMPLE(Class,
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

std::ostream&
operator <<(std::ostream& stream,
            Class const& x)
{
  stream << "(" << x.salt() << ", " << x.string() << ")";

  return (stream);
}

#endif
