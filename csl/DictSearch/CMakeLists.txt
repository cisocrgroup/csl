add_library( DictSearch DictSearch.cxx )

add_executable( dictSearch_demo exec_dictSearch_demo.cxx )
target_link_libraries( dictSearch_demo csl )

add_subdirectory( Test )
