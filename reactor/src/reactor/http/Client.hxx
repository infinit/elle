namespace reactor
{
  namespace http
  {
    template <typename ... Args>
    Request
    Client::request(Args&& ... args)
    {
      Request res(std::forward<Args>(args)...);
      this->_register(res);
      return res;
    }
  }
}
