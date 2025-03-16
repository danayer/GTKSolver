#include "model.h"
#include "trainer.h"
#include "tokenizer.h"
#include <iostream>
#include <string>
#include <cstring>

void print_usage() {
    std::cout << "Использование: train [опции]\n"
              << "Опции:\n"
              << "  --input FILE       Путь к входному текстовому корпусу для обучения\n"
              << "  --output FILE      Путь для сохранения обученной модели\n"
              << "  --vocab FILE       Путь для сохранения словаря (по умолчанию output_path + .vocab)\n"
              << "  --epochs N         Количество эпох обучения (по умолчанию 50)\n"
              << "  --batch-size N     Размер батча (по умолчанию 32)\n"
              << "  --emb-dim N        Размерность эмбеддингов (по умолчанию 256)\n"
              << "  --hidden-dim N     Размер скрытых слоёв (по умолчанию 512)\n"
              << "  --learning-rate N  Скорость обучения (по умолчанию 0.001)\n"
              << "  --help             Показать эту справку\n";
}

int main(int argc, char* argv[]) {
    std::string input_path;
    std::string output_path;
    std::string vocab_path;
    int epochs = 50;
    int batch_size = 32;
    int embedding_dim = 256;
    int hidden_dim = 512;
    double learning_rate = 0.001;
    
    // Разбор аргументов командной строки
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            input_path = argv[++i];
        } else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            output_path = argv[++i];
        } else if (strcmp(argv[i], "--vocab") == 0 && i + 1 < argc) {
            vocab_path = argv[++i];
        } else if (strcmp(argv[i], "--epochs") == 0 && i + 1 < argc) {
            epochs = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--batch-size") == 0 && i + 1 < argc) {
            batch_size = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--emb-dim") == 0 && i + 1 < argc) {
            embedding_dim = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--hidden-dim") == 0 && i + 1 < argc) {
            hidden_dim = std::stoi(argv[++i]);
        } else if (strcmp(argv[i], "--learning-rate") == 0 && i + 1 < argc) {
            learning_rate = std::stod(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else {
            std::cerr << "Неизвестный аргумент: " << argv[i] << std::endl;
            print_usage();
            return 1;
        }
    }
    
    // Проверка обязательных аргументов
    if (input_path.empty() || output_path.empty()) {
        std::cerr << "Ошибка: Не указаны обязательные аргументы --input и --output" << std::endl;
        print_usage();
        return 1;
    }
    
    // Установка пути к словарю по умолчанию, если не задан
    if (vocab_path.empty()) {
        vocab_path = output_path + ".vocab";
    }
    
    try {
        std::cout << "Инициализация токенизатора..." << std::endl;
        formula_teacher::FormulaTokenizer tokenizer;
        
        std::cout << "Построение словаря из корпуса..." << std::endl;
        tokenizer.build_vocabulary(input_path);
        
        std::cout << "Сохранение словаря в " << vocab_path << std::endl;
        tokenizer.save_vocabulary(vocab_path);
        
        int vocab_size = tokenizer.vocab_size();
        std::cout << "Размер словаря: " << vocab_size << std::endl;
        
        std::cout << "Создание модели..." << std::endl;
        formula_teacher::FormulaModel model(vocab_size, embedding_dim, hidden_dim);
        
        std::cout << "Инициализация тренера..." << std::endl;
        formula_teacher::FormulaTrainer trainer(model, tokenizer, learning_rate);
        
        std::cout << "Подготовка данных для обучения..." << std::endl;
        trainer.prepare_data(input_path);
        
        std::cout << "Запуск обучения на " << epochs << " эпохах, размер батча: " << batch_size << std::endl;
        std::cout << "Это может занять некоторое время..." << std::endl;
        trainer.train(epochs, batch_size, output_path);
        
        std::cout << "Обучение завершено. Модель сохранена в " << output_path << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
}
