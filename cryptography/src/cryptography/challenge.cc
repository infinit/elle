#include <cryptography/challenge.hh>
#include <cryptography/random.hh>
#include <cryptography/Error.hh>
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
                Code nonce):
        _challenger_K(challenger_K),
        _nonce(std::move(nonce))
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
                               _challenger_K, _nonce)
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
      ELLE_ATTRIBUTE_R(Code, nonce);
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

        // Encrypt the nonce with the challengee's public key.
        //
        // Once receiving the challenge, the challengee will
        // be able to decrypt it with its private key and
        // respond by encrypting the challenge by encrypting
        // it with the challenger's public key.

        Code code = challengee_K.encrypt(nonce);

        Challenge _challenge(challenger_K, std::move(code));

        elle::String challenge;
        elle::serialize::to_string<
          elle::serialize::OutputBase64Archive>(challenge) << _challenge;

        return (challenge);
      }

      static
      elle::String
      _accept(elle::String const& challenge,
              KeyPair const& challengee_keypair,
              PublicKey const* challenger_K)
      {
        ELLE_DEBUG_FUNCTION(challenge, challengee_keypair, challenger_K);

        // Decrypt the challenge so as to extract both the challenger's
        // public key and the nonce, in its encrypted form.
        auto extractor =
          elle::serialize::from_string<
            elle::serialize::InputBase64Archive>(challenge);
        Challenge _challenge(extractor);

        elle::String _nonce =
          challengee_keypair.k().decrypt<elle::String>(_challenge.nonce());

        // Check that the challenger is the expected one, if required.
        if (challenger_K != nullptr)
        {
          if (*challenger_K != _challenge.challenger_K())
            throw Error(
              elle::sprintf("the challenger does not seem to be the expected "
                            "one: challenger_K(%s) versus in-challenge(%s)",
                            *challenger_K, _challenge.challenger_K()));
        }

        // Then, re-encrypt the nonce with the challenger's public key
        // for the challenger only to be able to decrypt it.
        elle::String response =
          create(challengee_keypair.K(), _challenge.challenger_K(), _nonce);

        return (response);
      }

      elle::String
      accept(elle::String const& challenge,
             KeyPair const& challengee_keypair)
      {
        ELLE_TRACE_FUNCTION(challenge, challengee_keypair);

        return (_accept(challenge, challengee_keypair, nullptr));
      }

      elle::String
      accept(elle::String const& challenge,
             KeyPair const& challengee_keypair,
             PublicKey const& challenger_K)
      {
        ELLE_TRACE_FUNCTION(challenge, challengee_keypair, challenger_K);

        return (_accept(challenge, challengee_keypair, &challenger_K));
      }

      elle::Boolean
      check(elle::String const& response,
            PrivateKey const& challenger_k,
            elle::String const& nonce)
      {
        ELLE_TRACE_FUNCTION(response, challenger_k, nonce);

        // Decrypt the response with the challenger's public key and
        // compare the extracted nonce, once decrypted, from the original one.
        try
        {
          auto extractor =
            elle::serialize::from_string<
              elle::serialize::InputBase64Archive>(response);
          Challenge _challenge(extractor);

          elle::String _nonce =
            challenger_k.decrypt<elle::String>(_challenge.nonce());

          if (nonce != _nonce)
            return (false);

          return (true);
        }
        catch (Error const& e)
        {
          return (false);
        }

        elle::unreachable();
      }
    }
  }
}
