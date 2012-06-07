#ifndef  LUNE_AUTHORITYSERIALIZER_HXX
# define LUNE_AUTHORITYSERIALIZER_HXX

# include <cassert>
# include <iostream>

# include <elle/serialize/Pointer.hh>
# include <elle/cryptography/Cipher.hh>

# include <lune/Authority.hh>

ELLE_SERIALIZE_SIMPLE(lune::Authority,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.type;
  archive & elle::serialize::alive_pointer(value.cipher);
  std::cout << (Archive::mode == ArchiveMode::Input ? "Loading" : "Saving")
            << " authority type = " << value.type << " cipher = " << value.cipher << std::endl;
}

#endif
