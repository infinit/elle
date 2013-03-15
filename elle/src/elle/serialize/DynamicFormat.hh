#ifndef  ELLE_SERIALIZE_DYNAMICFORMAT_HH
# define ELLE_SERIALIZE_DYNAMICFORMAT_HH

# include "Format.hh"
# include "StaticFormat.hh"

namespace elle
{
  namespace serialize
  {

    /// @brief Enable a class versionning per instance.
    ///
    /// This class uses the Strangely Recurring Template Pattern to ensure that
    /// we have one version per class even in inheritance context. You may
    /// inherit from this class privately since you never want to access it
    /// from subclasses.
    template <typename T>
    class DynamicFormat
    {
    private:
      uint16_t  _version;

    public:
      DynamicFormat():
        _version(StaticFormat<T>::version)
      {}

      uint16_t
      version() const
      {
        return _version;
      }

      void
      version(uint16_t const version)
      {
        _version = version;
      }
    };

  }
}

#endif
