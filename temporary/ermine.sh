ERMINE=$(dirname ${0})"/Ermine.i386"

if [ ${#} -ne 1 ] ; then
  echo "[usage] ermine.sh {directory}"
  exit 0
fi

directory=${1}

files=$(find ${directory}/ -type f -executable | grep -v "^.*static$")

for f in ${files} ; do
  static="${f}.static"

  if [ -e ${static} ] ; then
    continue
  fi

  echo "${f}"

  ${ERMINE} --output ${f}.static ${f}
done
