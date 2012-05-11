#ifndef  ELLE_PRINT_HH
# define ELLE_PRINT_HH

# include <iosfwd>

///
/// This module defines straightforward functions devoted to print or convert
/// to string any values. In addition, some manipulators are defined in the
/// `iomanip' namespace, when the default behavior does not fit.
///

namespace elle
{

  ///
  /// Prints space separated values line into the provided out stream. All
  /// values are converted to string following this rules:
  ///   - If the value support the << operator, uses it
  ///   - If a elle::io::Stringify specialization exists, uses it
  ///   - Use the default behavior of Stringify class
  ///
  template<typename... T>
    void sprint(std::ostream& out, T const&... values);

  /// Same as `sprint' with standard output stream.
  template<typename... T>
    void print(T const&... values);

  /// Same as `sprint' but returns a string instead of printing it, and do not
  /// terminate the line with a line feed (this can be overriden with
  /// elle::iomanip::newline manipulator).
  template<typename... T>
    std::string stringify(T const&... values);

  /// manipulators for print functions are defined here
  namespace iomanip
  {

    /// You can define your own separator with this class, but common
    /// separators are defined as constants.
    struct Separator
    {
      char sep;
      Separator(char c) : sep(c) {}
    };

    /// no separator
    extern Separator const      nosep;

    /// space separator (default)
    extern Separator const      spacesep;

    /// tabulation separator
    extern Separator const      tabsep;

    /// linefeed separator
    extern Separator const      newlinesep;

    /// You can define your own end of line character with this class.
    struct EndOfLine
    {
      char endl;
      EndOfLine(char c) : endl(c) {}
    };

    /// line feed end of line character
    extern EndOfLine const      newline;

    /// no new line
    extern EndOfLine const      nonewline;

  }
}

# include <elle/print.hxx>

#endif
