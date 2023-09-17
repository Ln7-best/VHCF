add_library(bloom_filter STATIC "bloom_filter.h" "hashutil.h")
add_executable(main "bloom_filter.h" "hashutil.h" "main.cpp" "utils.h" "utils.cpp")
set_target_properties(bloom_filter PROPERTIES LINKER_LANGUAGE CXX)