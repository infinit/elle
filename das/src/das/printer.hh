#ifndef DAS_PRINTER_HH
# define DAS_PRINTER_HH

namespace das
{
  template <typename Field>
  struct PrintMember
  {
    template <typename T>
    static
    void
    apply(T const& o, std::ostream& s, bool& first)
    {
      if (first)
        first = false;
      else
        s << ", ";
      s << Field::name << " = " << Field::get(o);
    }
  };
}

template <typename T>
typename std::enable_if_exists<typename das::Das<T>::Model, std::ostream>::type&
operator << (std::ostream& s, T const& e)
{
  bool first = true;
  s << elle::demangle(typeid(T).name()) << "(";
  das::Das<T>::Model::template each_field<das::PrintMember>(e, s, first);
  s << ")";
  return s;
}

#endif
