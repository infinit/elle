#ifndef  ELLE_SERIALIZE_CLASSVERSION_HH
# define ELLE_SERIALIZE_CLASSVERSION_HH

namespace elle
{
  namespace serialize
  {

    ///
    /// Strong typed unsigned short representing the class version for
    /// Save()/Load() overloads. (it gives you a chance to catch the call)
    ///
    struct Format
    {
    public:
      uint16_t version;

    public:
      Format(uint16_t version):
        version(version)
      {}

      Format(Format const& other):
        version(other.version)
      {}

      Format&
      operator =(Format const& other)
      {
        version = other.version;
        return *this;
      }
    };

  }
}

#endif
