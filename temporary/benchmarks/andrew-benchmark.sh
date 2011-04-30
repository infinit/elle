#! /bin/sh

#
# andrew benchmark, to use with OpenSSL
#

Prepare()
{
  echo "---[ Prepare"

  cd "${from}/"

  directories=$(find ./ -type d)

  time								\
    (for directory in ${directories} ; do
      mkdir "${to}/${directory}"
    done) >andrew-benchmark.log
}

Copy()
{
  echo "---[ Copy"

  cd "${from}/"

  files=$(find ./ -type f && find ./ -type l)

  time								\
    (for file in ${files} ; do
      cp -P "${file}" "${to}/${file}"
      if [ ${?} -ne 0 ] ; then exit 1 ; fi
    done) >andrew-benchmark.log
}

List()
{
  echo "---[ List"

  time								\
    (ls -Rla "${to}") >andrew-benchmark.log
}

Search()
{
  echo "---[ Search"

  time								\
    (grep -R "teton" "${to}") >andrew-benchmark.log
}

Compile()
{
  echo "---[ Compile"

  cd "${to}/"

  time								\
    (./configure && make) >andrew-benchmark.log
#    (make initialize && make && make build install) >andrew-benchmark.log
#    (./config && sed -ri "s/^AR=(.*)$/AR=echo \1/" Makefile && sed -ri "s/^RANLIB=(.*)$/RANLIB=echo \1/" Makefile && make) >andrew-benchmark.log
}

if [ ${#} -ne 2 ] ; then
  echo "[usage] andrew-benchmark.sh {from} {to}"
  exit 0
fi

cd "${1}"
from="${PWD}"
cd "${OLDPWD}"

cd "${2}"
to="${PWD}"
cd "${OLDPWD}"

Prepare
Copy
List
Search
Compile
