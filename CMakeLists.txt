cmake_minimum_required(VERSION 3.15)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11 CACHE STRING "Build for 10.1")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE Type Unspecified; picking Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

project(clap-saw-demo VERSION 0.9.0 LANGUAGES C CXX)


set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)


# use asan as an option (currently mac only)
option(USE_SANITIZER "Build and link with ASAN" FALSE)
option(CSD_INCLUDE_GUI "Include a GUI in ClapSawDemo" TRUE)

# Copy on mac (could expand to other platforms)
option(COPY_AFTER_BUILD "Copy the clap to ~/Library on MACOS, ~/.clap on linux" FALSE)

add_subdirectory(libs/readerwriterqueue EXCLUDE_FROM_ALL)
add_subdirectory(libs/clap EXCLUDE_FROM_ALL)
add_subdirectory(libs/clap-helpers EXCLUDE_FROM_ALL)
set(BUILD_TESTS OFF CACHE BOOL "Disable madronalib tests")
add_subdirectory(libs/madronalib EXCLUDE_FROM_ALL)
if (${CSD_INCLUDE_GUI})
    message(STATUS "Including MLVG for GUI")
    set(BUILD_TESTS OFF CACHE BOOL "Disable MLVG tests")
    add_subdirectory(libs/mlvg EXCLUDE_FROM_ALL)
    # create embedded font resources
    create_resources(libs/mlvg/examples/app/resources build/resources/clap-saw-demo)
else()
    message(STATUS "GUI Bypassed. headless CSD generated")
endif()
if (APPLE AND ${CSD_INCLUDE_GUI})
  target_compile_options(mlvg PRIVATE -Wno-deprecated-declarations)
endif()


add_library(${PROJECT_NAME} MODULE
        src/clap-saw-demo.cpp
        src/clap-saw-demo-entry.cpp  # New simple entry file
)

if(CSD_INCLUDE_GUI)
  target_sources(${PROJECT_NAME} PRIVATE
    src/clap-saw-demo-gui.cpp
    build/resources/clap-saw-demo/resources.c
  )

  # Set the generated resource files to compile as C++
  set_source_files_properties(build/resources/clap-saw-demo/resources.c PROPERTIES COMPILE_FLAGS "-x c++")
endif()
target_link_libraries(${PROJECT_NAME} readerwriterqueue clap clap-helpers madronalib mlvg)

# Platform-specific libraries for MLVG
if(APPLE AND CSD_INCLUDE_GUI)
  find_library(METAL_FRAMEWORK Metal)
  find_library(METALKIT_FRAMEWORK MetalKit)
  if(METAL_FRAMEWORK AND METALKIT_FRAMEWORK)
    target_link_libraries(${PROJECT_NAME} ${METAL_FRAMEWORK} ${METALKIT_FRAMEWORK})
  endif()
endif()

# Add include directories
target_include_directories(${PROJECT_NAME} PRIVATE
  libs/clap/include
  libs/clap-helpers/include
  libs/madronalib/include
  libs/madronalib/source/app
  libs/madronalib/source/DSP
  libs/madronalib/source/matrix
  libs/madronalib/external
  libs/madronalib/external/cJSON
  libs/madronalib/external/aes256
  libs/madronalib/external/ffft
  libs/madronalib/external/oscpack
  libs/madronalib/external/rtaudio
  libs/madronalib/external/rtmidi
  libs/madronalib/external/sse2neon
  libs/madronalib/external/utf
  libs/mlvg/include
  libs/mlvg/source
  libs/mlvg/source/common
  libs/mlvg/source/widgets
  libs/mlvg/source/external
  libs/mlvg/source/external/nanovg/src
  libs/mlvg/source/external/MetalNanoVG/src
  libs/mlvg/source/external/nanosvg/src
  libs/mlvg/source/external/cJSON
)

# Testing infrastructure
enable_testing()

# Add clap-validator testing target
option(CSD_ENABLE_TESTING "Enable CLAP validation testing" TRUE)

if (${CSD_ENABLE_TESTING})
  # Check if Rust/Cargo is available for clap-validator
  find_program(CARGO_EXECUTABLE cargo)
  if (CARGO_EXECUTABLE)
    message(STATUS "Found Cargo at ${CARGO_EXECUTABLE} - enabling CLAP validation tests")

    # Build clap-validator
    add_custom_target(build-clap-validator
      COMMAND ${CARGO_EXECUTABLE} build --release
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/clap-validator
      COMMENT "Building clap-validator"
    )

    # Test target that runs clap-validator on our plugin
    add_custom_target(test-clap-plugin
      COMMAND ${CARGO_EXECUTABLE} run --release -- validate $<TARGET_FILE:${PROJECT_NAME}> --only-failed
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/clap-validator
      DEPENDS ${PROJECT_NAME} build-clap-validator
      COMMENT "Running CLAP specification validation on ${PROJECT_NAME}"
    )

    # Add to CTest
    add_test(
      NAME clap-specification-compliance
      COMMAND ${CARGO_EXECUTABLE} run --release -- validate $<TARGET_FILE:${PROJECT_NAME}> --only-failed
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/clap-validator
    )

    # Convenience test script targets
    if(WIN32)
      add_custom_target(test-plugin
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-plugin.bat
        COMMENT "Running full plugin test suite (Windows)"
      )
      add_custom_target(debug-plugin
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/debug-plugin.bat
        COMMENT "Running comprehensive plugin debugging suite (Windows)"
      )
    else()
      add_custom_target(test-plugin
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/test-plugin.sh
        COMMENT "Running full plugin test suite (Unix)"
      )
      add_custom_target(debug-plugin
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/debug-plugin.sh
        COMMENT "Running comprehensive plugin debugging suite (Unix)"
      )
    endif()

    # CLAP debugging tools integration
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tools/clap-info/CMakeLists.txt)
      message(STATUS "Found clap-info - adding debugging targets")
      add_custom_target(build-clap-info
        COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/tools/clap-info
                ${CMAKE_COMMAND} -Bbuild -DCMAKE_BUILD_TYPE=Release
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_SOURCE_DIR}/tools/clap-info/build --config Release
        COMMENT "Building clap-info debugging tool"
      )

      add_custom_target(inspect-plugin
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/clap-info/build/clap-info $<TARGET_FILE:${PROJECT_NAME}>
        DEPENDS ${PROJECT_NAME} build-clap-info
        COMMENT "Inspecting plugin with clap-info"
      )
    endif()

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tools/clap-host/CMakeLists.txt)
      message(STATUS "Found clap-host - adding host testing targets")
      # Note: clap-host requires Qt6 and has complex build requirements
      # Use scripts/debug-plugin.sh for full clap-host integration
    endif()

  else()
    message(WARNING "Cargo not found - CLAP validation tests disabled. Install Rust from https://rustup.rs/")
  endif()
endif()

if (${CSD_INCLUDE_GUI})
    message(STATUS "Adding MLVG GUI support")
    target_link_libraries(${PROJECT_NAME} mlvg)
    target_compile_definitions(${PROJECT_NAME} PRIVATE HAS_GUI=1)
else()
    message(STATUS "Skipping GUI generation")
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            BUNDLE True
            BUNDLE_EXTENSION clap
            MACOSX_BUNDLE_GUI_IDENTIFIER org.surge-synth-team.${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
            MACOSX_BUNDLE_BUNDLE_VERSION "0.1"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}.plist.in
            )
    target_link_libraries(${PROJECT_NAME} "-framework CoreFoundation" "-framework AppKit" "-framework CoreGraphics")

    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_MAC=1)
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Werror
            $<$<BOOL:${USE_SANITIZER}>:-fsanitize=address>
            $<$<BOOL:${USE_SANITIZER}>:-fsanitize=undefined>
            )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<BOOL:${USE_SANITIZER}>:-fsanitize=address>
            $<$<BOOL:${USE_SANITIZER}>:-fsanitize=undefined>
            )

    if (${COPY_AFTER_BUILD})
        message(STATUS "Will copy plugin after every build" )
        set(products_folder ${CMAKE_BINARY_DIR})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Installing ${products_folder}/${PROJECT_NAME}.clap to ~/Library/Audio/Plug-Ins/CLAP/"
                COMMAND ${CMAKE_COMMAND} -E make_directory "~/Library/Audio/Plug-Ins/CLAP"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${products_folder}/${PROJECT_NAME}.clap" "~/Library/Audio/Plug-Ins/CLAP/${PROJECT_NAME}.clap"
                )
    endif()
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_LINUX=1)
    if (${CSD_INCLUDE_GUI})
        target_sources(${PROJECT_NAME} PRIVATE src/linux-vstgui-adapter.cpp)
    endif()
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".clap" PREFIX "")
    if (${COPY_AFTER_BUILD})
        message(STATUS "Will copy plugin after every build" )
        set(products_folder ${CMAKE_BINARY_DIR})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Installing ${products_folder}/${PROJECT_NAME}.clap to ~/.clap"
                COMMAND ${CMAKE_COMMAND} -E make_directory "~/.clap"
                COMMAND ${CMAKE_COMMAND} -E copy "${products_folder}/${PROJECT_NAME}.clap" "~/.clap"
                )
    endif()

else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE IS_WIN=1)
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".clap" PREFIX "")
endif()
