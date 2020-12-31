find_package(Snapd QUIET)
if (Snapd_FOUND)
    add_subdirectory(snap)
endif()
