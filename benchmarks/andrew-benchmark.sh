#! /bin/bash

#
# andrew benchmark, to use with OpenSSL
#

Prepare()
{
  echo "---[ Prepare"

  directories=$(find "${from}" -type d)

  time								\
    (for directory in ${directories} ; do
      mkdir "${to}/${directory}"
    done) >andrew-benchmark.log
}

Copy()
{
  echo "---[ Copy"

  files=$(find "${from}" -type f)

  time								\
    (for file in ${files} ; do
      cp "${file}" "${to}/${file}"
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

  cd "${to}/$(basename ${from})/"

  time								\
    (./config && sed -ri "s/^AR=(.*)$/AR=echo \1/" Makefile && sed -ri "s/^RANLIB=(.*)$/RANLIB=echo \1/" Makefile && make) >andrew-benchmark.log
}

if [ ${#} -ne 2 ] ; then
  echo "[usage] andrew-benchmark.sh {from} {to}"
  exit 0
fi

from=${1}
to=${2}

Prepare
Copy
List
Search
Compile
