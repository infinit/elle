//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Settings.hxx
//
// created       julien quintard   [sun jun 26 20:57:08 2011]
// updated       julien quintard   [sun jun 26 21:12:13 2011]
//

#ifndef ELLE_UTILITY_SETTINGS_HXX
#define ELLE_UTILITY_SETTINGS_HXX

namespace elle
{
  namespace utility
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const T&			value)
    {
      String		string;

      enter();

      // transform the value into a string-based format.
      if (Variable::Convert(value, string) == StatusError)
	escape("unable to convert the value");

      // write the value.
      if (this->Write(identifier, name, string) == StatusError)
	escape("unable to write the value");

      leave();
    }

    ///
    /// XXX
    ///
    template <typename T>
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      T&			value,
				      const T			D)
    {
      String			string;

      enter();

      // set the value with the default.
      value = D;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the value");

      // convert the string into the type.
      if (Variable::Convert(string, value) == StatusError)
	escape("unable to convert the value");

      leave();
    }

  }
}

#endif
