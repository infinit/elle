#include <elle/print.hh>

namespace elle
{
  namespace iomanip
  {

    Separator const       nosep{'\0'};
    Separator const       spacesep{' '};
    Separator const       tabsep{'\t'};
    Separator const       newlinesep{'\n'};

    EndOfLine const       newline{'\n'};
    EndOfLine const       nonewline{'\0'};
  }

  namespace detail
  {

    bool
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::Printable const&             value)
    {
      if (!is_first)
          out << flags.sep;
      value.print(out);
      return false;
    }

    bool
    fprint_value(std::ostream&                      /*out*/,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::Separator const&    value)
    {
      flags.sep = value.sep;
      return is_first;
    }

    bool
    fprint_value(std::ostream&                      /*out*/,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::iomanip::EndOfLine const&    value)
    {
      flags.endl = value.endl;
      return is_first;
    }

    void
    fprint(std::ostream&                            out,
           PrintFlags&                              flags,
           bool                                     /*is_first*/)
    {
      if (flags.endl.size())
          out << flags.endl;
    }

  }
}
