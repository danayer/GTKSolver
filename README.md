# 🧠 GTKSolver - нейросеть для решения задач с формулами

## 📚 Описание проекта

**GTKSolver** - это интеллектуальная система, использующая нейронные сети для обучения на текстовых данных учебника, содержащих математические формулы, и последующего решения задач по этой тематике.

Система состоит из двух основных компонентов:
1. **🔬 Нейросетевое ядро** - написано на C++ с использованием LibTorch (библиотека PyTorch для C++)
2. **🖥️ Графический интерфейс** - разработан на языке Vala с использованием GTK4

## 📋 Требования для установки

### 🔧 Зависимости
- C++17 или выше
- CMake 3.14 или выше
- LibTorch 1.9.0 или выше
- GTK4
- Vala (последняя версия)
- Build Essential
- pkg-config

### 🔧 Установка зависимостей

#### 🐧 Ubuntu/Debian
```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install build-essential cmake git pkg-config

# Установка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip
# Для систем с CUDA можно загрузить соответствующую версию с сайта PyTorch

# Установка GTK4 и Vala
sudo apt-get install libgtk-4-dev valac
```

#### 🐧 Alt Linux
```bash
# Установка базовых инструментов
sudo apt-get update
sudo apt-get install make-gcc cmake git pkg-config

# Установка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Установка GTK4 и Vala
sudo apt-get install libgtk4-devel vala-tools
```

#### 🔒 Astra Linux
```bash
# Установка базовых инструментов
sudo apt update
sudo apt install build-essential cmake git pkg-config

# Установка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Установка GTK4 и Vala
sudo apt install libgtk-4-dev valac
```

#### 🏹 Arch Linux
```bash
# Установка базовых инструментов
sudo pacman -Syu
sudo pacman -S base-devel cmake git pkg-config

# Установка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Установка GTK4 и Vala
sudo pacman -S gtk4 vala
```

#### 🎩 Fedora Linux
```bash
# Установка базовых инструментов
sudo dnf update
sudo dnf install gcc-c++ cmake git pkg-config

# Установка LibTorch
wget https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-1.9.0%2Bcpu.zip
unzip libtorch-cxx11-abi-shared-with-deps-1.9.0+cpu.zip

# Установка GTK4 и Vala
sudo dnf install gtk4-devel vala
```

## 🚀 Сборка проекта

1. Клонируйте репозиторий:
```bash
git clone https://github.com/danayer/GTKSolver.git
cd GTKSolver
```

2. Настройте CMake:
```bash
mkdir build && cd build
cmake -DCMAKE_PREFIX_PATH=/путь/к/libtorch ..
```

3. Соберите проект:
```bash
cmake --build . --config Release
```

## 🎮 Использование

### 🧠 Обучение нейросети

1. Подготовьте текстовый файл с содержимым учебника, где формулы записаны в формате LaTeX.
2. Запустите программу обучения:
```bash
./train --input учебник.txt --output модель.pt --epochs 100
```

### 🖥️ Использование графического интерфейса

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
│   └── gui/                 # Графический интерфейс на Vala/GTK4
│       ├── main.vala
│       ├── window.vala
│       ├── model_manager.vala
│       ├── task_processor.vala
│       └── window.ui
│
└── README.md                # Этот файл
```

## 📜 Лицензия

MIT

## ✍️ Авторы

Danayer, 2025

