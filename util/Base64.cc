//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/util/Base64.cc
//
// created       julien quintard   [fri apr 16 19:25:45 2010]
// updated       julien quintard   [thu apr 22 14:15:20 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/util/Base64.hh>

namespace elle
{
  namespace util
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this string defines the set of alphanumeric characters composing
    /// a base64 string.
    ///
    const String		Base64::Alphabet =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz"
      "0123456789+-";

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method returns true if the given string is valid.
    ///
    Status		Base64::Validate(const String&		string)
    {
      Natural32		i;

      enter();

      // pass every character.
      for (i = 0; i < string.length(); i++)
	{
	  // verify the validity.
	  if ((::isalnum(string[i]) == 0) &&
	      (string[i] != '+') &&
	      (string[i] != '-') &&
	      (string[i] != '='))
	    false();
	}

      true();
    }

    ///
    /// this method encodes the given region.
    ///
    Status		Base64::Encode(const Region&		region,
				       String&			string)
    {
      Character		array3[3];
      Character		array4[4];
      Natural32		i;
      Natural32		j;
      Natural32		k;

      enter();

      for (i = 0, j = 0; i < region.size; i++)
	{
	  array3[j++] = (Character)region.contents[i];

	  if (j == 3)
	    {
	      array4[0] = (array3[0] & 0xfc) >> 2;
	      array4[1] = ((array3[0] & 0x03) << 4) +
		((array3[1] & 0xf0) >> 4);
	      array4[2] = ((array3[1] & 0x0f) << 2) +
		((array3[2] & 0xc0) >> 6);
	      array4[3] = array3[2] & 0x3f;

	      for (j = 0; j < 4; j++)
		string += Base64::Alphabet[array4[j]];

	      j = 0;
	    }
	}

      if (j != 0)
	{
	  for (k = j; k < 3; k++)
	    array3[k] = '\0';

	  array4[0] = (array3[0] & 0xfc) >> 2;
	  array4[1] = ((array3[0] & 0x03) << 4) +
	    ((array3[1] & 0xf0) >> 4);
	  array4[2] = ((array3[1] & 0x0f) << 2) +
	    ((array3[2] & 0xc0) >> 6);
	  array4[3] = array3[2] & 0x3f;

	  for (k = 0; k < (j + 1); k++)
	    string += Base64::Alphabet[array4[k]];

	  for (; j < 3; j++)
	    string += '=';
	}

      leave();
    }

    ///
    /// this method decodes a base64 string into a region.
    ///
    Status		Base64::Decode(const String&		string,
				       Region&			region)
    {
      Character		array3[3];
      Character		array4[4];
      Natural32		i;
      Natural32		j;
      Natural32		k;

      enter();

      if (Base64::Validate(string) != StatusTrue)
	escape("the argument is not a valid base64 string");

      if (region.Prepare(string.length() * 3 / 4) == StatusError)
	escape("unable to prepare the region");

      for (i = 0, j = 0; (i < string.length()) && (string[i] != '='); i++)
	{
	  array4[j++] = string[i];

	  if (j == 4)
	    {
	      for (j = 0; j < 4; j++)
		array4[j] = Base64::Alphabet.find(array4[j]);

	      array3[0] = (array4[0] << 2) +
		((array4[1] & 0x30) >> 4);
	      array3[1] = ((array4[1] & 0xf) << 4) +
		((array4[2] & 0x3c) >> 2);
	      array3[2] = ((array4[2] & 0x3) << 6) +
		array4[3];

	      for (j = 0; j < 3; j++)
		region.contents[region.size++] = (Byte)array3[j];

	      j = 0;
	    }
	}

      if (j != 0)
	{
	  for (k = j; k < 4; k++)
	    array4[k] = '\0';

	  for (k = 0; k < 4; k++)
	    array4[k] = Base64::Alphabet.find(array4[k]);

	  array3[0] = (array4[0] << 2) +
	    ((array4[1] & 0x30) >> 4);
	  array3[1] = ((array4[1] & 0xf) << 4) +
	    ((array4[2] & 0x3c) >> 2);
	  array3[2] = ((array4[2] & 0x3) << 6) + array4[3];

	  for (k = 0; k < (j - 1); k++)
	    region.contents[region.size++] = (Byte)array3[k];
	}

      leave();
    }

  }
}
