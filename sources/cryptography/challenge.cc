#include <cryptography/challenge.hh>
#include <cryptography/random.hh>
#include <cryptography/Exception.hh>
#include <cryptography/PublicKey.hh>
#include <cryptography/PrivateKey.hh>
#include <cryptography/Code.hh>

#include <elle/log.hh>
#include <elle/attribute.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/extract.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.challenge");

//
// ---------- Classes ---------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    // An internal class for easing the process of exchanging information
    // between two sides.
    class Challenge
    {
      /*-------------.
      | Construction |
      `-------------*/
    public:
      Challenge() // XXX to remove
      {}

      Challenge(PublicKey const& challenger_K,
                elle::String const& nonce):
        _challenger_K(challenger_K),
        _nonce(nonce)
      {}

      Challenge(Challenge const& other):
        _challenger_K(other._challenger_K),
        _nonce(other._nonce)
      {}

      Challenge(Challenge&& other):
        _challenger_K(std::move(other._challenger_K)),
        _nonce(std::move(other._nonce))
      {}

      ELLE_SERIALIZE_CONSTRUCT(Challenge,
                               _challenger_K)
      {}

      /*-----------.
      | Interfaces |
      `-----------*/
    public:
      ELLE_SERIALIZE_FRIEND_FOR(Challenge);

      /*-----------.
      | Attributes |
      `-----------*/
    public:
      ELLE_ATTRIBUTE_R(PublicKey, challenger_K);
      ELLE_ATTRIBUTE_R(elle::String, nonce);
    };
  }
}

/*-----------.
| Serializer |
`-----------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Challenge,
                      archive,
                      value,
                      format)
{
  enforce(format == 0, "unknown format");

  archive & value._challenger_K;
  archive & value._nonce;
}

//
// ---------- Helpers ---------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace challenge
    {
      /*----------.
      | Constants |
      `----------*/

      elle::Natural32 const nonce_length = 32;

      /*----------.
      | Functions |
      `----------*/

      elle::String
      nonce()
      {
        ELLE_TRACE_FUNCTION("");

        return (random::generate<elle::String>(nonce_length));
      }

      elle::String
      create(PublicKey const& challenger_K,
             PublicKey const& challengee_K,
             elle::String const& nonce)
      {
        ELLE_TRACE_FUNCTION(challenger_K, challengee_K, nonce);

        // Encrypt with the challengee's public key both the
        // challenger's public key and the nonce.
        //
        // Once receiving the challenge, the challengee will
        // be able to decrypt it with its private key and
        // respond by encrypting the challenge by encrypting
        // it with the challenger's public key.

        Challenge _challenge(challenger_K, nonce);

        Code code = challengee_K.encrypt(_challenge);

        elle::String challenge;
        elle::serialize::to_string<
          elle::serialize::OutputBase64Archive>(challenge) << code;

        return (challenge);
      }

      static
      Challenge
      _accept(elle::String const& challenge,
              PrivateKey const& challengee_k)
      {
        ELLE_DEBUG_FUNCTION(challenge, challengee_k);

        auto extractor =
          elle::serialize::from_string<
            elle::serialize::InputBase64Archive>(challenge);
        Code code(extractor);

        Challenge _challenge = challengee_k.decrypt<Challenge>(code);

        return (_challenge);
      }

      static
      elle::String
      _accept(elle::String const& challenge,
              PrivateKey const& challengee_k,
              PublicKey const* challenger_K)
      {
        ELLE_DEBUG_FUNCTION(challenge, challengee_k, challenger_K);

        // Decrypt the challenge and extract both the challenger's
        // public key and the nonce.
        Challenge _challenge = _accept(challenge, challengee_k);

        // Check that the challenger is the expected one, if required.
        if (challenger_K != nullptr)
        {
          if (*challenger_K != _challenge.challenger_K())
            throw Exception(
              elle::sprintf("the challenger does not seem to be the expected "
                            "one: challenger_K(%s) versus in-challenge(%s)",
                            *challenger_K, _challenge.challenger_K()));
        }

        // Then, re-encrypt the nonce with the challenger's public key
        // for the challenger only to be able to decrypt it.
        Code code = _challenge.challenger_K().encrypt(_challenge.nonce());

        elle::String response;
        elle::serialize::to_string<
          elle::serialize::OutputBase64Archive>(response) << code;

        return (response);
      }

      elle::String
      accept(elle::String const& challenge,
             PrivateKey const& challengee_k)
      {
        ELLE_TRACE_FUNCTION(challenge, challengee_k);

        return (_accept(challenge, challengee_k, nullptr));
      }

      elle::String
      accept(elle::String const& challenge,
             PrivateKey const& challengee_k,
             PublicKey const& challenger_K)
      {
        ELLE_TRACE_FUNCTION(challenge, challengee_k, challenger_K);

        return (_accept(challenge, challengee_k, &challenger_K));
      }

      elle::Boolean
      check(elle::String const& response,
            PrivateKey const& challenger_k,
            elle::String const& nonce)
      {
        ELLE_TRACE_FUNCTION(response, challenger_k, nonce);

        // Decrypt the response with the challenger's public key and
        // compare the extracted nonce from the original one.
        try
        {
          auto extractor =
            elle::serialize::from_string<
              elle::serialize::InputBase64Archive>(response);
          Code code(extractor);

          elle::String _nonce = challenger_k.decrypt<elle::String>(code);

          if (nonce != _nonce)
            return (false);

          return (true);
        }
        catch (Exception const& e)
        {
          return (false);
        }

        elle::unreachable();
      }
    }
  }
}
