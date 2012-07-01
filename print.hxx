#ifndef  ELLE_PRINT_HXX
# define ELLE_PRINT_HXX

# include <elle/idiom/Close.hh>
#  include <iostream>
#  include <sstream>
# include <elle/idiom/Open.hh>

namespace elle
{

  namespace detail
  {
    struct PrintFlags
    {
    public:
      char endl;
      char sep;

    public:
      PrintFlags()
        : endl('\n')
        , sep(' ')
      {}
    };

    // generic value fprint
    template<typename T>
    bool
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 T const&                           value)
    {
      if (!is_first && flags.sep != '\0')
          out << flags.sep;
      out << value;
      return false;
    }

    // specialization to treat separator classes
    bool
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::Separator const&    value);

    // specialization to treat end of line classes
    bool
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::EndOfLine const&    value);

    // fprint recursion ends here
    void
    fprint(std::ostream&                            out,
           PrintFlags&                              flags,
           bool                                     is_first);

    // Generic fprint
    template <typename T, typename... U>
    void
    fprint(std::ostream&                            out,
           PrintFlags&                              flags,
           bool                                     is_first,
           T const&                                 value,
           U const&...                              values)
    {

      is_first = fprint_value(out, flags, is_first, value);
      fprint(out, flags, is_first, values...);
    }

  } // !elle::detail

  template<typename... T>
  void
  print(T const&...         values)
  {
    return fprint(std::cout, values...);
  }

  template<typename... T>
  void
  fprint(std::ostream&      out,
         T const&...        values)
  {
    out << std::dec;
    elle::detail::PrintFlags flags;
    return ::elle::detail::fprint(out, flags, true, values...);
  }

  template<typename... T>
  std::string
  sprint(T const&...     values)
  {
    std::ostringstream ss;
    fprint(ss, iomanip::nonewline, values...);
    return ss.str();
  }
}

#endif

