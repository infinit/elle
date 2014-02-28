#ifndef AWS_S3_S3_HH
# define AWS_S3_S3_HH

# include <map>
# include <vector>

# include <elle/Printable.hh>
# include <elle/Buffer.hh>
# include <elle/attribute.hh>

# include <aws/CanonicalRequest.hh>
# include <aws/Credentials.hh>
# include <aws/StringToSign.hh>

namespace aws
{
  class S3:
    public elle::Printable
  {
    /*------.
    | Types |
    `------*/
  public:
    typedef uint64_t FileSize;
    typedef std::vector<std::pair<std::string, FileSize>> List;

    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// Create a new S3 handler.
    /// This requires a bucket name, remote folder and set of credentials.
    /// Use of this should be optimised based on S3 pricing:
    /// http://aws.amazon.com/s3/pricing/
    S3(std::string const& bucket_name,
       std::string const& remote_folder,
       Credentials const& credentials);

    /*-----------.
    | Operations |
    `-----------*/
  public:
    /// Put a block (as a file) into the remote folder.
    /// This function does a single PUT call, so block size should be something
    /// reasonable for an HTTP request.
    void
    put_object(elle::ConstWeakBuffer const& object,
               std::string const& object_name);

    /// Returns a list of all files names and their respective sizes inside the
    /// remote folder.
    /// This is limited to 1000 results but a starting offset (marker) can be
    /// used if more are required. It is important to note that the files are
    /// listed in alphabetical order.
    std::vector<std::pair<std::string, FileSize>>
    list_remote_folder(std::string const& marker = "");

    /// Fetch an object from the remote folder.
    /// The fetch is done in a single GET.
    elle::Buffer
    get_object(std::string const& object_name);

    /// Delete an object in the remote folder.
    /// The folder itself can be deleted only once it is empty. This can be done
    /// by setting the object_name to an empty string.
    void
    delete_object(std::string const& object_name);

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    ELLE_ATTRIBUTE(std::string, bucket_name);
    ELLE_ATTRIBUTE(std::string, remote_folder);
    ELLE_ATTRIBUTE(Credentials, credentials);
    ELLE_ATTRIBUTE(std::string, host_name);

    /*--------.
    | Helpers |
    `--------*/
  private:
    std::string
    _md5_digest(elle::ConstWeakBuffer const& buffer);

    std::string
    _sha256_hexdigest(elle::ConstWeakBuffer const& buffer);

    std::string
    _amz_date(RequestTime const& request_time);

    std::vector<std::string>
    _signed_headers(RequestHeaders const& headers);

    StringToSign
    _make_string_to_sign(RequestTime const& request_time,
                         std::string const& canonical_request_sha256);

    std::vector<std::pair<std::string, FileSize>>
    _parse_list_xml(std::istream& stream);

    RequestHeaders
    _make_put_headers(elle::ConstWeakBuffer const& object,
                      RequestTime const& request_time);

    RequestHeaders
    _make_generic_headers(RequestTime const& request_time);

    CanonicalRequest
    _make_put_canonical_request(elle::ConstWeakBuffer const& object,
                                std::string const& object_name,
                                RequestHeaders const& headers);

    CanonicalRequest
    _make_list_canonical_request(RequestHeaders const& headers,
                                 RequestQuery const& query);

    CanonicalRequest
    _make_get_canonical_request(RequestHeaders const& headers,
                                std::string const& object_name);

    CanonicalRequest
    _make_delete_canonical_request(RequestHeaders const& headers,
                                   std::string const& object_name);

    /*----------.
    | Printable |
    `----------*/
  public:
    virtual
    void
    print(std::ostream& stream) const;
  };
}

#endif
