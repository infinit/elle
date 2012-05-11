#ifndef  ELLE_PRINT_HH
# define ELLE_PRINT_HH

# include <iosfwd>

namespace elle
{

  namespace iomanip
  {

    struct Separator
    {
      char sep;
      Separator(char c) : sep(c) {}
    };

    extern Separator const      nosep;
    extern Separator const      spacesep;
    extern Separator const      tabsep;
    extern Separator const      newlinesep;

    struct EndOfLine
    {
      char endl;
      EndOfLine(char c) : endl(c) {}
    };

    extern EndOfLine const      newline;
    extern EndOfLine const      nonewline;

  }

  template<typename... T>
    void sprint(std::ostream& out, T const&... values);

  template<typename... T>
    void print(T const&... values);

  template<typename... T>
    std::string stringify(T const&... values);
}

# include <elle/print.hxx>

#endif
