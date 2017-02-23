#ifndef ELLE_PYTHON_DATETIME_CONVERTER_HH
# define ELLE_PYTHON_DATETIME_CONVERTER_HH

# include <elle/python/converter-plugin.hh>

namespace elle
{
  namespace python
  {
    extern elle::Plugin<ConverterPlugin> datetime_converter;

    class DatetimeConverter:
      public ConverterPlugin
    {
    public:
      DatetimeConverter();
    };
  }
}

#endif
