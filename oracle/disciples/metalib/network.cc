
#include "elle/cryptography/KeyPair.hh"
#include "elle/io/Path.hh"
#include <elle/types.hh>
#include "elle/io/Unique.hh"

#include "hole/Model.hh"

#include <lune/Identity.hh>
#include <lune/Authority.hh>
#include <lune/Descriptor.hh>

#include <nucleus/proton/Address.hh>
#include <nucleus/proton/Block.hh>
#include <nucleus/neutron/Access.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Trait.hh>


// XXX When Qt is out, remove this
#ifdef slots
# undef slots
#endif
#ifdef slot
# undef slot
#endif

#include "metalib.hh"
#include "network.hh"


static elle::io::Unique
generate_network_descriptor(elle::String const& id,
                            elle::String const& name,
                            elle::String const& model_name,
                            elle::io::Unique const& root_address,
                            elle::String const& authority_file,
                            elle::String const& authority_password)
{
  lune::Descriptor descriptor;
  hole::Model model;
  nucleus::proton::Address address;
  lune::Authority authority;
  elle::io::Path authority_path;

  if (authority_path.Create(authority_file) == elle::Status::Error)
    throw std::runtime_error("unable to create authority path");

  if (authority.Load(authority_path) == elle::Status::Error)
    throw std::runtime_error("unable to load the authority file");

  if (authority.Decrypt(authority_password) == elle::Status::Error)
    throw std::runtime_error("unable to decrypt the authority");

  if (model.Create(model_name) != elle::Status::Ok)
    throw std::runtime_error("unable to create model");

  if (address.Restore(root_address) != elle::Status::Ok)
    throw std::runtime_error("Unable to restore root address");

  if (descriptor.Create(id, name, model, address,
                        lune::Descriptor::History,
                        lune::Descriptor::Extent,
                        lune::Descriptor::Contention,
                        lune::Descriptor::Balancing) != elle::Status::Ok)
    throw std::runtime_error("Unable to create the network descriptor");

  if (descriptor.Seal(authority) != elle::Status::Ok)
    throw std::runtime_error("Unable to seal the network descriptor");

  elle::io::Unique unique;
  if (descriptor.Save(unique) != elle::Status::Ok)
    throw std::runtime_error("Unable to save the network descriptor");

  return unique;
}


extern "C" PyObject* metalib_generate_network_descriptor(PyObject* self, PyObject* args)
{
  char const* network_id = nullptr,
            * network_name = nullptr,
            * network_model = nullptr,
            * root_address = nullptr,
            * authority_file = nullptr,
            * authority_password = nullptr;
  PyObject* ret = nullptr;

  if (!PyArg_ParseTuple(args, "ssssss:check_root_block_signature",
                        &network_id, &network_name, &network_model, &root_address,
                        &authority_file, &authority_password))
    return nullptr;

  PyThreadState *_save;
  _save = PyEval_SaveThread();

  try
    {
      elle::io::Unique descriptor = generate_network_descriptor(
          network_id, network_name, network_model, root_address,
          authority_file, authority_password
      );

      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);

      ret = Py_BuildValue("s", descriptor.c_str());
    }
  catch (std::exception const& err)
    {
      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);
      std::cout << "############################################################\n";
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      std::cout << "############################################################\n";
      char const* error_string = err.what();
      PyErr_SetString(metalib_MetaError, error_string);
    }

  return ret;
}


static bool check_root_block_signature(elle::io::Unique const& root_block,
                                       elle::io::Unique const& root_address,
                                       elle::io::Unique const& public_key)
{
  nucleus::neutron::Object directory;
  nucleus::proton::Address address;
  elle::cryptography::PublicKey       K;

  std::cerr << "GOT pub key: " << public_key << "\n"
            << "GOT root block: " << root_block << "\n"
            << "GOT root address: " << root_address << "\n";

  if (K.Restore(public_key) != elle::Status::Ok)
    throw std::runtime_error("Unable to restore public key");

  if (address.Restore(root_address) == elle::Status::Error)
    throw std::runtime_error("Unable to restore root address");

  if (directory.Restore(root_block) == elle::Status::Error)
    throw std::runtime_error("Unable to restore root block: <" + root_block + ">");

  auto access = nucleus::neutron::Access::Null;
  if (directory.Validate(address, access) != elle::Status::Ok)
    return false;

  if (directory.owner.K != K)
    return false;

  return true;
}


extern "C" PyObject* metalib_check_root_block_signature(PyObject* self, PyObject* args)
{
  char const* root_block = nullptr;
  char const* root_address = nullptr;
  char const* public_key = nullptr;
  PyObject* ret = nullptr;

  if (!PyArg_ParseTuple(args, "sss:check_root_block_signature",
                        &root_block, &root_address, &public_key))
    return nullptr;

  if (!root_block || !public_key)
    return nullptr;

  PyThreadState *_save;
  _save = PyEval_SaveThread();

  try
    {
      bool result = check_root_block_signature(
          root_block,
          root_address,
          public_key
      );
      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);
      ret = result ? Py_True : Py_False;
      Py_INCREF(ret);
    }
  catch (std::exception const& err)
    {
      // WARNING: restore state before setting exception !
      PyEval_RestoreThread(_save);
      std::cout << "############################################################\n";
#include <elle/idiom/Open.hh>
      show();
#include <elle/idiom/Close.hh>
      std::cout << "############################################################\n";
      char const* error_string = err.what();
      PyErr_SetString(metalib_MetaError, error_string);
    }

  return ret;
}
