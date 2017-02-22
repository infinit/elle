#pragma once

#include <elle/Plugin.hh>

namespace elle
{
  namespace reactor
  {
    namespace plugins
    {
      extern elle::Plugin<elle::log::Indenter> logger_indentation;
      extern elle::Plugin<elle::log::Tag> logger_tags;
    }
  }
}
