#ifndef ELLE_PYTHON_CONVERTER_PLUGIN_HH
# define ELLE_PYTHON_CONVERTER_PLUGIN_HH

# include <elle/Plugin.hh>

namespace elle
{
  namespace python
  {
    class ConverterPlugin
    {
    public:
      virtual
      ~ConverterPlugin() = default;
    };
  }
}

#endif
