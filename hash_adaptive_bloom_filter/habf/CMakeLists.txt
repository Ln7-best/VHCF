add_executable(main "main_fat_tree.cpp" "habf.h" "utils.h" "utils.cpp")
target_link_libraries(main "util")
set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
