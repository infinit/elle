#include <elle/Elle.hh>
#include <elle/types.hh>
#include <elle/Exception.hh>
#include <elle/format/hexadecimal.hh>

#include <cryptography/Digest.hh>
#include <cryptography/OneWay.hh>
#include <cryptography/Random.hh>

#include <nucleus/Nucleus.hh>
#include <nucleus/proton/Porcupine.hh>
#include <nucleus/proton/Nest.hh>
#include <nucleus/proton/Limits.hh>
#include <nucleus/proton/Statistics.hh>
#include <nucleus/neutron/Catalog.hh>
#include <nucleus/neutron/Entry.hh>
#include <nucleus/neutron/Range.hh>

#include <etoile/Etoile.hh>
#include <etoile/gear/Transcript.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/nest/Nest.hh>

#include <lune/Lune.hh>

#include <hole/Hole.hh>

#include <Infinit.hh>

// XXX[temporary: for cryptography]
using namespace infinit;

ELLE_LOG_COMPONENT("infinit.tests.nucleus.proton.Porcupine");

// To define in order to make the checks stronger and so as to
// detect inconsistencies early on.
#undef PORCUPINE_THOROUGH_CHECK

// To define to test the serialization mechanism with porcupines.
#undef PORCUPINE_SERIALIZE_TEST

// To define to dump the porcupine's statistcs.
#undef PORCUPINE_STATISTICS

std::vector<elle::String>
test_porcupine_prepare(elle::Natural32 const n)
{
  std::vector<elle::String> vector(n);

  for (elle::Natural32 i = 0; i < n; i++)
    {
      cryptography::Digest digest{
        cryptography::oneway::hash(i, cryptography::oneway::Algorithm::sha1)};

      elle::String string =
        elle::format::hexadecimal::encode(
          (const char*)digest.buffer().contents(), digest.buffer().size());

      vector[i] = string;
    }

  return (vector);
}

void
test_porcupine_add(nucleus::proton::Porcupine& porcupine,
                   std::vector<elle::String>& vector)
{
  ELLE_TRACE_FUNCTION(porcupine, vector);

  for (elle::Natural32 i = 0; i < vector.size(); i++)
    {
      nucleus::proton::Handle handle;

      ELLE_TRACE_SCOPE("[%s] add(%s)", i, vector[i]);

      handle = porcupine.lookup<nucleus::neutron::Catalog>(vector[i]);

      nucleus::proton::Ambit<nucleus::neutron::Catalog> catalog(
        porcupine.nest(), handle);

      catalog.load();

      nucleus::neutron::Entry* entry{
        new nucleus::neutron::Entry(vector[i],
                                    nucleus::proton::Address::null())};

      catalog()->insert(entry);

      catalog.unload();

      porcupine.update<nucleus::neutron::Catalog>(vector[i]);

#ifdef PORCUPINE_THOROUGH_CHECK
      porcupine.check<nucleus::neutron::Catalog>(
        nucleus::proton::Porcupine::FlagRecursive |
        nucleus::proton::Porcupine::FlagKey |
        nucleus::proton::Porcupine::FlagCapacity |
        nucleus::proton::Porcupine::FlagFootprint |
        nucleus::proton::Porcupine::FlagState);
#endif
    }

  assert(porcupine.capacity() == vector.size());
  assert(porcupine.state() == nucleus::proton::StateDirty);

  porcupine.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagRecursive |
    nucleus::proton::Porcupine::FlagKey |
    nucleus::proton::Porcupine::FlagCapacity |
    nucleus::proton::Porcupine::FlagFootprint |
    nucleus::proton::Porcupine::FlagState);
}

void
test_porcupine_lookup(nucleus::proton::Porcupine& porcupine,
                      std::vector<elle::String>& vector)
{
  ELLE_TRACE_FUNCTION(porcupine, vector);

  for (elle::Natural32 i = 0; i < vector.size(); i++)
    {
      nucleus::proton::Handle handle;

      ELLE_TRACE_SCOPE("[%s] lookup(%s)", i, vector[i]);

      handle = porcupine.lookup<nucleus::neutron::Catalog>(vector[i]);

      nucleus::proton::Ambit<nucleus::neutron::Catalog> catalog(
        porcupine.nest(), handle);

      catalog.load();

      assert(catalog()->exists(vector[i]) == true);

      catalog.unload();

      assert(porcupine.exists<nucleus::neutron::Catalog>(vector[i]) == true);

#ifdef PORCUPINE_THOROUGH_CHECK
      porcupine.check<nucleus::neutron::Catalog>(
        nucleus::proton::Porcupine::FlagRecursive |
        nucleus::proton::Porcupine::FlagKey |
        nucleus::proton::Porcupine::FlagCapacity |
        nucleus::proton::Porcupine::FlagFootprint |
        nucleus::proton::Porcupine::FlagState);
#endif
    }

  porcupine.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagRecursive |
    nucleus::proton::Porcupine::FlagKey |
    nucleus::proton::Porcupine::FlagCapacity |
    nucleus::proton::Porcupine::FlagFootprint |
    nucleus::proton::Porcupine::FlagState);
}

void
test_porcupine_seal(nucleus::proton::Porcupine& porcupine,
                    cryptography::SecretKey& secret)
{
  ELLE_TRACE_FUNCTION(porcupine, secret);

  porcupine.seal<nucleus::neutron::Catalog>(secret);

  porcupine.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagAll);
}

void
test_porcupine_seek(nucleus::proton::Porcupine& porcupine,
                    std::vector<elle::String>& vector)
{
  ELLE_TRACE_FUNCTION(porcupine, vector);

  std::vector<elle::String> w(vector.size());

  for (elle::Natural32 i = 0; i < vector.size();)
    {
      nucleus::proton::Handle handle;
      elle::Natural64 base;

      ELLE_TRACE_SCOPE("[%s] seek(%s)", i, i);

      handle = porcupine.seek<nucleus::neutron::Catalog>(i, base);

      nucleus::proton::Ambit<nucleus::neutron::Catalog> catalog(
        porcupine.nest(), handle);

      catalog.load();

      nucleus::neutron::Range<nucleus::neutron::Entry> range;

      range = catalog()->consult(i - base,
                                 catalog()->capacity() - (i - base));

      for (auto it = range.container.begin();
           it != range.container.end();
           ++it)
        {
          nucleus::neutron::Entry* entry = *it;

          w[i++] = entry->name();
        }

      catalog.unload();

#ifdef PORCUPINE_THOROUGH_CHECK
      porcupine.check<nucleus::neutron::Catalog>(
        nucleus::proton::Porcupine::FlagRecursive |
        nucleus::proton::Porcupine::FlagKey |
        nucleus::proton::Porcupine::FlagCapacity |
        nucleus::proton::Porcupine::FlagFootprint |
        nucleus::proton::Porcupine::FlagState);
#endif
    }

  std::vector<elle::String> s(vector);
  sort(s.begin(), s.end());

  assert(s == w);

  porcupine.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagAll);
}

nucleus::proton::Porcupine
test_porcupine_serialize(nucleus::proton::Porcupine& input,
                         cryptography::SecretKey& secret,
                         std::vector<elle::String>& vector)
{
  etoile::gear::Transcript transcript;
  etoile::nest::Nest* nest = static_cast<etoile::nest::Nest*>(&input.nest());

  ELLE_TRACE_FUNCTION(input, secret, vector);

  nest->record(transcript);

  for (auto action: transcript)
    {
      // perform the action.
      switch (action->type())
        {
        case etoile::gear::Action::Type::push:
          {
            // store the block in the depot.
            if (etoile::depot::Depot::Push(
                  action->address(),
                  action->block()) == elle::Status::Error)
              throw elle::Exception("unable to push the block in the depot");

            break;
          }
        case etoile::gear::Action::Type::wipe:
          {
            // wipe the block from the depot.
            if (etoile::depot::Depot::Wipe(
                  action->address()) == elle::Status::Error)
              throw elle::Exception("unable to wipe the block from the depot");

            break;
          }
        default:
          {
            throw elle::Exception("unknown action type");
          }
        }
    }

  nucleus::proton::Porcupine output;
  etoile::nest::Nest* n = new etoile::nest::Nest(input.nest().limits());
  {
    elle::Buffer buffer;
    buffer.writer() << input;
    buffer.reader() >> output;

    output.nest(*n);
    output.unseal(secret);
  }

  // This first lookup phase is going to be slower than when the
  // porcupine has been created because blocks needed to be fetched
  // from the storage layer.
  test_porcupine_lookup(output, vector);

  test_porcupine_seek(output, vector);

  input.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagAll);

  return (output);
}

void
test_porcupine_remove(nucleus::proton::Porcupine& porcupine,
                      std::vector<elle::String>& vector,
                      elle::Natural32 const index,
                      elle::Natural32 const size)
{
  ELLE_TRACE_FUNCTION(porcupine, vector, index, size);

  for (elle::Natural32 i = index; i < (index + size); i++)
    {
      nucleus::proton::Handle handle;

      ELLE_TRACE_SCOPE("[%s] remove(%s)", i, vector[i]);

      handle = porcupine.lookup<nucleus::neutron::Catalog>(vector[i]);

      nucleus::proton::Ambit<nucleus::neutron::Catalog> catalog(
        porcupine.nest(), handle);

      catalog.load();

      catalog()->erase(vector[i]);

      catalog.unload();

      porcupine.update<nucleus::neutron::Catalog>(vector[i]);

#ifdef PORCUPINE_THOROUGH_CHECK
      porcupine.check<nucleus::neutron::Catalog>(
        nucleus::proton::Porcupine::FlagRecursive |
        nucleus::proton::Porcupine::FlagKey |
        nucleus::proton::Porcupine::FlagCapacity |
        nucleus::proton::Porcupine::FlagFootprint |
        nucleus::proton::Porcupine::FlagState);
#endif
    }

  vector.erase(vector.begin() + index, vector.begin() + index + size);

  porcupine.check<nucleus::neutron::Catalog>(
    nucleus::proton::Porcupine::FlagRecursive |
    nucleus::proton::Porcupine::FlagKey |
    nucleus::proton::Porcupine::FlagCapacity |
    nucleus::proton::Porcupine::FlagFootprint |
    nucleus::proton::Porcupine::FlagState);

  assert(porcupine.state() == nucleus::proton::StateDirty);
}

void
test_porcupine_catalog()
{
  // A test with N = 20000 and an extent of 1024 leads to a porcupine
  // with 6 levels which is enough to test all cases.

  elle::Natural32 const N = 2000;
  std::vector<elle::String> vector;

  vector = test_porcupine_prepare(N);

  // XXX[provide a path to where blocks should be stored, if not, impossible to store]
  etoile::nest::Nest nest1(nucleus::proton::Limits(1024, 0.5, 0.2));
  nucleus::proton::Porcupine porcupine1(nest1);

  test_porcupine_add(porcupine1, vector);

  assert(porcupine1.height() >= 1);
  assert(porcupine1.height() <= 10);

  test_porcupine_lookup(porcupine1, vector);

#ifdef PORCUPINE_STATISTCS
  nucleus::proton::Statistics stats =
    porcupine1.statistics<nucleus::neutron::Catalog>();
  stats.Dump();
#endif

  cryptography::SecretKey secret1;
  secret1.Generate(nucleus::proton::Porcupine::secret_length);

  test_porcupine_seal(porcupine1, secret1);

  test_porcupine_seek(porcupine1, vector);

#ifdef PORCUPINE_SERIALIZE_TEST
  nucleus::proton::Porcupine porcupine2 =
    test_porcupine_serialize(porcupine1, secret1, vector);

  test_porcupine_remove(porcupine2, vector, N / 3, N / 3);

  cryptography::SecretKey secret2;
  secret2.Generate(nucleus::proton::Porcupine::secret_length);

  test_porcupine_seal(porcupine2, secret2);

  nucleus::proton::Porcupine porcupine3 =
    test_porcupine_serialize(porcupine2, secret2, vector);
#endif

  test_porcupine_remove(porcupine1, vector, 0, vector.size());
}

int
Main(elle::Natural32,
     elle::Character* argv[])
{
  // XXX
  Infinit::Network = "local";

  try
    {
      lune::Lune::Initialize();
      etoile::Etoile::Initialize();
      Infinit::Initialize();

#ifdef PORCUPINE_SERIALIZE_TEST
      hole::Hole::Initialize();
#endif

      test_porcupine_catalog();

#ifdef PORCUPINE_SERIALIZE_TEST
      hole::Hole::Clean();
#endif

      Infinit::Clean();
      etoile::Etoile::Clean();
      lune::Lune::Clean();

      std::cout << "tests done.\n";
    }
  catch (reactor::Exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e << std::endl;
      goto _error;
    }
  catch (std::exception const& e)
    {
      std::cerr << argv[0] << ": fatal error: " << e.what() << std::endl;
      goto _error;
    }
  catch (...)
    {
      std::cerr << argv[0] << ": unknown exception" << std::endl;
      goto _error;
    }

  elle::concurrency::scheduler().terminate();
  return (0);

 _error:
  elle::concurrency::scheduler().terminate();
  return (1);
}

int
main(int argc,
     char* argv[])
{
  reactor::Scheduler& sched = elle::concurrency::scheduler();
  reactor::VThread<int> main(sched, "main",
                             boost::bind(&Main, argc, argv));
  sched.run();
  return main.result();
}
