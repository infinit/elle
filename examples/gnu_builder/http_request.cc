#include <iostream>

#include <curl/curl.h>

int
main(int argc, char** argv)
{
  CURL* curl;
  CURLcode res;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, "https://example.com/");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    // In order to verify the host, we need a set of trusted CA certificates.
    // curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
      std::cerr << "curl_easy_perform() failed: " << std::endl
                << curl_easy_strerror(res);
    }
    curl_easy_cleanup(curl);
  }
  curl_global_cleanup();
  return 0;
}
