#ifndef INFINIT_CRYPTOGRAPHY_SIGNATURE_HXX
# define INFINIT_CRYPTOGRAPHY_SIGNATURE_HXX

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Signature,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
