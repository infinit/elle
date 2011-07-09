//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Hexadecimal.cc
//
// created       julien quintard   [fri apr 16 19:25:45 2010]
// updated       julien quintard   [fri jul  8 17:58:56 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Hexadecimal.hh>

#include <elle/core/Character.hh>
#include <elle/core/Byte.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this string defines the set of alphanumeric characters composing
    /// a hexadecimal string.
    ///
    const String		Hexadecimal::Alphabet =
      "0123456789abcdef";

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns true if the given string is valid.
    ///
    Status		Hexadecimal::Validate(const String&	string)
    {
      Natural32		i;

      enter();

      if ((string.length() % 2) != 0)
	false();

      // pass every character.
      for (i = 0; i < string.length(); i++)
	{
	  // verify the validity.
	  if (::isalpha(string[i]) != 0)
	    if (::islower(string[i]) == 0)
	    false();

	  if (::isxdigit(string[i]) == 0)
	    false();
	}

      true();
    }

    ///
    /// this method encodes the given region.
    ///
    Status		Hexadecimal::Encode(const Region&	region,
					    String&		string)
    {
      Natural32		i;

      enter();

      for (i = 0; i < region.size; i++)
	{
	  Character	hexadecimal[2];

	  hexadecimal[0] =
	    Hexadecimal::Alphabet[(region.contents[i] >> 4) & 0xf];
	  hexadecimal[1] =
	    Hexadecimal::Alphabet[region.contents[i] & 0xf];

	  string.append(hexadecimal, 2);
	}

      leave();
    }

    ///
    /// this method decodes a hexadecimal string into a region.
    ///
    Status		Hexadecimal::Decode(const String&	string,
					    Region&		region)
    {
      Natural32		i;

      enter();

      if (Hexadecimal::Validate(string) != StatusTrue)
	escape("the argument is not a valid hexadecimal string");

      if (region.Prepare(string.length() / 2) == StatusError)
	escape("unable to prepare the region");

      for (i = 0; i < (string.length() / 2); i++)
	{
	  Natural32	hexadecimal[2];

	  hexadecimal[0] =
	    Hexadecimal::Alphabet.find(string[i * 2]);
	  hexadecimal[1] =
	    Hexadecimal::Alphabet.find(string[(i * 2) + 1]);

	  region.contents[i] =
	    ((hexadecimal[0] << 4) & 0xf0) | ((hexadecimal[1] & 0xf));
	}

      region.size = i;

      leave();
    }

  }
}
