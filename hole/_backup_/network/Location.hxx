namespace hole
{
  namespace network
  {
    inline void
    Address::ToQHostAddress(QHostAddress * address)
    {
      switch (type)
      {
      case IPv4:
        address->setAddress(addr.ipv4);
        break;
      case IPv6:
        address->setAddress(addr.ipv6);
        break;
      }
    }
  }
}
