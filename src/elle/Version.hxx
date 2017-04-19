namespace elle
{
  constexpr
  Version::Version(uint8_t major,
                   uint8_t minor,
                   uint8_t subminor)
    : _major(major)
    , _minor(minor)
    , _subminor(subminor)
  {}
}
