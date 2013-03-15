#ifndef INFINIT_CRYPTOGRAPHY_EXCEPTION_HXX
# define INFINIT_CRYPTOGRAPHY_EXCEPTION_HXX

namespace infinit
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename... A>
    Exception::Exception(elle::String const& format,
                         A&&... arguments):
      elle::Exception(format, std::forward<A>(arguments)...)
    {
    }
  }
}

#endif
