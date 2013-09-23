#ifndef CURLY_SERVICE_HXX
# define CURLY_SERVICE_HXX

#include <curly/curly_service.hh> // never included useful for static analysis

namespace curly
{
  template <typename T>
  void
  curl_service::option(CURLMoption opt, T&& param)
  {
    auto mc = curl_multi_setopt(this->_multi.get(),
                                opt,
                                std::forward<T>(param));
    throw_if_mcode(mc);
  }
} /* curly */

#endif /* end of include guard: CURLY_SERVICE_HXX */
