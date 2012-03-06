//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun jun 26 19:36:23 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Type.hh>

#include <elle/idiom/Close.hh>
# include <fstream>
# include <climits>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace core
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// these macro-function calls define the general information
    /// related to the basic types.
    ///
    TypeDefine(Null, Nil, Nil, Nil);
    TypeDefine(Boolean, false, true, false);
    TypeDefine(Character, '\0', CHAR_MAX, '\0');
    TypeDefine(Integer8, CHAR_MIN, CHAR_MAX, CHAR_MIN);
    TypeDefine(Integer16, SHRT_MIN, SHRT_MAX, SHRT_MIN);
    TypeDefine(Integer32, INT_MIN, INT_MAX, INT_MIN);
    TypeDefine(Integer64, LLONG_MIN, LLONG_MAX, LLONG_MIN);
    TypeDefine(Natural8, 0, UCHAR_MAX, 0);
    TypeDefine(Natural16, 0, USHRT_MAX, 0);
    TypeDefine(Natural32, 0, UINT_MAX, 0);
    TypeDefine(Natural64, 0, ULLONG_MAX, 0);
    TypeDefine(String, "", "", "");
    TypeDefine(Real, 0.0, 0.0, 0.0);

    ///
    /// large-specific definition.
    ///
    /// note that the minimum, maximum and default attributes
    /// are not defined.
    ///
    const Character*    Type<Large>::Name = "Large";

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method converts a boolean into a string.
    ///
    Status              Variable::Convert(const Boolean&        input,
                                          String&               output)
    {
      // test the input.
      if (input == true)
        output = "true";
      else
        output = "false";

      return StatusTrue;
    }

    ///
    /// this method converts a character into a string.
    ///
    Status              Variable::Convert(const Character&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts an integer into a string.
    ///
    Status              Variable::Convert(const Integer8&       input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts an integer into a string.
    ///
    Status              Variable::Convert(const Integer16&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts an integer into a string.
    ///
    Status              Variable::Convert(const Integer32&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts an integer into a string.
    ///
    Status              Variable::Convert(const Integer64&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a natural into a string.
    ///
    Status              Variable::Convert(const Natural8&       input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a natural into a string.
    ///
    Status              Variable::Convert(const Natural16&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a natural into a string.
    ///
    Status              Variable::Convert(const Natural32&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a natural into a string.
    ///
    Status              Variable::Convert(const Natural64&      input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a real into a string.
    ///
    Status              Variable::Convert(const Real&           input,
                                          String&               output)
    {
      std::stringstream stream;

      // transfer the input into a string.
      stream << input;

      // assign the output.
      output.assign(stream.str());

      return StatusTrue;
    }

    ///
    /// this method converts a string into a boolean.
    ///
    Status              Variable::Convert(const String&         input,
                                          Boolean&              output)
    {
      // test the input.
      if ((input == "1") ||
          (input == "True") || (input == "true") ||
          (input == "Yes") || (input == "yes") ||
          (input == "On") || (input == "on"))
        output = true;
      else if ((input == "0") ||
               (input == "False") || (input == "false") ||
               (input == "No") || (input == "no") ||
               (input == "Off") || (input == "off"))
        output = false;
      else
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a character.
    ///
    Status              Variable::Convert(const String&         input,
                                          Character&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      /// XXX \todo this check fails :( and I don't know why!
      /*
      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;
      */

      return StatusTrue;
    }

    ///
    /// this method converts a string into an integer.
    ///
    Status              Variable::Convert(const String&         input,
                                          Integer8&             output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into an integer.
    ///
    Status              Variable::Convert(const String&         input,
                                          Integer16&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into an integer.
    ///
    Status              Variable::Convert(const String&         input,
                                          Integer32&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into an integer.
    ///
    Status              Variable::Convert(const String&         input,
                                          Integer64&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a natural.
    ///
    Status              Variable::Convert(const String&         input,
                                          Natural8&             output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a natural.
    ///
    Status              Variable::Convert(const String&         input,
                                          Natural16&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a natural.
    ///
    Status              Variable::Convert(const String&         input,
                                          Natural32&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a natural.
    ///
    Status              Variable::Convert(const String&         input,
                                          Natural64&            output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

    ///
    /// this method converts a string into a real.
    ///
    Status              Variable::Convert(const String&         input,
                                          Real&                 output)
    {
      std::istringstream        stream(input);

      // extract the value.
      stream >> output;

      // check the stream.
      if (!stream || !stream.eof())
        return StatusFalse;

      return StatusTrue;
    }

  }
}
