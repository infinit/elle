#ifndef INFINIT_CRYPTOGRAPHY_CHALLENGE_HH
# define INFINIT_CRYPTOGRAPHY_CHALLENGE_HH

# include <cryptography/fwd.hh>

# include <elle/types.hh>

namespace infinit
{
  namespace cryptography
  {
    /// Provide functions for setting up a challenge so as to authenticate
    /// a user.
    ///
    /// Note that the challenge mechanism relies on an asymetric cryptosystem
    /// as detailed in the protocol below.
    ///
    /// First, the node acting as the server decides to authenticate another
    /// node, referred to as the client.
    ///
    /// The server starts by generating a nonce, which is a random value used
    /// exclusively for the purpose of this challenge.
    ///
    ///   elle::String nonce = nonce();
    ///
    /// Then, the server create a challenge for authenticating a user referred
    /// to as the challengee, in opposition to the server which acts as the
    /// challenger:
    ///
    ///   elle::String challenge = create(client_K, server_K, nonce);
    ///
    /// The challenge string can then be sent to the client.
    ///
    /// Upon reception, the client can accept the challenge by decrypting
    /// it, extracting both the server's public key and nonce and re-encrypting
    /// the nonce with it for the response to be decrypted by the server only.
    ///
    ///   elle::String response = accept(challenge, client_k);
    ///
    /// Finally, the response can be sent back to the server for decryption and
    /// final verification after which the client will be considered
    /// authenticated.
    ///
    ///   if (check(response, server_k, nonce) == false)
    ///     throw Error("failed challenge");
    namespace challenge
    {
      /*----------.
      | Functions |
      `----------*/

      /// Generate a random value for the purpose of a challenge.
      ///
      /// Note that a nonce should never be re-used for another challenge.
      elle::String
      nonce();
      /// Create a challenge by encrypting both the challenger's public key
      /// and the nonce with the challengee's public key for him only to be
      /// able to decrypt the tuple.
      elle::String
      create(PublicKey const& challenger_K,
             PublicKey const& challengee_K,
             elle::String const& nonce);
      /// Take a challenge string, decrypt it with the challengee's private key,
      /// extract the nonce and re-encrypt it with the challenger's public key.
      elle::String
      accept(elle::String const& challenge,
             KeyPair const& challengee_keypair);
      /// Accept a challenge and check that the challenger is the expected one
      /// by verifying that the public key embedded in the challenge string is
      /// the right one.
      elle::String
      accept(elle::String const& challenge,
             KeyPair const& challengee_keypair,
             PublicKey const& challenger_K);
      /// Decrypt the response with the challenger's private key, extract the
      /// response nonce and check its validy given the original nonce.
      elle::Boolean
      check(elle::String const& response,
            PrivateKey const& challenger_k,
            elle::String const& nonce);
    }
  }
}

#endif
