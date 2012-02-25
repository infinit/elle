#
# ---------- header -----------------------------------------------------------
#
# project       dependencies
#
# license       infinit
#
# author        julien quintard   [sat feb 25 12:55:32 2012]
#

# openssl
OPENSSL_LIBRARIES="${OPENSSL_LIBRARIES} ${WORKDIR}/lib/libssl.dylib ${WORKDIR}/lib/libcrypto.dylib"

# msgpack
MSGPACK_LIBRARIES="${MSGPACK_LIBRARIES} ${WORKDIR}/lib/libmsgpack.dylib"

# jsoncpp
JSONCPP_LIBRARIES="${JSONCPP_LIBRARIES} ${WORKDIR}/lib/libjsoncpp.dylib"

# curl
CURL_LIBRARIES="${CURL_LIBRARIES} ${WORKDIR}/lib/libcurl.dylib"

# qjson
QJSON_LIBRARIES="${QJSON_LIBRARIES} ${WORKDIR}/lib/libqjson.dylib"
