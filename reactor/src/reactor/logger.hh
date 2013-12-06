#ifndef REACTOR_LOGGER_HH
# define REACTOR_LOGGER_HH

# include <elle/Plugin.hh>

namespace reactor
{
  namespace plugins
  {
    extern elle::Plugin<elle::log::Indenter> logger_indentation;
    extern elle::Plugin<elle::log::Tag> logger_tags;
  }
}

#endif
