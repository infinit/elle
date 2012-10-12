#ifndef  ELLE_SERIALIZE_EXTRACT_HXX
# define ELLE_SERIALIZE_EXTRACT_HXX

namespace elle
{
  namespace serialize
  {

    /// @brief Input archive that owns a stream instance.
    template <typename Archive>
    class Extractor:
      public Archive
    {
    private:
      std::unique_ptr<std::istream> _stream;

    public:
      explicit
      Extractor(std::unique_ptr<std::istream>&& stream):
        Archive(*(_stream = std::move(stream)))
      {}

      Extractor(Extractor&& other):
        Extractor(std::move(other._stream))
      {}

      Extractor(Extractor const&) = delete;
      Extractor& operator =(Extractor const&) = delete;
      Extractor& operator =(Extractor&&) = delete;
    };

    namespace detail
    {
      // Avoid including fstream and sstream

      std::unique_ptr<std::istream>
      new_ifstream(std::string const& path);

      std::unique_ptr<std::istream>
      new_istringstream(std::string const& str);
    }

    template <typename Archive = OutputBinaryArchive>
    Extractor<Archive>
    from_file(std::string const& path)
    {
      return Extractor<Archive>(std::move(detail::new_ifstream(path)));
    }

    template <typename Archive = OutputBinaryArchive>
    Extractor<Archive>
    from_string(std::string const& str)
    {
      return Extractor<Archive>(std::move(detail::new_istringstream(str)));
    }

  }
}

#endif
