#ifndef NUCLEUS_PROTON_HANDLE_HXX
# define NUCLEUS_PROTON_HANDLE_HXX

//
// ---------- Handle ----------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Pointer.hh>

# include <cryptography/SecretKey.hh>

# include <nucleus/proton/Egg.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Handle);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Handle,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  switch (value._state)
    {
    case nucleus::proton::Handle::State::unnested:
      {
        ELLE_ASSERT(value._identity != nullptr);

        // XXX archive << elle::serialize::alive_pointer(value._identity);
        ELLE_ASSERT(false);

        break;
      }
    case nucleus::proton::Handle::State::nested:
      {
        ELLE_ASSERT(value._egg != nullptr);

        // In this case, extract the address and secret from the egg and
        // serialize then.
        archive << value._egg->address();
        archive << value._egg->secret();

        break;
      }
    default:
      throw Exception("unknown state '%s'", value._state);
    }
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Handle,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  ELLE_ASSERT(value._state == nucleus::proton::Handle::State::unnested);
  ELLE_ASSERT(value._identity == nullptr);

  ELLE_ASSERT(false);
  (void)archive;
  // XXX archive >> elle::serialize::alive_pointer(value._identity);
}

//
// ---------- Identity --------------------------------------------------------
//

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Handle::Identity,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._address;
  archive & value._secret;
}

#endif
