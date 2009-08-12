namespace hole
{
  inline Node &
  FingerTable::Predecessor()
  {
    return *predecessor_;
  }

  inline Node &
  FingerTable::Successor()
  {
    return (*this)[1];
  }

  inline Node &
  FingerTable::operator[](int index)
  {
    return *successors_[index];
  }
}
