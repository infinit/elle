#pragma once

#include <elle/Plugin.hh>

namespace elle
{
  namespace reactor
  {
    namespace plugins
    {
      /// Plugin injection to support Thread specific indentation when using
      /// Elle's logging system.
      extern elle::Plugin<elle::log::Indenter> logger_indentation;
      /// Plugin injection to support Thread specific tags when using Elle's
      /// logging system.
      extern elle::Plugin<elle::log::Tag> logger_tags;
    }
  }
}
