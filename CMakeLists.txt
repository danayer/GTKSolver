cmake_minimum_required(VERSION 3.14)
project(formula_teacher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим и подключаем LibTorch
find_package(Torch REQUIRED)

# Добавляем поддержку GTK4 через pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# Используем наш собственный FindVala.cmake вместо pkg-config
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
find_package(Vala REQUIRED)

# Основные исходники ядра на C++
set(CORE_SOURCES
    src/core/model.cpp
    src/core/trainer.cpp
    src/core/tokenizer.cpp
    src/core/inference.cpp
)

# Заголовочные файлы ядра
set(CORE_HEADERS
    src/core/model.h
    src/core/trainer.h
    src/core/tokenizer.h
    src/core/inference.h
)

# Создаем библиотеку ядра
add_library(formula_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_link_libraries(formula_core ${TORCH_LIBRARIES})
target_include_directories(formula_core PRIVATE ${TORCH_INCLUDE_DIRS})

# Исполняемый файл для обучения через командную строку
add_executable(train src/core/train_main.cpp)
target_link_libraries(train formula_core ${TORCH_LIBRARIES})

# Компилируем GResources
find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources)

# Компилируем GResource для GUI
set(GRESOURCE_GUI_XML ${CMAKE_SOURCE_DIR}/src/gui/resources.xml)
set(GRESOURCE_GUI_C ${CMAKE_BINARY_DIR}/resources_gui.c)

add_custom_command(
    OUTPUT ${GRESOURCE_GUI_C}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS --target=${GRESOURCE_GUI_C} --sourcedir=${CMAKE_SOURCE_DIR}/src/gui --generate-source ${GRESOURCE_GUI_XML}
    DEPENDS ${GRESOURCE_GUI_XML} ${CMAKE_SOURCE_DIR}/src/gui/window.ui
    COMMENT "Compiling GUI GResource"
)

# Компилируем GResource для Trainer
set(GRESOURCE_TRAINER_XML ${CMAKE_SOURCE_DIR}/src/trainer/resources.xml)
set(GRESOURCE_TRAINER_C ${CMAKE_BINARY_DIR}/resources_trainer.c)

add_custom_command(
    OUTPUT ${GRESOURCE_TRAINER_C}
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS --target=${GRESOURCE_TRAINER_C} --sourcedir=${CMAKE_SOURCE_DIR}/src/trainer --generate-source ${GRESOURCE_TRAINER_XML}
    DEPENDS ${GRESOURCE_TRAINER_XML} ${CMAKE_SOURCE_DIR}/src/trainer/trainer_window.ui
    COMMENT "Compiling Trainer GResource"
)

# Enable C language for the project
enable_language(C)

# Create simple GUI application in pure C
add_executable(formula-teacher-gui ${CMAKE_SOURCE_DIR}/src/gui/simple_app.c)
target_link_libraries(formula-teacher-gui PRIVATE PkgConfig::GTK4)

# Create simple Trainer application in pure C
add_executable(formula-teacher-trainer ${CMAKE_SOURCE_DIR}/src/trainer/simple_trainer.c)
target_link_libraries(formula-teacher-trainer PRIVATE PkgConfig::GTK4)

# Настраиваем компиляцию Vala кода для основного GUI
include(${CMAKE_SOURCE_DIR}/cmake/ValaPrecompile.cmake)

# First compile the Vala code
# vala_precompile(GUI_VALA_C gui_vala_target
#    src/gui/main.vala
#    src/gui/window.vala
#    src/gui/model_manager.vala
#    src/gui/task_processor.vala
# PACKAGES
#    gtk4
#    gio-2.0
# OPTIONS
#    --target-glib=2.50
#    --gresources=${CMAKE_SOURCE_DIR}/src/gui/resources.xml
# DIRECTORY
#    ${CMAKE_CURRENT_BINARY_DIR}/vala-generated
# CUSTOM_VAPIS
# )

# vala_precompile(TRAINER_VALA_C trainer_vala_target
#    src/trainer/main_trainer.vala
#    src/trainer/trainer_window.vala
#    src/trainer/training_manager.vala
# PACKAGES
#    gtk4
#    gio-2.0
# OPTIONS
#    --target-glib=2.50
#    --gresources=${CMAKE_SOURCE_DIR}/src/trainer/resources.xml
# DIRECTORY
#    ${CMAKE_CURRENT_BINARY_DIR}/vala-generated
# CUSTOM_VAPIS
# )

# Make sure the generated files are included in the build
include_directories(${CMAKE_CURRENT_BINARY_DIR}/vala-generated)

# Create the GUI executable without the main_wrapper.c since Vala already has a main
# add_executable(formula-teacher-gui ${GUI_VALA_C} ${GRESOURCE_GUI_C})
# add_dependencies(formula-teacher-gui gui_vala_target)
# target_link_libraries(formula-teacher-gui PRIVATE formula_core PkgConfig::GTK4)

# Create the Trainer executable
# add_executable(formula-teacher-trainer ${TRAINER_VALA_C} ${GRESOURCE_TRAINER_C})
# add_dependencies(formula-teacher-trainer trainer_vala_target)
# target_link_libraries(formula-teacher-trainer PRIVATE formula_core PkgConfig::GTK4)

# Установка
install(TARGETS formula-teacher-gui formula-teacher-trainer train DESTINATION bin)

# Installation rules for AppImage and system installation
install(TARGETS formula-teacher-gui RUNTIME DESTINATION bin RENAME gtksolver-gui)
install(TARGETS formula-teacher-trainer RUNTIME DESTINATION bin RENAME gtksolver-trainer)
install(TARGETS train RUNTIME DESTINATION bin RENAME gtksolver-trainer-cli)

# Install icons
install(FILES icons/gtksolver.svg
        DESTINATION share/icons/hicolor/scalable/apps)
install(FILES icons/gtksolver-trainer.svg
        DESTINATION share/icons/hicolor/scalable/apps)

# Install desktop files
install(FILES desktop/gtksolver-gui.desktop
        DESTINATION share/applications)
install(FILES desktop/gtksolver-trainer.desktop
        DESTINATION share/applications)

# Install AppStream metadata
install(FILES metainfo/gtksolver.appdata.xml
        DESTINATION share/metainfo)

# Предупреждения для C++
if(MSVC)
  target_compile_options(formula_core PRIVATE /W4)
else()
  target_compile_options(formula_core PRIVATE -Wall -Wextra -pedantic)
endif()