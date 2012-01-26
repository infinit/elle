//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [wed jan 28 12:08:47 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Pack.hh"

// this file is included here to avoid cycles.
#include "Generator.hh"

namespace elle
{
  namespace test
  {

//
// ---------- methods ---------------------------------------------------------
//

    Status              Pack::Create(Archive&                   archive,
                                     const Natural32            n,
                                     const Boolean              refer)
    {
      Natural32         i;

      enter();

      for (i = 0; i < n; i++)
        {
          Byte          random = Random::Generate(Archive::TypeBoolean,
                                                  Archive::TypeArchive);

          printf("%u/%u\n", i, n);

          switch ((enum Archive::Type)random)
            {
            case Archive::TypeUnknown:
              {
                escape("unknown element type");
                break;
              }
            case Archive::TypeNull:
              {
                if (archive.Serialize(none) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeNull,
                                    &none,
                                    sizeof (Null)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeBoolean:
              {
                Boolean         value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a boolean");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeBoolean,
                                    &value,
                                    sizeof (Boolean)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeCharacter:
              {
                Character       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a character");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeCharacter,
                                    &value,
                                    sizeof (Character)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeReal:
              {
                Real            value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a real");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeReal,
                                    &value,
                                    sizeof (Real)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeInteger8:
              {
                Integer8        value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a integer 8-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeInteger8,
                                    &value,
                                    sizeof (Integer8)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeInteger16:
              {
                Integer16       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a integer 16-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeInteger16,
                                    &value,
                                    sizeof (Integer16)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeInteger32:
              {
                Integer32       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a integer 32-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeInteger32,
                                    &value,
                                    sizeof (Integer32)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeInteger64:
              {
                Integer64       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a integer 64-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeInteger64,
                                    &value,
                                    sizeof (Integer64)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeNatural8:
              {
                Natural8        value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a natural 8-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeNatural8,
                                    &value,
                                    sizeof (Natural8)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeNatural16:
              {
                Natural16       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a natural 16-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeNatural16,
                                    &value,
                                    sizeof (Natural16)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeNatural32:
              {
                Natural32       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a natural 32-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeNatural32,
                                    &value,
                                    sizeof (Natural32)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeNatural64:
              {
                Natural64       value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a natural 64-bit");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeNatural64,
                                    &value,
                                    sizeof (Natural64)) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeLarge:
              {
                Large           value;
                void*           buffer;
                Natural32       size;

                enterx(slab(buffer, ::free));

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a large");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                size = BN_num_bytes(&value);

                if ((buffer = ::malloc(size)) == NULL)
                  escape("unable to allocate memory");

                ::BN_bn2bin(&value, (unsigned char*)buffer);

                ::BN_clear_free(&value);

                if (refer)
                  if (Referee::Push(Archive::TypeLarge,
                                    buffer,
                                    size) == StatusError)
                    escape("unable to push the element into the referee");

                free(buffer);

                waive(buffer);

                release();

                break;
              }
            case Archive::TypeString:
              {
                String          value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a string");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeString,
                                    value.data(),
                                    value.length()) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeRegion:
              {
                Region          value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a region");

                if (archive.Serialize(value) == StatusError)
                  escape("unable to serialize the element");

                if (refer)
                  if (Referee::Push(Archive::TypeRegion,
                                    value.contents,
                                    value.size) == StatusError)
                    escape("unable to push the element into the referee");

                break;
              }
            case Archive::TypeArchive:
              {
                Archive         value;

                if (Generator::Create(value) == StatusError)
                  escape("unable to create a archive");

                if (value.size != 0)
                  {
                    if (archive.Serialize(value) == StatusError)
                      escape("unable to serialize the element");

                    if (refer)
                      if (Referee::Push(Archive::TypeArchive,
                                        value.contents,
                                        value.size) == StatusError)
                        escape("unable to push the element into the referee");
                  }

                break;
              }
            }
        }

      leave();
    }

    Status              Pack::Verify(Archive&                   archive)
    {
      Archive::Type     fetch;
      Archive::Type     type;
      void*             data;
      Natural32         size;

      enter();

      while (archive.offset != archive.size)
        {
          if (archive.Fetch(fetch) == StatusError)
            escape("unable to fetch the archive's next type");

          if (Referee::Pop(type,
                           data,
                           size) == StatusError)
            escape("unable to pop the element from the referee");

          if (fetch != type)
            escape("the next type is different from the one recorded "
                   "by the referee");

          switch (fetch)
            {
            case Archive::TypeUnknown:
              {
                escape("unknown element type");
                break;
              }
            case Archive::TypeNull:
              {
                if (archive.Extract(none) == StatusError)
                  escape("unable to extract the element");

                break;
              }
            case Archive::TypeBoolean:
              {
                Boolean         value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeCharacter:
              {
                Character       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeReal:
              {
                Real            value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeInteger8:
              {
                Integer8        value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeInteger16:
              {
                Integer16       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeInteger32:
              {
                Integer32       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeInteger64:
              {
                Integer64       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeNatural8:
              {
                Natural8        value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeNatural16:
              {
                Natural16       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeNatural32:
              {
                Natural32       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeNatural64:
              {
                Natural64       value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (::memcmp(&value, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeLarge:
              {
                Large           value;
                Natural32       s;
                void*           buffer;

                enterx(slab(buffer, ::free));

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                s = BN_num_bytes(&value);

                if ((buffer = ::malloc(size)) == NULL)
                  escape("unable to allocate memory");

                ::BN_bn2bin(&value, (unsigned char*)buffer);

                ::BN_clear_free(&value);

                if (s != size)
                  escape("different size between the archive and the referee");

                if (::memcmp(buffer, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                ::free(buffer);

                waive(buffer);

                release();

                break;
              }
            case Archive::TypeString:
              {
                String          value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (value.length() != size)
                  escape("different size between the archive and the referee");

                if (::memcmp(value.data(), data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeRegion:
              {
                Region          value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (value.size != size)
                  escape("different size between the archive and the referee");

                if (::memcmp(value.contents, data, size) != 0)
                  escape("different values between the archive and the "
                         "referee");

                break;
              }
            case Archive::TypeArchive:
              {
                Archive         value;

                if (archive.Extract(value) == StatusError)
                  escape("unable to extract the element");

                if (value.size != size)
                  escape("different size between the archive and the referee");

                if (::memcmp(value.contents, data, size) != 0)
                  escape("different values between the archive and "
                         "the referee");

                break;
              }
            }

          free(data);
        }

      // check that all the referee's value have been checked.
      if (Referee::List.empty() == false)
        escape("some elements remain in the referee's list");

      leave();
    }

  }
}
