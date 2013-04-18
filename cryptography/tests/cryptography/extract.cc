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
//       elle::io::Unique unique;
//       cryptography::KeyPair kp =
//         cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 1024);
//       if (kp.Save(unique) == elle::Status::Error)
//         throw elle::Exception("unable XXX");
//       std::cout << unique << std::endl;

      std::string string("AAAAAAAAAAAAAIAAAADRQ7aOm3A+Kf9wR7M1CkRSyeDk3OjCxZomg3d1vXT2xO9Fgyi/N/M6Vg7xBEMY2Xx/4g0D/8FOfjnSbU3yMpVLmY02st+loVy1XWHOGDaBbW6cblhe7S2Dtw0P/7ddf91KHs6G9Jzq/O0twp29+RiexcoV1KkgM+wilRQjSEYfewAAAwAAAAEAAQAAAAAAAAAAgAAAANFDto6bcD4p/3BHszUKRFLJ4OTc6MLFmiaDd3W9dPbE70WDKL838zpWDvEEQxjZfH/iDQP/wU5+OdJtTfIylUuZjTay36WhXLVdYc4YNoFtbpxuWF7tLYO3DQ//t11/3Uoezob0nOr87S3Cnb35GJ7FyhXUqSAz7CKVFCNIRh97AAADAAAAAQABAACAAAAAsW33SJzUo3Cai4nPkaY93fAbhhEkLZi7pfIMBMF5fY3mzgpyLwdybEDxdDtH5Lbqj8eLf5uHPFVcE2XoE/X9jE4GyyF/d8/iwbfd+ef+RDZPVBltQKO8k49/wo3hdTzPUJBp4f+t7MThWGWoBjddmUYPpOwCCH3Ub5/vPVPWcikAAEAAAADzqXqA6/POIal/SbYCKvPmCo3PRdQzgvtWVH5N5p/uJ2aDFIDKSaoEXxmy0qxn7iRoBDL3JO+xRXQVr8jxYQ21AABAAAAA29xZbFfyfh856FUXrR04ThOe5/faWoQGj1tO73sQCL60jGyxXYhuUH0/XjLdtoCYOxPiNnN6OB0sQlqTVTk2bwAAQAAAAIr+rdQnRS/cQf1LhhkceX9Lm/OWKOOtn0Ry9I3ptme7cB3sLz+139eYjGdXCGRNE9WGN7CGEHl2Xi8U1LLlI1UAAEAAAAA03vhiJYQa8Edsp4naB3zSycUc422O9tDdCIgi7uUcNUAVxM2iQHzGvlfOv0yx5pq1GkL79Ske9WT/T+i+RdPhAABAAAAAoQXn4nF0gcuecC4jGsS9dlK1wyp5IQ3QhWjMd82zjchBbhG3UODpyDgjRcNl/ly5zcEzuKzcItkqpX7YE29k4w==");

  auto extractor = from_string<InputBase64Archive>(string);
  cryptography::KeyPair pair{extractor};

  std::cout << "tests done." << std::endl;
  return 0;
}
