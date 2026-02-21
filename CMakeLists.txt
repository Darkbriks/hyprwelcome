cmake_minimum_required(VERSION 3.20)
project(hyprwelcome VERSION 0.3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

include(FetchContent)
FetchContent_Declare(
        tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)

# ═══════════════════════════════════════════════════════════════════
# HYPRWELCOME
# ═══════════════════════════════════════════════════════════════════

set(SOURCES
        src/panel/main.cpp
        src/panel/MainWindow.cpp
        src/panel/ActionButton.cpp
        src/panel/ConfigLoader.cpp
        src/panel/ThemeManager.cpp
)

set(HEADERS
        src/panel/MainWindow.hpp
        src/panel/ActionButton.hpp
        src/panel/ConfigLoader.hpp
        src/panel/ThemeManager.hpp
)

qt_add_executable(hyprwelcome ${SOURCES} ${HEADERS})

target_link_libraries(hyprwelcome PRIVATE
        Qt6::Core
        Qt6::Widgets
        tomlplusplus::tomlplusplus
)

target_compile_definitions(hyprwelcome PRIVATE
        APP_NAME="hyprwelcome"
        APP_VERSION="0.3.0"
)

target_compile_options(hyprwelcome PRIVATE
        $<$<CONFIG:Release>:-O2>
        -Wall -Wextra -Wpedantic
)

# ═══════════════════════════════════════════════════════════════════
# HYPRTODO
# ═══════════════════════════════════════════════════════════════════

set(TODO_SOURCES
        src/todo/main.cpp
        src/todo/TodoItem.cpp
        src/todo/TodoStore.cpp
        src/todo/TodoRow.cpp
        src/todo/TodoWindow.cpp
        src/panel/ConfigLoader.cpp
        src/panel/ThemeManager.cpp
)

qt_add_executable(hyprtodo ${TODO_SOURCES})

target_link_libraries(hyprtodo PRIVATE
        Qt6::Core
        Qt6::Widgets
        tomlplusplus::tomlplusplus
)

target_compile_definitions(hyprtodo PRIVATE
        APP_NAME="hyprtodo"
        APP_VERSION="0.1.0"
)

target_compile_options(hyprtodo PRIVATE
        $<$<CONFIG:Release>:-O2>
        -Wall -Wextra -Wpedantic
)

# ═══════════════════════════════════════════════════════════════════
# INSTALLATION
# ═══════════════════════════════════════════════════════════════════

include(GNUInstallDirs)

install(TARGETS hyprwelcome hyprtodo
        RUNTIME DESTINATION "$ENV{HOME}/.local/bin"
)

install(CODE "
    set(DEST \"\$ENV{HOME}/.config/hyprwelcome\")
    if(NOT EXISTS \"\${DEST}/config.toml\")
        file(MAKE_DIRECTORY \"\${DEST}\")
        file(COPY \"${CMAKE_SOURCE_DIR}/.config/hyprwelcome/config.toml\"
             DESTINATION \"\${DEST}\")
        message(STATUS \"Config installée dans \${DEST}\")
    else()
        message(STATUS \"Config existante conservée dans \${DEST}\")
    endif()
")

install(CODE "
    set(THEMES_SRC \"${CMAKE_SOURCE_DIR}/themes\")
    set(THEMES_DEST \"\$ENV{HOME}/.config/hyprwelcome/themes\")
    file(MAKE_DIRECTORY \"\${THEMES_DEST}\")

    file(GLOB THEME_DIRS \"\${THEMES_SRC}/*\")
    foreach(THEME \${THEME_DIRS})
        get_filename_component(THEME_NAME \"\${THEME}\" NAME)
        if(NOT EXISTS \"\${THEMES_DEST}/\${THEME_NAME}\")
            file(COPY \"\${THEME}\" DESTINATION \"\${THEMES_DEST}\")
            message(STATUS \"Thème installé : \${THEME_NAME}\")
        else()
            message(STATUS \"Thème existant conservé : \${THEME_NAME}\")
        endif()
    endforeach()
")