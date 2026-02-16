cmake_minimum_required(VERSION 3.16)
project(LogisticsApp LANGUAGES CXX)

# 1. Use C++ 17 or 20
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt build helpers (moc/uic/rcc)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

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
    src/order_form_dialog.cpp
    src/order_form_dialog.h
    src/database.cpp
    src/database.h
    src/models.h
    resources/migrations.qrc
  )
endif()

target_link_libraries(app PRIVATE Qt6::Widgets Qt6::Sql nlohmann_json::nlohmann_json)
