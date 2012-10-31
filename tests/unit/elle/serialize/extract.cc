#include <elle/serialize/extract.hh>

#include <lune/Descriptor.hh>

int main()
{
  using elle::serialize::from_string;
  using elle::serialize::InputBase64Archive;
  using elle::serialize::OutputBase64Archive;


//   std::string str;

//   std::stringstream ss;
//   {
//     OutputBase64Archive ar{ss};
// //    int i = 4;
// //      ar & 4;
// //    ar & 0.32f;
//     ar & std::string("Bite");

//     // std::cout << ss.str() << std::endl;


//     // std::cout << str << std::endl;
//   }
//   str = ss.str();


//   {
//     auto extractor = from_string<InputBase64Archive>(str);

//     assert(extractor._stream.get() == &extractor.stream());

//    // std::stringstream ss(str);

//     // InputBase64Archive extractor(ss);

//     //  int i;
//     // float f;
//     std::string s;

// //    extractor & i;
//     // extractor & f;
//     extractor & s;

// //    std::cout << "DSLED i:" << i << " f: " << f << " s: " << s << std::endl;
//   }


    std::string descriptor_str("AQAYAAAANTA4YTRlMjllNzc5ODkxYzA0MDAwMDAyAAAAAIAA1fJsjrxeklXMcepsLeqkk8UeAH5p/yv52f7cmSPo+M0vZym5ZSt0wjj+rqWGf6TE9CuCJ4XvNgZVMzv6LaYCqfy8f9JnsevGxQljZEA4CjYMD4M2ravNxIXUf8QYQ/kLQ/ATlUTQWMtzb7Dnpm6fYAupoLKxvZx0N0Rn5VIWhJ0AAAMAAQABAAADAAAAAgAAABgAAAA1MDhhNGUyOWU3Nzk4OTFjMDQwMDAwMDIDAAEAAAAAACAAAAAAAAAA6Go4MnPBZjRvof6hkr0nwQFMj4k3UqYiV+8Z0YsuaMwAAAIAAAAYAAAANTA4YTRlMjllNzc5ODkxYzA0MDAwMDAyAwAGAAAAAAAgAAAAAAAAAD6mRqimBJ7GHm4mWnwvf+QbXyokeWxK+F89/ghvjgV+AAAgAAAAAAAAAAIAAAAAAAB6vC7Wk4FdAyMCX8Ofl9oEU+ye8VhPsoBFmDfFdXW9Pm9oRXoMhY0nqnVanPWTOq+83OSBZWzSMwNoNPdV6s8fyDp9A9Z4FzbfBtxxg//B2Oe/pEQj7vt/MlnUxT3dNQRdupXkWFzpbRp5ukfVyU0Czonxy2chPDAU1WOG1Yl+ibq+LAfUui/yRsK68iwyWpVoBZ7gRVOSQUofoadVMmdKL+ZMhCx//PmWFbyWc43ID84MDltkm0H7Sp0nMBhfewZq9MGtUQpCRebj9pMrAlo9WC180VmrwES1+/a4SAzOBPJ6fKrwmCRx9KZwMvZLflWEEilQOpdJ+lRVbuCQx3YRfStgPfjwpK9A2ysrkHtnYfh7fW6vlh2TB6qerXp2o/igOU7ZztWVGqYEER4py+QYOqT0uYTCvjUNbcDbX+SDm86vv3Uqey7QyDoQF5QpFoZ3Z89GPW/rn6AXxoBd2EbXGw2Xk8S9t1091heOBMhUkeUmJ38rcXyo8ZJ/zgiJSsgFuZEckOrci7hoclcADHAG0jV4ZSSedoGEaxPE+vpzHiNKM0+wn7QIOVMPZrPsjCFQ/tgBWUwVyNSogDhYbSGTENxjPh/M23rOcRPEtjAKunjtopXiztc5VcgvrvpV/Aj3aLN/9iSvZAuEdVxl6hQIb8BihLeXRXlFOgACyM+mtBEAAABGaXN0UGFydHkyMDMxODk5OQMAAQAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgAAAAAAAAAAph/MjpJ1C5b61SXsK8mK83ugrXKnLwHx2DvQ2iJaLxOepY6gCJDs2bqJVB3XDGGBCC57+jlQuLTSGEhc6RONOm6wUropWgVJ+vlVN/CywbU9jF1FKD8L/Zp09hx4CXsSVE1VXAsJSG1c/IclpfB3jU+4gkjudCrVycM1cMytf9g==");

    auto extractor = from_string<InputBase64Archive>(descriptor_str);

    lune::Descriptor descriptor{extractor};



  // std::string result_str_bite;

  // *(extractor._stream) >> result_str_bite;

  // assert(descriptor_str == result_str_bite);

  // lune::Descriptor descriptor{};

  return 0;
}
