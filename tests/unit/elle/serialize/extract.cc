#include <elle/serialize/extract.hh>

#include <cryptography/KeyPair.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

int main()
{
  using elle::serialize::from_string;
  using elle::serialize::InputBase64Archive;
  using elle::serialize::OutputBase64Archive;

  // This string has been generated as follows:
  //
  //     elle::io::Unique unique;
  //     cryptography::KeyPair kp = cryptography::KeyPair::generate(1024);
  //     if (kp.Save(unique) == elle::Status::Error)
  //       escape("XXX");
  //     std::cout << unique << std::endl;
  std::string string("AAAAAAAAgADEfggzknOW/UsxKwhRszX+Ie3IWsFKQwQt9S6vJn98fUi13CXGub6OwOAezIhab004FB3tsC+LcPd7xz6ZNxUcGDwhWgmezS0DeopgwFvMEPASrNW2IcPebGKBC6BdX4x+Artx1O8JUBLd7vGfMkxVlS7VWgaYi9fa21klif6/JwAAAwABAAEAAAAAgADEfggzknOW/UsxKwhRszX+Ie3IWsFKQwQt9S6vJn98fUi13CXGub6OwOAezIhab004FB3tsC+LcPd7xz6ZNxUcGDwhWgmezS0DeopgwFvMEPASrNW2IcPebGKBC6BdX4x+Artx1O8JUBLd7vGfMkxVlS7VWgaYi9fa21klif6/JwAAAwABAAEAAIAAvl0wCu7W57BsGa2MOML8C0LJwPMUEqNMuE2yteQBXRVvzSkEvDwV/u2j9JLJg3wTfsIWoMGxvlFqo1PMNNolqsiNMvixi7RYw1g4Mj6vh7lDavdzja9q6jJx/7LYK04StXs1DSrNQe343G95TRQ5FNVyL02X+uCrAePSMONwE4EAAEAA56zqzeRE/tULy0Secx/F8+VwVmhk/yU0g7BL254qAIySaWisJz1lC1eVzxJpm9ZbpIKDBHWl+FyCfxq90NuWQQAAQADZH3IRQBGhdUsGYmmAse2i6NJEDB5PB3i1tBDfWqxFmjl2zsoEd7Y615F0weYfx6/54MMVlhMEvU86ilsjEItnAABAAA08M63PfKGSSjoykVrCVey5DtHg5OQum/FqAOtq/btKTJL5B9y2BS/AsS5DPwVIGbQsQI/gqLqnjkz0miN0aAEAAEAABc0QSCO1FOJEYeRJO1YFql+cFpNYT/Y8JGauOLl8dtObC3CU9y+Xn9TYnwz7MAguzHdT3wTGd07zifXeX352qwAAQAARXLikv1SNErdzah7icGFyf6RhegmU2wJ6bJ8Snx5Fu9Y1upSd68L9oXhb1jW+AbzSPBBvJ1ZmRx8ONW315m1i");

  auto extractor = from_string<InputBase64Archive>(string);

  cryptography::KeyPair pair{extractor};

  std::cout << "tests done." << std::endl;
  return 0;
}
