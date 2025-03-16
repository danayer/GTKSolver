# 🧠 GTKSolver - Нейросеть для решения математических задач

<div align="center">
  <img src="gtksolver.svg" alt="GTKSolver Logo" width="180">
  <img src="gtksolver-trainer.svg" alt="GTKSolver Logo" width="180">
  <br>
  <em>Интеллектуальная система для решения задач с формулами на основе нейросетей</em>
  <br><br>
  <a href="#-установка"><strong>Установка</strong></a> •
  <a href="#-использование"><strong>Использование</strong></a> •
  <a href="#-обучение-модели"><strong>Обучение</strong></a> •
  <a href="#-архитектура"><strong>Архитектура</strong></a> •
  <a href="#-сборка-и-использование-appimage"><strong>AppImage</strong></a>
</div>

## 📚 Описание проекта

**GTKSolver** - это интеллектуальная система, использующая нейронные сети для обучения на текстовых данных учебника, содержащих математические формулы, и последующего решения задач по этой тематике.

Система предлагает следующие возможности:
- 🔬 **Автоматическое решение задач** с математическими формулами
- 🧠 **Обучение на текстовых данных** с математическими выражениями
- 🖊️ **Пошаговое объяснение решений** с формулами и обоснованиями
- 📊 **Визуализация процесса обучения** модели
- 📁 **Сохранение и загрузка** обученных моделей

Система состоит из трех основных компонентов:
1. **🔬 Нейросетевое ядро** - написано на C++ с использованием LibTorch (библиотека PyTorch для C++)
2. **🖥️ Графический интерфейс решения задач** - разработан на языке Vala с использованием GTK4
3. **🎓 Графический интерфейс обучения** - отдельное приложение на Vala + GTK4 для обучения модели

## 🚀 Установка

### 🔧 Зависимости
- C++17 или выше
- CMake 3.14 или выше
- LibTorch 1.9.0 или выше
- GTK4
- Build Essential
- pkg-config

### 📀 Установка в различных дистрибутивах Linux

<details>
<summary><img src="https://upload.wikimedia.org/wikipedia/commons/9/9e/UbuntuCoF.svg" height="20"> <b>Ubuntu/Debian</b></summary>

```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install build-essential cmake git pkg-config

# Установка GTK4
sudo apt-get install libgtk-4-dev

# Установка Vala
sudo apt-get install valac

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

<details>
<summary><img src="https://www.altlinux.org/Images.www.altlinux.org/3/30/Alt_linux_team_small.png" height="20"> <b>Alt Linux</b></summary>

```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install make-gcc cmake git pkg-config

# Установка GTK4
sudo apt-get install libgtk4-devel

# Установка Vala
sudo apt-get install vala-compiler

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

<details>
<summary><img src="https://upload.wikimedia.org/wikipedia/ru/8/86/Astra_Linux.png" height="20"> <b>Astra Linux</b></summary>

```bash
# Установка базовых инструментов
sudo apt update
sudo apt install build-essential cmake git pkg-config

# Установка GTK4
sudo apt install libgtk-4-dev

# Установка Vala
sudo apt install valac

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

<details>
<summary><img src="https://upload.wikimedia.org/wikipedia/commons/1/13/Arch_Linux_"Crystal"_icon.svg" height="20"> <b>Arch Linux</b></summary>

```bash
# Установка базовых инструментов
sudo pacman -Syu
sudo pacman -S base-devel cmake git pkg-config

# Установка GTK4
sudo pacman -S gtk4

# Установка Vala
sudo pacman -S vala

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

<details>
<summary><img src="https://upload.wikimedia.org/wikipedia/commons/4/45/The_GIMP_icon_-_gnome.svg" height="20"> <b>Fedora/Red Hat/CentOS</b></summary>

```bash
# Установка базовых инструментов
sudo dnf update
sudo dnf install gcc gcc-c++ make cmake git pkg-config

# Установка GTK4
sudo dnf install gtk4-devel

# Установка Vala
sudo dnf install vala

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

<details>
<summary><img src="https://upload.wikimedia.org/wikipedia/commons/d/d0/OpenSUSE_Logo.svg" height="20"> <b>openSUSE</b></summary>

```bash
# Установка базовых инструментов
sudo zypper refresh
sudo zypper install -t pattern devel_basis
sudo zypper install cmake git pkg-config

# Установка GTK4
sudo zypper install gtk4-devel

# Установка Vala
sudo zypper install vala

# Создание директории для LibTorch
mkdir -p $HOME/libtorch
cd $HOME/libtorch

# Загрузка и распаковка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Добавление переменной окружения
echo 'export LD_LIBRARY_PATH=$HOME/libtorch/lib:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

</details>

### 🔨 Сборка из исходников

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

После успешной сборки у вас появятся следующие исполняемые файлы:
- `formula-teacher-gui` - основное приложение для решения задач
- `formula-teacher-trainer` - приложение для обучения модели
- `train` - консольная утилита для обучения модели

## 🎮 Использование

### 📊 Решение задач через графический интерфейс

1. **Запуск приложения**

    ```bash
    ./build/formula-teacher-gui
    ```

2. **Загрузка модели**
   
   Нажмите кнопку "Загрузить модель" и выберите файл модели с расширением `.pt`. После загрузки модели появится информационное окно с её характеристиками.
   
   ![Загрузка модели](docs/images/load_model_example.png)

3. **Ввод задачи**
   
   Введите или вставьте текст задачи в верхнее текстовое поле. Вы также можете загрузить задачу из файла, нажав кнопку "Загрузить задачу".
   
   **Пример задачи:**
   ```
   Найти интеграл функции f(x) = x^2 + 3x + 2.
   ```

4. **Решение задачи**
   
   Нажмите кнопку "Решить". Через несколько секунд в нижнем поле появится решение задачи.
   
   **Пример решения:**
   ```
   Решение с использованием модели calculus v2.1 (точность: 95.3%)

   Задачу из области "интегральное исчисление" решим с использованием метода интегрирования степенных функций.

   Основная формула для решения: ∫x^n dx = (x^(n+1))/(n+1) + C, при n≠-1

   Шаги решения:
   1. Разобьем интеграл на сумму интегралов: ∫(x^2 + 3x + 2)dx = ∫x^2dx + ∫3xdx + ∫2dx
   2. Применим формулу интегрирования для каждого слагаемого:
      ∫x^2dx = (x^3)/3 + C1
      ∫3xdx = 3∫xdx = 3(x^2/2) + C2 = (3x^2)/2 + C2
      ∫2dx = 2x + C3
   3. Объединим результаты: (x^3)/3 + (3x^2)/2 + 2x + C, где C = C1 + C2 + C3

   Ответ: (x^3)/3 + (3x^2)/2 + 2x + C
   ```

5. **Сохранение решения**
   
   При необходимости вы можете сохранить полученное решение в текстовый файл, нажав кнопку "Сохранить решение".

### 📋 Примеры задач для решения

<details>
<summary><b>Интегралы</b></summary>

1. **Простой пример:**
   ```
   Найти интеграл sin(x)dx
   ```

2. **Сложный пример:**
   ```
   Вычислить интеграл (e^x * cos(x))dx
   ```
</details>

<details>
<summary><b>Производные</b></summary>

1. **Простой пример:**
   ```
   Найти производную функции f(x) = x^3 + 4x^2 - 2x + 7
   ```

2. **Сложный пример:**
   ```
   Вычислить производную f(x) = ln(sin(x^2))
   ```
</details>

<details>
<summary><b>Уравнения</b></summary>

1. **Линейное уравнение:**
   ```
   Решить уравнение 3x - 5 = 7
   ```

2. **Квадратное уравнение:**
   ```
   Найти корни уравнения x^2 - 5x + 6 = 0
   ```
</details>

<details>
<summary><b>Геометрия</b></summary>

1. **Площадь:**
   ```
   Найти площадь треугольника со сторонами 3, 4, 5
   ```

2. **Окружность:**
   ```
   Вычислить длину окружности радиуса 4.5 см
   ```
</details>

## 📚 Обучение модели

### 📝 Подготовка данных для обучения

Для обучения модели требуются парные данные "задача-решение". Такие данные можно подготовить из учебников, задачников и других образовательных материалов.

#### 🧩 Формат данных для обучения
Данные должны быть представлены в формате текстового файла, где каждая строка содержит пару "задача|||решение" (разделитель - три вертикальные черты).

**Пример файла данных:**
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

## 🛠️ Сборка и использование AppImage

[<img src="https://appimage.github.io/img/logo3.svg" height="30">](https://appimage.github.io/)

### Что такое AppImage?

AppImage — это формат распространения приложений для Linux, который позволяет запускать программы без установки. Это означает:
- 🚀 Никаких зависимостей для установки
- 🔌 Запуск на любом дистрибутиве
- 📦 Всё нужное находится внутри одного файла
- 🗑️ Удаление — просто удалите файл AppImage

### Использование GTKSolver в формате AppImage

1. **Загрузите AppImage файл**

   Скачайте файл AppImage с [GitHub Releases](https://github.com/danayer/GTKSolver/releases):
   ```bash
   wget https://github.com/danayer/GTKSolver/releases/latest/download/GTKSolver-x86_64.AppImage
   ```

2. **Сделайте файл исполняемым**

   ```bash
   chmod +x GTKSolver-x86_64.AppImage
   ```

3. **Запустите приложение**

   ```bash
   ./GTKSolver-x86_64.AppImage
   ```

   Для удобства можете создать ярлык на рабочем столе или добавить в меню приложений:
   ```bash
   ./GTKSolver-x86_64.AppImage --install
   ```

### Создание собственного AppImage

Если вы хотите собрать GTKSolver в формате AppImage из исходного кода:

1. **Установите необходимые инструменты**

   ```bash
   sudo apt-get install patchelf desktop-file-utils libgdk-pixbuf2.0-dev fakeroot strace
   wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
   wget https://github.com/linuxdeploy/linuxdeploy-plugin-gtk/releases/download/continuous/linuxdeploy-plugin-gtk-x86_64.AppImage
   chmod +x linuxdeploy*.AppImage
   ```

2. **Соберите проект с правильным путём установки**

   ```bash
   mkdir build && cd build
   cmake -DCMAKE_PREFIX_PATH=$HOME/libtorch -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
   make -j$(nproc)
   make install DESTDIR=AppDir
   ```

3. **Создайте AppImage**

   ```bash
   ./linuxdeploy-x86_64.AppImage --appdir AppDir \
       --plugin gtk \
       --output appimage \
       --desktop-file=../desktop/gtksolver-gui.desktop \
       --icon-file=../icons/gtksolver.svg
   ```

4. **Интеграция с системой**

   После создания AppImage его можно интегрировать в систему запуском:
   ```bash
   ./GTKSolver-x86_64.AppImage --install
   ```

5. **Особенности для LibTorch**

   При использовании LibTorch внутри AppImage необходимо убедиться, что все библиотеки корректно упакованы:
   ```bash
   mkdir -p AppDir/usr/lib
   cp -a $HOME/libtorch/lib/* AppDir/usr/lib/
   ```

   При создании AppImage добавьте:
   ```bash
   --extra-libs=$HOME/libtorch/lib/
   ```

### Распространение вашего AppImage

1. **Проверьте AppImage на разных дистрибутивах**

   Протестируйте AppImage на разных версиях Linux, чтобы убедиться в его работоспособности.

2. **Добавьте информацию в GitHub Release**

   При создании релиза на GitHub загрузите AppImage файл и укажите информацию о требованиях:
   ```
   GTKSolver-x86_64.AppImage - Standalone application bundle for Linux x86_64
   - No installation required
   - Contains all dependencies
   - Minimum system requirements: Linux kernel 3.2 or newer
   ```

3. **Обновление AppImage**

   Для обновления просто замените старый файл AppImage на новый.


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
├── desktop/                 # Desktop файлы для интеграции в систему
│   ├── gtksolver-gui.desktop
│   └── gtksolver-trainer.desktop
├── icons/                   # Иконки приложения
│   ├── gtksolver.svg
│   └── gtksolver-trainer.svg
├── metainfo/                # Метаданные AppStream
│   └── gtksolver.appdata.xml
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
