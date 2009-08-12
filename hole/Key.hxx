namespace hole
{
  inline bool
  Key::operator<(const Key & other) const
  {
    return Cmp(other) == -1;
  }

  inline bool
  Key::operator==(const Key & other) const
  {
    return Cmp(other) == 0;
  }
}
