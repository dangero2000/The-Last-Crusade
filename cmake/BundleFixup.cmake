# Invoked as: cmake -DBUNDLE=<path> -DSDL2_DIRS=<path> -P BundleFixup.cmake
# Copies all required dylibs into Contents/Frameworks/ and rewrites their
# install names so the .app is fully self-contained.
include(BundleUtilities)
fixup_bundle("${BUNDLE}" "" "${SDL2_DIRS}")
