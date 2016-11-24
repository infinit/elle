#ifndef INFINIT_CRYPTOGRAPHY_CONSTANTS_HH
# define INFINIT_CRYPTOGRAPHY_CONSTANTS_HH

namespace infinit
{
  namespace cryptography
  {
    namespace constants
    {
      /*----------.
      | Constants |
      `----------*/

      /// The size of the chunk to process iteratively from the streams.
      static uint32_t const stream_block_size = 524288;
    }
  }
}

#endif
