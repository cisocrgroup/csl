add_executable( compileMDString compileMDString.cxx )
target_link_libraries( compileMDString csl )

add_executable( lookupMDString lookupMDString.cxx )
target_link_libraries( lookupMDString csl )
