#ifndef  ELLE_PRINT_HXX
# define ELLE_PRINT_HXX

# include <iostream>
# include <sstream>

# include <elle/print.hh>

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

    // generic value sprint
    template<typename T>
    bool
    sprint_value(std::ostream&                      out,
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
    sprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::Separator const&    value);

    // specialization to treat end of line classes
    bool
    sprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::EndOfLine const&    value);

    // sprint recursion ends here
    void
    sprint(std::ostream&                            out,
           PrintFlags&                              flags,
           bool                                     is_first);

    // Generic sprint
    template <typename T, typename... U>
    void
    sprint(std::ostream&                            out,
           PrintFlags&                              flags,
           bool                                     is_first,
           T const&                                 value,
           U const&...                              values)
    {

      is_first = sprint_value(out, flags, is_first, value);
      sprint(out, flags, is_first, values...);
    }

  } // !elle::detail

  template<typename... T>
  void
  print(T const&...         values)
  {
    return sprint(std::cout, values...);
  }

  template<typename... T>
  void
  sprint(std::ostream&      out,
         T const&...        values)
  {
    detail::PrintFlags flags;
    return detail::sprint(out, flags, true, values...);
  }

  template<typename... T>
  std::string
  stringify(T const&...     values)
  {
    std::ostringstream ss;
    sprint(ss, iomanip::nonewline, values...);
    return ss.str();
  }
}

#endif

