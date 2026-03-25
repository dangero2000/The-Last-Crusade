cmake_minimum_required(VERSION 3.14)
project(LastCrusade CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Dependencies ────────────────────────────────────────────────────────────

# SDL2 – window creation and keyboard input
find_package(SDL2 REQUIRED)

# miniaudio – single-header cross-platform audio (WAV, MP3, 3D spatial)
# Downloaded automatically by CMake on first configure.
include(FetchContent)
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
    ${SDL2_INCLUDE_DIRS}
    ${miniaudio_SOURCE_DIR}   # provides miniaudio.h
)

target_link_libraries(LastCrusade PRIVATE ${SDL2_LIBRARIES})

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

    # Resolve SDL2's library directory so BundleFixup.cmake can find it.
    # Handles both the modern SDL2::SDL2 imported target and the legacy
    # SDL2_LIBRARIES / SDL2_LIBRARY_DIRS variables.
    if(TARGET SDL2::SDL2)
        get_target_property(_sdl2_loc SDL2::SDL2 IMPORTED_LOCATION)
        get_filename_component(SDL2_LIBDIR "${_sdl2_loc}" DIRECTORY)
    elseif(SDL2_LIBRARY_DIRS)
        set(SDL2_LIBDIR "${SDL2_LIBRARY_DIRS}")
    else()
        # Last-resort: derive from SDL2_LIBRARIES list
        list(GET SDL2_LIBRARIES 0 _sdl2_first)
        get_filename_component(SDL2_LIBDIR "${_sdl2_first}" DIRECTORY)
    endif()

    # Copy all required dylibs (SDL2, etc.) into Contents/Frameworks/ and
    # rewrite their install names so the .app works without Homebrew.
    add_custom_command(TARGET LastCrusade POST_BUILD
        COMMAND ${CMAKE_COMMAND}
            "-DBUNDLE=$<TARGET_BUNDLE_DIR:LastCrusade>"
            "-DSDL2_DIRS=${SDL2_LIBDIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BundleFixup.cmake"
        COMMENT "Bundling dylibs into LastCrusade.app"
        VERBATIM)

    # Assets go into Contents/Resources/ inside the bundle.
    # SDL_GetBasePath() in main.cpp returns this path at runtime, so all
    # relative asset paths in the engine resolve correctly.
    set(ASSET_DEST "$<TARGET_BUNDLE_CONTENT_DIR:LastCrusade>/Resources")
else()
    # On Linux / Windows assets sit next to the executable.
    # SDL_GetBasePath() returns the executable directory there.
    set(ASSET_DEST "$<TARGET_FILE_DIR:LastCrusade>")
endif()

# ─── Copy game assets at build time ──────────────────────────────────────────

set(BUILD_ASSETS_DIR "${CMAKE_SOURCE_DIR}/Build")

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
