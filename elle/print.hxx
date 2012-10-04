#ifndef  ELLE_PRINT_HXX
# define ELLE_PRINT_HXX

# include <elle/Printable.hh>

# include <elle/idiom/Close.hh>
#  include <iostream>
#  include <sstream>
# include <elle/idiom/Open.hh>

namespace elle
{

  namespace detail
  {

    template<typename _OStream, typename _T> struct IsPrintable
    {
    private:
      template<typename K>
        struct Clean
        {
          typedef typename std::remove_cv<
            typename std::remove_reference<K>::type
          >::type type;
        };
      typedef typename Clean<_OStream>::type  OStream;
      typedef typename Clean<_T>::type        T;
      typedef char No;
      typedef struct { No _[2];} Yes;
      static No f(...);
      template<size_t> struct Helper {};
      template<typename U>
        static Yes f(U*,  Helper<sizeof(*((OStream*) 0) << *((U*)0))>* sfinae = 0);
    public:
      static bool const value = (sizeof f((T*)0) == sizeof(Yes));
    };

    struct PrintFlags
    {
    public:
      std::string endl;
      std::string sep;

    public:
      PrintFlags()
        : endl("\n")
        , sep(" ")
      {}
    };

    // generic value fprint
    template<typename T>
    typename std::enable_if<IsPrintable<std::ostream, T>::value, bool>::type
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 T const&                           value)
    {
      if (!is_first)
          out << flags.sep;
      out << value;
      return false;
    }

    template<typename T>
    typename std::enable_if<!IsPrintable<std::ostream, T>::value, bool>::type
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 T const&                           value)
    {
      if (!is_first)
          out << flags.sep;
      out << '<' << typeid(T).name() << "instance at "
          << std::hex << static_cast<void const*>(&value) << '>';
      return false;
    }

    // Specialization to treat printable classes
    bool
    fprint_value(std::ostream&                      out,
                 PrintFlags&                        flags,
                 bool                               is_first,
                 elle::Printable const&             value);

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
    static iomanip::EndOfLine nonewline('\0');
    fprint(ss, nonewline, values...);
    return ss.str();
  }
}

#endif

