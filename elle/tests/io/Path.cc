#include <cassert>
#include <iostream>

#include <elle/io/Path.hh>
#include <elle/io/Piece.hh>
#include <elle/io/Pattern.hh>

int main()
{
  elle::io::Pattern pattern("chiche/mange/mon/%POSITION%");
  elle::io::Path path(pattern,
                      elle::io::Piece("%POSITION%", "cul"));

  assert(path.string() == "chiche/mange/mon/cul");

  std::cout << "tests done.\n";

  return 0;
}
