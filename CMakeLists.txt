cmake_minimum_required(VERSION 3.14)
project(formula_teacher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Находим и подключаем LibTorch
find_package(Torch REQUIRED)

# Добавляем поддержку Vala и GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(VALA REQUIRED IMPORTED_TARGET vala)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

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

# Настраиваем компиляцию Vala кода для основного GUI
include(${CMAKE_SOURCE_DIR}/cmake/ValaPrecompile.cmake)

vala_precompile(GUI_VALA_C
    src/gui/main.vala
    src/gui/window.vala
    src/gui/model_manager.vala
    src/gui/task_processor.vala
PACKAGES
    gtk4
OPTIONS
    --target-glib=2.50
)

# Настраиваем компиляцию Vala кода для GUI обучения
vala_precompile(TRAINER_VALA_C
    src/trainer/main_trainer.vala
    src/trainer/trainer_window.vala
    src/trainer/training_manager.vala
PACKAGES
    gtk4
OPTIONS
    --target-glib=2.50
)

# Создаем исполняемый файл для основного GUI
add_executable(formula-teacher-gui ${GUI_VALA_C})
target_link_libraries(formula-teacher-gui formula_core PkgConfig::GTK4)

# Создаем исполняемый файл для GUI обучения
add_executable(formula-teacher-trainer ${TRAINER_VALA_C})
target_link_libraries(formula-teacher-trainer formula_core PkgConfig::GTK4)

# Установка
install(TARGETS formula-teacher-gui formula-teacher-trainer train DESTINATION bin)

# Копирование UI файлов
configure_file(src/gui/window.ui ${CMAKE_BINARY_DIR}/window.ui COPYONLY)
configure_file(src/trainer/trainer_window.ui ${CMAKE_BINARY_DIR}/trainer_window.ui COPYONLY)

# Предупреждения для C++
if(MSVC)
  target_compile_options(formula_core PRIVATE /W4)
else()
  target_compile_options(formula_core PRIVATE -Wall -Wextra -pedantic)
endif()
