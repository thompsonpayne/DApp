cmake_minimum_required(VERSION 3.16)
project(LogisticsApp LANGUAGES CXX)

# 1. Use C++ 17 or 20
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC specific: Add /Zc:__cplusplus flag to ensure __cplusplus is set correctly
if(MSVC)
  add_compile_options(/Zc:__cplusplus)
endif()
# Qt build helpers (moc/uic/rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find Qt6 - Use MSVC version for Visual Studio
# Set CMAKE_PREFIX_PATH to explicitly use MSVC Qt6
set(Qt6_DIR "C:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6" CACHE PATH "Qt6 directory")
list(PREPEND CMAKE_PREFIX_PATH "C:/Qt/6.10.2/msvc2022_64/lib/cmake")

if(DEFINED ENV{QT_DIR})
  list(PREPEND CMAKE_PREFIX_PATH "$ENV{QT_DIR}/lib/cmake")
  message(STATUS "Using QT_DIR from environment: $ENV{QT_DIR}")
else()
  message(STATUS "Using Qt6 from: C:/Qt/6.10.2/msvc2022_64")
endif()

message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

find_package(Qt6 COMPONENTS Widgets Sql)
if(NOT Qt6_FOUND)
  message(FATAL_ERROR 
    "Qt6 not found!\n"
    "Please either:\n"
    "1. Set QT_DIR environment variable: set QT_DIR=C:\\Qt\\6.6.0\\msvc2022_64\n"
    "2. Or pass CMAKE_PREFIX_PATH: cmake -B build -DCMAKE_PREFIX_PATH=C:\\Qt\\6.6.0\\msvc2022_64\\lib\\cmake\n"
    "Download Qt6 from: https://www.qt.io/download"
  )
endif()
message(STATUS "Qt6 found: ${Qt6_DIR}")

if (COMMAND qt_standard_project_setup)
  qt_standard_project_setup()
endif()

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(json)

# 3. Define your executable
if (COMMAND qt_add_executable)
  qt_add_executable(app
    src/main.cpp
    src/main_window.cpp
    src/main_window.h
    src/home_screen.cpp
    src/home_screen.h
    src/detail_screen.cpp
    src/detail_screen.h
    src/login_screen.h
    src/login_screen.cpp
    src/order_form_dialog.cpp
    src/order_form_dialog.h
    src/database.cpp
    src/database.h
    src/models.h
    resources/migrations.qrc
  )
else()
  add_executable(app
    src/main.cpp
    src/main_window.cpp
    src/main_window.h
    src/home_screen.cpp
    src/home_screen.h
    src/detail_screen.cpp
    src/detail_screen.h
    src/login_screen.h
    src/login_screen.cpp
    src/order_form_dialog.cpp
    src/order_form_dialog.h
    src/database.cpp
    src/database.h
    src/models.h
    resources/migrations.qrc
  )
endif()

target_link_libraries(app PRIVATE Qt6::Widgets Qt6::Sql nlohmann_json::nlohmann_json)

# Automatic Qt DLL deployment for Windows
if(WIN32 AND Qt6_FOUND)
  get_target_property(QT_BIN_DIR Qt6::Core IMPORTED_LOCATION)
  get_filename_component(QT_BIN_DIR "${QT_BIN_DIR}" DIRECTORY)

  # Add post-build step to run windeployqt
  add_custom_command(TARGET app POST_BUILD
    COMMAND "${QT_BIN_DIR}/windeployqt.exe" 
            --no-compiler-runtime 
            "$<TARGET_FILE:app>"
    COMMENT "Running windeployqt to deploy Qt DLLs..."
    VERBATIM
  )
endif()
