#include <elle/AtomicFile.hh>
#include <elle/cryptography/rsa/KeyPair.hh>
#include <elle/das/Symbol.hh>
#include <elle/das/model.hh>
#include <elle/das/printer.hh>
#include <elle/das/serializer.hh>
#include <elle/reactor/http/Request.hh>
#include <elle/reactor/http/exceptions.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>

// Declare symbols used by das.
DAS_SYMBOL(array);
DAS_SYMBOL(dict);
DAS_SYMBOL(foo);
DAS_SYMBOL(f);
DAS_SYMBOL(i);
DAS_SYMBOL(encrypted_payload);
DAS_SYMBOL(body);
DAS_SYMBOL(signature);

// Create an example data that look like that:
// {
//   "body": {
//     "array": [...],
//     "dict": {...},
//     "f": ...,
//     "i": ...,
//     "encrypted_payload": ...
//   }
// }
struct Body
{
  std::vector<std::string> array;
  std::unordered_map<std::string, std::string> dict;
  double f;
  int i;
  elle::Buffer encrypted_payload;

  // Generate a model for the class Body.
  using Model = das::Model<Body, elle::meta::List<Symbol_array,
                                                  Symbol_dict,
                                                  Symbol_f,
                                                  Symbol_i,
                                                  Symbol_encrypted_payload>>;
};

// Create a wrapper around any class to have:
// {
//   body: {...}
//   signature: ...
// }
template <typename T>
struct Signed
{
  T body;
  elle::Buffer signature;

  // What to sign.
  // Here, we use the binary representation of body.
  static
  elle::Buffer
  signature_plain(T const& body)
  {
    elle::Buffer res;
    {
      elle::IOStream output(res.ostreambuf());
      elle::serialization::binary::SerializerOut serializer(output, false);
      das::serialize(body, serializer);
    }
    return res;
  }

  // Generate a Model the templated class Signed<T>.
  using Model = das::Model<Signed<T>, elle::meta::List<Symbol_body,
                                                       Symbol_signature
                                                       >>;
};

using Example = Signed<Body>;

// Declare both Body and Example serializable.
DAS_SERIALIZE(Body);
DAS_SERIALIZE(Example);

// Declare all das::Model printable.
using das::operator <<;

// Read your fake keypair from examples/example.key.
static
elle::cryptography::rsa::KeyPair
key()
{
  elle::AtomicFile f("examples/example.key");
  return f.read() << [&] (elle::AtomicFile::Read& read)
  {
    return elle::serialization::json::deserialize<
      elle::cryptography::rsa::KeyPair>(read.stream(), false);
  };
}

// Download the example file.
static
Example
get()
{
  std::string url = "https://gist.githubusercontent.com"
    "/Dimrok"
    "/a9a6898bc84397b7b037b97a03c58437/raw"
    "/d7764d2b26814af3bfff17c707dc4f33a93f7e8c"
    "/example.json";
  std::cout << "1. GET example.json at " << url << std::endl;
  elle::reactor::http::Request r(url);
  elle::reactor::wait(r);
  std::cout << "2. Deserialize response" << std::endl;
  return elle::serialization::json::deserialize<Example>(r);
}

// 1. Downlaod the example file
// 2. Deserialize its content
// 3. Ensure the signature is valid (the private key has signed the body)
// 4. Decrypt the hidden secret
int
main()
{
  elle::reactor::Scheduler sched;
  elle::reactor::Thread t(
    sched, "main",
    []
    {
      try
      {
        auto example = get();
        auto public_key = key().K();
        // Check if you are the author of the signature.
        std::cout << "3. Verify signature" << std::endl;
        if (!public_key.verify(example.signature,
                               Example::signature_plain(example.body)))
          // Throw an elle::Exception.
          elle::err("invalid signature: Someone replaced your data");
        std::cout << "> You are the author of the of "
                 << example.body << std::endl;
        std::cout << "4. Decrypt secret" << std::endl;
        elle::ConstWeakBuffer payload(example.body.encrypted_payload);
        auto secret = key().k().decrypt(payload).string();
        // std::cout << "> " << secret << std::endl;
      }
      catch (elle::reactor::http::ResolutionFailure const& e)
      {
        std::cerr << "Unable to resolve " << e.url() << std::endl;
        throw;
      }
      catch (elle::reactor::http::RequestError const& e)
      {
        std::cerr << "Unable to get " << e.url() << std::endl;
        throw;
      }
    });
  try
  {
    sched.run();
  }
  catch (...)
  {
    return 1;
  }
  return 0;
}
