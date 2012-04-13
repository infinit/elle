#ifndef ELLE_CRYPTOGRAPHY_ONEWAY_HXX
# define ELLE_CRYPTOGRAPHY_ONEWAY_HXX


namespace elle
{
  namespace cryptography
  {

    template <typename T>
    Status
    OneWay::Hash(T const&     value,
                 Digest&      digest)
    {
      throw "not implemented";
      return Status::Ok;
    }

  }
}

#endif
