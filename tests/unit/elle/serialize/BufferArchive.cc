

#include <elle/utility/Buffer.hh>
#include <elle/serialize/BufferArchive.hh>


int main()
{
  elle::utility::Buffer buffer;

  buffer.Writer() << int32_t{12}
                  << uint64_t{42}
                  << std::string{"GIGADEPTH IS BIGGER"}
                  ;


  int32_t int32_t_val;
  uint64_t uint64_t_val;
  std::string string_val;
  buffer.Reader() >> int32_t_val
                  >> uint64_t_val
                  >> string_val
                  ;

  assert(int32_t_val == 12);
  assert(uint64_t_val == 42);
  assert(string_val == "GIGADEPTH IS BIGGER");

  std::cout << "tests done.\n";
  return 0;
}
