set(SERIALIZE_LIB
    ${INFINIT_LIBRARIES} comet lune #Fuck
)

simple_test(NAME all_archives PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME BinaryArchive PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME JSONArchive PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME Base64Archive PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME Pointer PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME footprint PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME version_change PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME Serializable PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME construct PASS_REGEX "tests done." LIB ${SERIALIZE_LIB})
simple_test(NAME extract PASS_REGEX "tests done."  LIB ${SERIALIZE_LIB})
simple_test(NAME insert PASS_REGEX "tests done."  LIB ${SERIALIZE_LIB})
