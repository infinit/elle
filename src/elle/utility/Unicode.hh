namespace elle
{
  namespace utility
  {

    /// XXX \todo create a static class for that!

    /**
     * @brief converts an utf8 string to an utf16 string
     *
     * @param input valid input
     * @param input_size the size in byte of input, can be -1 if input is null
     * terminated
     * @param output the output destination, will be realloced
     * @param output_size the size of the outpout destination, non null
     */
    Status Utf8To16(const char * input,
                    ssize_t      input_size,
                    wchar_t **   output,
                    ssize_t *    output_size);

    /**
     * @brief converts an utf16 string to an utf8 string
     *
     * @param input valid input
     * @param input_size the size in byte of input, can be -1 if input is null
     * terminated
     * @param output the output destination, will be realloced
     * @param output_size the size of the outpout destination, non null
     */
    Status Utf16To8(const wchar_t * input,
                    ssize_t         input_size,
                    char **         output,
                    ssize_t *       output_size);
  }
}
