add_library(hash_table STATIC "hash_table.h" "murmur3.h")
set_target_properties(hash_table PROPERTIES LINKER_LANGUAGE CXX)