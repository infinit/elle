//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/test/archive/Generator.cc
//
// created       julien quintard   [fri jan 30 03:09:18 2009]
// updated       julien quintard   [sun aug 23 21:14:56 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Generator.hh"

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace test
  {

//
// ---------- definitions -----------------------------------------------------
//

    const Integer64	Generator::MinimumBooleanSize = Type<Boolean>::Minimum;
    const Integer64	Generator::MaximumBooleanSize = Type<Boolean>::Maximum;

    const Integer64	Generator::MinimumCharacterSize = '!';
    const Integer64	Generator::MaximumCharacterSize = '~';

    const Integer64	Generator::MinimumRealSize = -65535;
    const Integer64	Generator::MaximumRealSize = 65536;

    const Integer64	Generator::MinimumInteger8Size = 33;
    const Integer64	Generator::MaximumInteger8Size = 126;

    const Integer64	Generator::MinimumInteger16Size = Type<Integer16>::Minimum;
    const Integer64	Generator::MaximumInteger16Size = Type<Integer16>::Maximum;

    const Integer64	Generator::MinimumInteger32Size = Type<Integer32>::Minimum;
    const Integer64	Generator::MaximumInteger32Size = Type<Integer32>::Maximum;

    const Integer64	Generator::MinimumInteger64Size = Type<Integer64>::Minimum;
    const Integer64	Generator::MaximumInteger64Size = Type<Integer64>::Maximum;

    const Integer64	Generator::MinimumNatural8Size = 33;
    const Integer64	Generator::MaximumNatural8Size = 126;

    const Integer64	Generator::MinimumNatural16Size = Type<Natural16>::Minimum;
    const Integer64	Generator::MaximumNatural16Size = Type<Natural16>::Maximum;

    const Integer64	Generator::MinimumNatural32Size = Type<Natural32>::Minimum;
    const Integer64	Generator::MaximumNatural32Size = Type<Natural32>::Maximum;

    const Integer64	Generator::MinimumNatural64Size = Type<Integer64>::Minimum;
    const Integer64	Generator::MaximumNatural64Size = Type<Integer64>::Maximum;

    const Integer64	Generator::MinimumLargeSize = 128;
    const Integer64	Generator::MaximumLargeSize = 2048;

    const Integer64	Generator::MinimumStringLength = 1;
    const Integer64	Generator::MaximumStringLength = 1024;

    const Integer64	Generator::MinimumRegionSize = 1;
    const Integer64	Generator::MaximumRegionSize = 4096;

    const Integer64	Generator::MinimumArchiveChoice = 0;
    const Integer64	Generator::MaximumArchiveChoice = 256;

    const Integer64	Generator::MinimumArchiveSize = 1;
    const Integer64	Generator::MaximumArchiveSize = 128;

//
// ---------- methods ---------------------------------------------------------
//

    Status		Generator::Create(Boolean&		value)
    {
      value = Random::Generate(Generator::MinimumBooleanSize,
			       Generator::MaximumBooleanSize);

      leave();
    }

    Status		Generator::Create(Character&		value)
    {
      value = Random::Generate(Generator::MinimumCharacterSize,
			       Generator::MaximumCharacterSize);

      leave();
    }

    Status		Generator::Create(Real&			value)
    {
      value =
	((Real)Random::Generate(Generator::MinimumRealSize,
				Generator::MaximumRealSize)) +
	((Real)(1 / (Real)Random::Generate(Generator::MinimumRealSize,
					   Generator::MaximumRealSize)));

      leave();
    }

    Status		Generator::Create(Integer8&		value)
    {
      value = Random::Generate(Generator::MinimumInteger8Size,
			       Generator::MaximumInteger8Size);

      leave();
    }

    Status		Generator::Create(Integer16&		value)
    {
      value = Random::Generate(Generator::MinimumInteger16Size,
			       Generator::MaximumInteger16Size);

      leave();
    }

    Status		Generator::Create(Integer32&		value)
    {
      value = Random::Generate(Generator::MinimumInteger32Size,
			       Generator::MaximumInteger32Size);

      leave();
    }

    Status		Generator::Create(Integer64&		value)
    {
      value = Random::Generate(Generator::MinimumInteger64Size,
			       Generator::MaximumInteger64Size);

      leave();
    }

    Status		Generator::Create(Natural8&		value)
    {
      value = Random::Generate(Generator::MinimumNatural8Size,
			       Generator::MaximumNatural8Size);

      leave();
    }

    Status		Generator::Create(Natural16&		value)
    {
      value = Random::Generate(Generator::MinimumNatural16Size,
			       Generator::MaximumNatural16Size);

      leave();
    }

    Status		Generator::Create(Natural32&		value)
    {
      value = Random::Generate(Generator::MinimumNatural32Size,
			       Generator::MaximumNatural32Size);

      leave();
    }

    Status		Generator::Create(Natural64&		value)
    {
      value = Random::Generate(Generator::MinimumNatural64Size,
			       Generator::MaximumNatural64Size);

      leave();
    }

    Status		Generator::Create(Large&		value)
    {
      ::BN_init(&value);
      ::BN_rand(&value, Random::Generate(Generator::MinimumLargeSize,
					 Generator::MaximumLargeSize), 0, 0);

      leave();
    }

    Status		Generator::Create(String&		value)
    {
      Natural32		length;
      Natural32		i;

      length = Random::Generate(Generator::MinimumStringLength,
				Generator::MaximumStringLength);

      for (i = 0; i < length; i++)
	value.append(1,
		     (char)Random::Generate(Generator::MinimumCharacterSize,
					    Generator::MaximumCharacterSize));

      leave();
    }

    Status		Generator::Create(Region&		value)
    {
      Natural32		size = Random::Generate(Generator::MinimumRegionSize,
						Generator::MaximumRegionSize);
      Natural32		i;

      if (value.Prepare(size) == NULL)
	escape("unable to prepare the region");

      value.size = size;

      for (i = 0; i < size; i++)
	value.contents[i] = Random::Generate(Generator::MinimumCharacterSize,
					     Generator::MaximumCharacterSize);

      leave();
    }

    Status		Generator::Create(Archive&		value)
    {
      Integer64		choice = Random::Generate(Generator::MinimumArchiveChoice,
						  Generator::MaximumArchiveChoice);

      if (choice == 0)
	{
	  if (value.Create() == StatusError)
	    escape("unable to create the archive");

	  if (Pack::Create(value,
			   Random::Generate(Generator::MinimumArchiveSize,
					    Generator::MaximumArchiveSize),
			   false) == StatusError)
	    escape("unable to create a pack");
	}

      leave();
    }

  }
}
