#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# ifndef INFINIT_CRYPTOGRAPHY_LEGACY_OUTPUT_HXX
#  define INFINIT_CRYPTOGRAPHY_LEGACY_OUTPUT_HXX

/*----------.
| Serialize |
`----------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Output,
                      archive,
                      value,
                      format)
{
  enforce(format == 0);

  archive & value._buffer;
}
# endif

#else
# warning "LEGACY: this file should not have been included"
#endif
