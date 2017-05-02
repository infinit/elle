namespace elle
{
  namespace network
  {
    bool
    operator &(Interface::Filter lhs, Interface::Filter rhs)
    {
      return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

    Interface::Filter
    operator |(Interface::Filter lhs, Interface::Filter rhs)
    {
      return static_cast<Interface::Filter>(
          static_cast<int>(lhs) | static_cast<int>(rhs)
      );
    }

    Interface::Filter&
    operator |=(Interface::Filter& lhs, Interface::Filter rhs)
    {
      lhs = lhs | rhs;
      return lhs;
    }
  }
}
