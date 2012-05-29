

#include <elle/utility/Buffer.hh>
#include <elle/serialize/BufferArchive.hh>


int main()
{
  elle::utility::Buffer buffer;

  buffer.Writer() << size_t{12}
                  << uint64_t{42}
                  << std::string{"GIGADEPTH IS BIGGER"}
                  ;


  size_t size_t_val;
  uint64_t uint64_t_val;
  std::string string_val;
  buffer.Reader() >> size_t_val
                  >> uint64_t_val
                  >> string_val
                  ;

  assert(size_t_val == 12);
  assert(uint64_t_val == 42);
  assert(string_val == "GIGADEPTH IS BIGGER");

  std::cout << "tests done.\n";
  return 0;
}
