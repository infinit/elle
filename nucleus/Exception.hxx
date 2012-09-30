#ifndef NUCLEUS_EXCEPTION_HXX
# define NUCLEUS_EXCEPTION_HXX

namespace nucleus
{

  template <typename... A>
  Exception::Exception(elle::String const& format,
                       A&&... arguments):
    elle::Exception(format, std::forward<A>(arguments)...)
  {
  }

}

#endif
