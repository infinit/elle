#include <boost/filesystem/path.hpp>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

#include <elle/os/locale.hh>
#include <elle/log.hh>

ELLE_LOG_COMPONENT("elle.os.locale");

namespace elle
{
  namespace os
  {
    namespace locale
    {
      Imbuer::Imbuer()
        : _previous_locale()
      {}

      Imbuer::Imbuer(std::locale const& new_locale)
        : _previous_locale(boost::filesystem::path::imbue(new_locale))
      {}

      Imbuer::~Imbuer()
      {
        if (this->_previous_locale)
        {
          ELLE_TRACE_SCOPE("%s: restore previous locale", *this);
          boost::filesystem::path::imbue(this->_previous_locale.get());
        }
      }

      DefaultImbuer::DefaultImbuer()
        : Imbuer()
      {
        ELLE_TRACE_SCOPE("%s: construction", *this);
      }
    }
  }
}
