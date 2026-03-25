cmake_minimum_required(VERSION 3.14)
project(LastCrusade CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Dependencies ────────────────────────────────────────────────────────────

include(FetchContent)

# SDL2 – window creation and keyboard input
# Downloaded automatically by CMake on first configure.
FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SDL2)

# miniaudio – single-header cross-platform audio (WAV, MP3, 3D spatial)
# Downloaded automatically by CMake on first configure.
FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(miniaudio)

# ─── Executable ──────────────────────────────────────────────────────────────

add_executable(LastCrusade Engine/main.cpp)

target_include_directories(LastCrusade PRIVATE
    Engine/
    ${miniaudio_SOURCE_DIR}   # provides miniaudio.h
)

target_link_libraries(LastCrusade PRIVATE SDL2::SDL2 SDL2::SDL2main)

# Platform-specific libraries required by miniaudio's backends
if(APPLE)
    target_link_libraries(LastCrusade PRIVATE
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework CoreFoundation")
elseif(UNIX)
    target_link_libraries(LastCrusade PRIVATE pthread dl m)
endif()

# ─── macOS .app bundle ───────────────────────────────────────────────────────

if(APPLE)
    set_target_properties(LastCrusade PROPERTIES
        MACOSX_BUNDLE                      TRUE
        MACOSX_BUNDLE_BUNDLE_NAME          "The Last Crusade"
        MACOSX_BUNDLE_BUNDLE_VERSION       "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        MACOSX_BUNDLE_GUI_IDENTIFIER       "com.thelastcrusade.game"
    )

    # Resolve SDL2's library directory so BundleFixup.cmake can find the dylib.
    get_target_property(_sdl2_loc SDL2::SDL2 LOCATION)
    get_filename_component(SDL2_LIBDIR "${_sdl2_loc}" DIRECTORY)

    # Copy all required dylibs (SDL2, etc.) into Contents/Frameworks/ and
    # rewrite their install names so the .app works without Homebrew.
    add_custom_command(TARGET LastCrusade POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            "-DBUNDLE=$<TARGET_BUNDLE_DIR:LastCrusade>"
            "-DSDL2_DIRS=${SDL2_LIBDIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BundleFixup.cmake"
        COMMENT "Bundling dylibs into LastCrusade.app"
        VERBATIM)

    # Re-sign after fixup_bundle modifies the binary (which invalidates
    # any existing signature). Ad-hoc signing satisfies Gatekeeper.
    add_custom_command(TARGET LastCrusade POST_BUILD
        COMMAND codesign --force --deep --sign - "$<TARGET_BUNDLE_DIR:LastCrusade>"
        COMMENT "Code signing LastCrusade.app"
        VERBATIM)

    # Assets go into Contents/Resources/ inside the bundle.
    # SDL_GetBasePath() + ../Resources in main.cpp resolves here at runtime.
    set(ASSET_DEST "$<TARGET_BUNDLE_CONTENT_DIR:LastCrusade>/Resources")
else()
    # On Linux / Windows assets sit next to the executable.
    # SDL_GetBasePath() returns the executable directory there.
    set(ASSET_DEST "$<TARGET_FILE_DIR:LastCrusade>")
endif()

# ─── Copy game assets at build time ──────────────────────────────────────────

set(BUILD_ASSETS_DIR "${CMAKE_SOURCE_DIR}/Build")

# Ensure the Resources directory exists on macOS before copying into it
if(APPLE)
    add_custom_command(TARGET LastCrusade POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${ASSET_DEST}"
        COMMENT "Creating Resources directory"
        VERBATIM)
endif()

# Map files
file(GLOB MAP_FILES "${BUILD_ASSETS_DIR}/*.map")
foreach(MAP_FILE ${MAP_FILES})
    add_custom_command(TARGET LastCrusade POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MAP_FILE}" "${ASSET_DEST}/"
        VERBATIM)
endforeach()

# Sound and number directories
foreach(DIR forest.sounds graveyard.sounds castle.sounds sounds numbers)
    if(EXISTS "${BUILD_ASSETS_DIR}/${DIR}")
        add_custom_command(TARGET LastCrusade POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${BUILD_ASSETS_DIR}/${DIR}"
                "${ASSET_DEST}/${DIR}"
            VERBATIM)
    endif()
endforeach()

# ─── Install ─────────────────────────────────────────────────────────────────

install(TARGETS LastCrusade
    BUNDLE  DESTINATION .        # installs the .app on macOS
    RUNTIME DESTINATION bin)     # installs the executable on Linux/Windows
