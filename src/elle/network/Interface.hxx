namespace elle
{
  namespace network
  {
    inline
    bool
    operator &(Interface::Filter const lhs,
               Interface::Filter const rhs)
    {
      return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

    inline
    Interface::Filter
    operator |(Interface::Filter const lhs,
               Interface::Filter const rhs)
    {
      return static_cast<Interface::Filter>(
          static_cast<int>(lhs) | static_cast<int>(rhs)
      );
    }
  }
}
