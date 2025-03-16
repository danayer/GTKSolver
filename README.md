# 🧠 GTKSolver - нейросеть для решения задач с формулами

## 📚 Описание проекта

**GTKSolver** - это интеллектуальная система, использующая нейронные сети для обучения на текстовых данных учебника, содержащих математические формулы, и последующего решения задач по этой тематике.

Система состоит из трех основных компонентов:
1. **🔬 Нейросетевое ядро** - написано на C++ с использованием LibTorch (библиотека PyTorch для C++)
2. **🖥️ Графический интерфейс решения задач** - разработан на языке C с использованием GTK4
3. **🎓 Графический интерфейс обучения** - отдельное приложение на C + GTK4 для обучения модели

## 📋 Требования для установки

### 🔧 Зависимости
- C++17 или выше
- CMake 3.14 или выше
- LibTorch 1.9.0 или выше
- GTK4
- Build Essential
- pkg-config

### 🔧 Установка зависимостей

#### 🐧 Ubuntu/Debian
```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install build-essential cmake git pkg-config

# Установка GTK4
sudo apt-get install libgtk-4-dev

# Установка LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip
```

#### 🐧 Alt Linux
```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install make-gcc cmake git pkg-config

# Установка GTK4
sudo apt-get install libgtk4-devel

# Установка LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip
```

#### 🔒 Astra Linux
```bash
# Установка базовых инструментов
sudo apt update
sudo apt install build-essential cmake git pkg-config

# Установка GTK4
sudo apt install libgtk-4-dev

# Установка LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip
```

## 🚀 Сборка проекта

### Стандартная сборка

1. Клонируйте репозиторий:
```bash
git clone https://github.com/danayer/GTKSolver.git
cd GTKSolver
```

2. Настройте CMake:
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=$HOME/libtorch -DCMAKE_BUILD_TYPE=Release ..
```

3. Соберите проект:
```bash
cmake --build . --config Release
```

### 🚀 Сборка AppImage

AppImage позволяет упаковать приложение со всеми зависимостями в один исполняемый файл, который можно запустить на любом Linux-дистрибутиве.

1. Соберите проект как описано выше

2. Запустите скрипт создания AppImage:
```bash
cd /home/danayer/GTKSolver
chmod +x build_appimage.sh
./build_appimage.sh
```

3. После успешного выполнения в директории `/home/danayer/GTKSolver/appimage` будут созданы два AppImage файла:
   - `GTKSolver-GUI-x86_64.AppImage` - основное приложение для решения задач
   - `GTKSolver-Trainer-x86_64.AppImage` - приложение для обучения модели

4. Сделайте AppImage файлы исполняемыми и запустите:
```bash
chmod +x appimage/GTKSolver-GUI-x86_64.AppImage
chmod +x appimage/GTKSolver-Trainer-x86_64.AppImage

./appimage/GTKSolver-GUI-x86_64.AppImage
```

## 🎮 Использование

### 📚 Подготовка данных для обучения

Для обучения модели вам потребуются текстовые данные, содержащие примеры задач и их решения. Рекомендуемая структура:

1. **Учебный корпус** - текстовый файл (.txt), где каждая строка представляет собой пару "задача - решение", разделенную специальным символом (например, `|||`):
```markdown
Задача 1 ||| Решение 1
Задача 2 ||| Решение 2
...
```

### 🧠 Обучение нейросети через графический интерфейс

1. Запустите графический интерфейс для обучения:
```bash
./GTKSolver-trainer
```

2. В интерфейсе тренера вы можете:
   - 📂 Загрузить текстовые данные для обучения
   - ⚙️ Настроить параметры обучения (эпохи, размер батча)
   - 📊 Наблюдать за процессом обучения в реальном времени
   - 💾 Сохранить обученную модель

### 🧠 Обучение нейросети через командную строку

Для обучения через командную строку:
```bash
./train --input учебник.txt --output модель.pt --epochs 100
```

### 🖥️ Использование интерфейса для решения задач

Запустите графический интерфейс:
```bash
./GTKSolver-gui
```

В графическом интерфейсе вы можете:
1. 📂 Загрузить обученную модель
2. 📝 Загрузить текст задачи
3. ✅ Получить решение задачи с пояснениями
4. 💾 Сохранить результаты

## ⚙️ Архитектура системы

### 🧠 Нейросетевое ядро

В основе системы лежит трансформер-архитектура, адаптированная для работы с математическими выражениями. Модель преобразует входной текст в векторное представление, учитывая как обычный текст, так и математические формулы.

Процесс обучения включает:
- 🔤 Токенизацию текста и формул
- 📊 Векторизацию входных данных
- 📈 Обучение с использованием backpropagation
- 🧪 Валидацию на тестовом наборе данных

### 🖥️ Графический интерфейс

Интерфейс создан на языке Vala с использованием GTK4 и предоставляет удобные инструменты для:
- 💿 Управления моделями (загрузка, сохранение)
- 📝 Работы с задачами (ввод, загрузка из файла)
- 📊 Визуализации результатов
- 📤 Экспорта решений

### 🎓 Интерфейс обучения

Интерфейс для обучения позволяет:
- 📚 Управлять корпусом текстов для обучения
- ⚙️ Настраивать гиперпараметры модели
- 📈 Отслеживать прогресс обучения
- 🔄 Сохранять контрольные точки
- 📊 Визуализировать потери и точность

## 📂 Структура проекта

```
GTKSolver/
├── CMakeLists.txt           # Основной файл сборки CMake
├── src/
│   ├── core/                # Ядро нейросети на C++/LibTorch
│   │   ├── model.h
│   │   ├── model.cpp
│   │   ├── trainer.h
│   │   ├── trainer.cpp
│   │   ├── tokenizer.h
│   │   ├── tokenizer.cpp
│   │   ├── inference.h
│   │   └── inference.cpp
│   ├── gui/                 # Графический интерфейс на Vala/GTK4
│   │   ├── main.vala
│   │   ├── main_wrapper.c   # C-обертка для точки входа
│   │   ├── window.vala
│   │   ├── model_manager.vala
│   │   ├── task_processor.vala
│   │   ├── window.ui
│   │   └── resources.xml    # Определение GResources для GUI
│   └── trainer/             # Графический интерфейс для обучения
│       ├── main_trainer.vala
│       ├── main_wrapper.c   # C-обертка для точки входа
│       ├── trainer_window.vala
│       ├── training_manager.vala
│       ├── trainer_window.ui
│       └── resources.xml    # Определение GResources для Trainer
│
└── README.md                # Этот файл
```

## 🛠️ Реализация UI ресурсов

Проект использует GResources для эффективного встраивания UI файлов (*.ui) в исполняемые файлы:

1. **Определение ресурсов**: Файлы `resources.xml` определяют, какие файлы UI должны быть скомпилированы
   ```xml
   <gresources>
     <gresource prefix="/org/gtksolver/app">
       <file>window.ui</file>
     </gresource>
   </gresources>
   ```

2. **Компиляция ресурсов**: CMake компилирует XML-описания ресурсов в C-код с помощью `glib-compile-resources`.

3. **Доступ к ресурсам**: В Vala-коде UI-файлы доступны по путям:
   - `/org/gtksolver/app/window.ui` для основного приложения
   - `/org/gtksolver/trainer/trainer_window.ui` для тренера

## 🛠️ Реализация интеграции Vala и C++

Проект использует несколько технических решений для эффективной интеграции Vala и C++:

1. **C-обертки для точек входа**: Файлы `main_wrapper.c` содержат стандартные C-функции `main()`, которые инициализируют GTK и вызывают специальные экспортированные функции из Vala-кода (`gtk_solver_app_main` и `gtk_solver_trainer_main`).

2. **Экспорт функций из Vala в C**: Используются атрибуты `[CCode (cname = "...")]` для экспорта функций Vala с определёнными именами в C:
   ```vala
   [CCode (cname = "gtk_solver_app_main")]
   public void gtk_solver_app_main(string[] args) {
       var app = new GTKSolverApp();
       app.run(args);
   }
   ```

3. **GResources для UI файлов**: UI файлы встраиваются в исполняемые файлы через систему GResources:
   ```xml
   <gresources>
     <gresource prefix="/org/gtksolver/app">
       <file>window.ui</file>
     </gresource>
   </gresources>
   ```

4. **C API для C++ функциональности**: В файле `inference.cpp` реализовано C API, которое позволяет вызывать C++ функциональность из Vala:
   ```cpp
   extern "C" {
       bool load_model_from_file(const char* path);
       bool load_vocabulary_from_file(const char* path);
       char* process_task_with_model(const char* task_text, void* model_ptr);
   }
   ```

Данный подход позволяет совместить высокопроизводительный C++ код для машинного обучения с современным пользовательским интерфейсом на Vala/GTK4.

## 📜 Лицензия

MIT

## ✍️ Авторы

Danayer, 2025

