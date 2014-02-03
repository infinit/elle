#ifndef ELLE_OSTREAM_HH
# define ELLE_OSTREAM_HH

# include <memory>

# include <elle/attribute.hh>
# include <elle/Buffer.hh>

namespace elle
{
  class OStream
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef OStream Self;
    typedef unsigned int Size;
    class Backend;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    OStream(std::unique_ptr<Backend> backend);
    OStream(OStream&& stream) = default;
    ~OStream() noexcept(false);

  /*--------.
  | Writing |
  `--------*/
  public:
    void
    write(elle::Byte b);
    void
    write(elle::ConstWeakBuffer const& data);
    void
    flush();

  /*--------.
  | Backend |
  `--------*/
  public:
    class Backend
    {
    public:
      typedef OStream::Size Size;
      virtual
      void
      write(elle::ConstWeakBuffer const& data) = 0;
      virtual
      Size
      size() = 0;
      virtual
      Size
      size(Size size);
    };
  private:
    ELLE_ATTRIBUTE(std::unique_ptr<Backend>, backend);
    ELLE_ATTRIBUTE(elle::Buffer, buffer);
  };
}

#endif
