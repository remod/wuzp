# Finds the Blitz++ library (https://github.com/blitzpp/blitz).
#
# Blitz++ ships neither a CMake package nor a consistent pkg-config file across distributions, so it is searched for
# directly. Set Blitz_ROOT to point at a custom installation.
#
# Imported target:
#   Blitz::Blitz
#
# Result variables:
#   Blitz_FOUND
#   Blitz_INCLUDE_DIR
#   Blitz_LIBRARY

find_path(Blitz_INCLUDE_DIR NAMES blitz/array.h)
find_library(Blitz_LIBRARY NAMES blitz)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blitz REQUIRED_VARS Blitz_LIBRARY Blitz_INCLUDE_DIR)

if(Blitz_FOUND AND NOT TARGET Blitz::Blitz)
    add_library(Blitz::Blitz UNKNOWN IMPORTED)
    set_target_properties(Blitz::Blitz PROPERTIES
        IMPORTED_LOCATION "${Blitz_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Blitz_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(Blitz_INCLUDE_DIR Blitz_LIBRARY)
