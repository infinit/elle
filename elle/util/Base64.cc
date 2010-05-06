//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/util/Base64.cc
//
// created       julien quintard   [fri apr 16 19:25:45 2010]
// updated       julien quintard   [tue may  4 13:26:25 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/util/Base64.hh>

#include <elle/core/Character.hh>
#include <elle/core/Byte.hh>

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
      Byte		in[3];
      Character		out[4];
      Natural32		i;
      Natural32		j;
      Natural32		k;

      enter();

      // first clear the string.
      string.clear();

      for (i = 0, j = 0; i < region.size; i++)
	{
	  in[j++] = (Character)region.contents[i];

	  if (j == 3)
	    {
	      out[0] = (in[0] & 0xfc) >> 2;
	      out[1] = ((in[0] & 0x03) << 4) |
		((in[1] & 0xf0) >> 4);
	      out[2] = ((in[1] & 0x0f) << 2) |
		((in[2] & 0xc0) >> 6);
	      out[3] = in[2] & 0x3f;

	      for (j = 0; j < 4; j++)
		string += Base64::Alphabet[out[j]];

	      j = 0;
	    }
	}

      if (j != 0)
	{
	  for (k = j; k < 3; k++)
	    in[k] = '\0';

	  out[0] = (in[0] & 0xfc) >> 2;
	  out[1] = ((in[0] & 0x03) << 4) |
	    ((in[1] & 0xf0) >> 4);
	  out[2] = ((in[1] & 0x0f) << 2) |
	    ((in[2] & 0xc0) >> 6);
	  out[3] = in[2] & 0x3f;

	  for (k = 0; k < (j + 1); k++)
	    string += Base64::Alphabet[out[k]];

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
      Byte		out[3];
      Character		in[4];
      Natural32		i;
      Natural32		j;
      Natural32		k;

      enter();

      // first, validate the input string.
      if (Base64::Validate(string) != StatusTrue)
	escape("the argument is not a valid base64 string");

      if (region.Prepare(string.length() * 3 / 4) == StatusError)
	escape("unable to prepare the region");

      for (i = 0, j = 0; (i < string.length()) && (string[i] != '='); i++)
	{
	  in[j++] = string[i];

	  if (j == 4)
	    {
	      for (j = 0; j < 4; j++)
		in[j] = Base64::Alphabet.find(in[j]);

	      out[0] = (in[0] << 2) |
		((in[1] & 0x30) >> 4);
	      out[1] = ((in[1] & 0xf) << 4) |
		((in[2] & 0x3c) >> 2);
	      out[2] = ((in[2] & 0x3) << 6) |
		in[3];

	      for (j = 0; j < 3; j++)
		region.contents[region.size++] = (Byte)out[j];

	      j = 0;
	    }
	}

      if (j != 0)
	{
	  for (k = j; k < 4; k++)
	    in[k] = '\0';

	  for (k = 0; k < 4; k++)
	    in[k] = Base64::Alphabet.find(in[k]);

	  out[0] = (in[0] << 2) |
	    ((in[1] & 0x30) >> 4);
	  out[1] = ((in[1] & 0xf) << 4) |
	    ((in[2] & 0x3c) >> 2);
	  out[2] = ((in[2] & 0x3) << 6) |
	    in[3];

	  for (k = 0; k < (j - 1); k++)
	    region.contents[region.size++] = (Byte)out[k];
	}

      leave();
    }

  }
}
