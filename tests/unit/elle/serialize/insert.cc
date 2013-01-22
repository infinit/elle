#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>

#include <lune/Descriptor.hh>

int main()
{
  using elle::serialize::from_string;
  using elle::serialize::to_string;
  using elle::serialize::InputBase64Archive;
  using elle::serialize::OutputBase64Archive;

  // This string has been generated as follows:
  //
  //   elle::io::Unique unique;
  //   if (descriptor.Save(unique) == elle::Status::Error)
  //     escape("XXX");
  //   std::cout << unique << std::endl;
  std::string descriptor_str("AAAFAAAAbG9jYWwAAAAAgAC+qKTe1Ib9HXaGWlEu03vIH/dgIHafJtYZwS4nVjH0uzX2ov+AaVoIYaTtwQ+WKRbzUaEUqEvUYOlw0xRjytXcvuEjGvJZqjPjJJQbK4xMQgaeWsPGc9pAw15PeyBhNJURij5jFuMkFjIRrZVnfRTwr3D0kmBA4DwACy+QUqoz1wAAAwABAAEAAAEAAAACAAAAAAAFAAAAbG9jYWwDAAEAAAAAACAAAAAAAAAAz7SW68DZiIChmRkO5q035WhXuNYwvGxIAtgamz9eU6IAAAIAAAAAAAUAAABsb2NhbAMABgAAAAAAIAAAAAAAAAAdNugLAMidIDjXbAsGr01ufXGI2OQp/x+8N0UYhXN/qQAAIAAAAAAAAAACAAAAAAAAXB9h3cUoDKCZFJQqqlSz9l2NKLMrRUc43FU3slmLESDOFZzMg2TJ4aJmZFIAuZZOwMRC7zi4UebSmvq1wF62e5w7zYDx13ttWiEPjgggfdTFnPWKjkBy+wP6bXH+qu/8ovFVAw6y0aXb4VwIhZuTl+05WfT+KNslZdusbRHqvds962RmsTjnLzOXKkFoNJf4a3wjq8TfUU2JkGJeXtQTuqYKZ4MlXSLDKxy9jJIQO/FIsYOM16a3MVVZ/UmD6DG11iomIosPAXvuIEuX0t2/16cpZ4QsL0RzgVXKoPau2HsUOtPliyKCcoLzIgTL/Jyhur5kHNqhR2HOX99wPa9u5WWJT/wsZlSbNqLPyh+YFOoLUDX4pdOhBm+4/xqcAzzc8ZmD4/64navPQ0+j98c2Ca8mdOaKTjqqAdve8u3LkI0SGZ3ghXBum6NVIzvo9D7l1zaiUygumhfZbbmE4JA2Fn15GJrmhkxnGlIK4S7Pl5n5W8tO/2S4jSMlIlq1q5EvzlE9TSqXKXnG10+dw94IPOD2u+sUqC9lVhZnEOnKHAe6/xnS2pxFVV3VZI9xKEU1l4IpqTlusKXOfgcdfKZpPdxPhIgN+HbYfTtJjS1FbJBB0LAWg9DFk+GhBx2sQPLZ3iwCXRY2qhJqVAUjrJ/mxG7h+RagY3w+6XKlL7FAVIsFAAAAbG9jYWwDAAEAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAqXOVscW+qosSevFvYwd3ppGG9QkEq/kWs5ZI6sWpyBgu2/HIqynRGADr0bqb58BIr3QMHbSnnptsfU0lFpul9OA7Yga3X1tsBAOwCEB+Llikmh32KaPa2jUJ8LlQb3CpXprRuGUYJMJbUld41JYzDm7YcLmenQOBZV+2zOqmW0Y=");


  lune::Descriptor descriptor{from_string<InputBase64Archive>(descriptor_str)};

  std::string out;
  to_string<OutputBase64Archive>(out) << descriptor;

  assert(out == descriptor_str);

  std::cout << "tests done." << std::endl;
  return 0;
}


