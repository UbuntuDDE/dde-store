find_package(Snapd QUIET)
if (Snapd_FOUND)
    add_subdirectory(snap)
endif()

pkg_check_modules(Flatpak IMPORTED_TARGET flatpak>=0.11.8)
if (Flatpak_FOUND)
    add_subdirectory(flatpak)
endif()