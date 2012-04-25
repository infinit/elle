

#include <elle/utility/Buffer.hh>
#include <elle/serialize/BufferArchive.hh>


int main()
{
  elle::utility::Buffer buffer;

  size_t const size_t_val = 12;
  std::string const string_val = "GIGADEPTH IS BIGGER";

  buffer.Writer() << string_val << size_t_val;


  size_t size_t_val_;
  std::string string_val_;
  buffer.Reader() >> string_val_ >> size_t_val_;

  assert(size_t_val == size_t_val_);
  assert(string_val == string_val_);

  std::cout << "tests done.\n";
  return 0;
}
