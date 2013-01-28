#ifndef INFINIT_CRYPTOGRAPHY_INTERFACE_HXX
# define INFINIT_CRYPTOGRAPHY_INTERFACE_HXX

//
// ---------- Public Key ------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::interface::PublicKey,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  // XXX[nothing to do]
  (void)archive;
  (void)value;
}

//
// ---------- Private Key -----------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::interface::PrivateKey,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  // XXX[nothing to do]
  (void)archive;
  (void)value;
}

#endif
