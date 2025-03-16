#pragma once

#include "model.h"
#include "tokenizer.h"
#include <string>

namespace formula_teacher {

class FormulaInference {
public:
    // Конструктор
    FormulaInference();
    
    // Загрузка модели из файла
    bool load_model(const std::string& model_path);
    
    // Загрузка словаря из файла
    bool load_vocabulary(const std::string& vocab_path);
    
    // Решение задачи
    std::string solve_task(const std::string& task_text);
    
private:
    // Указатели на модель и токенизатор
    std::shared_ptr<FormulaModel> model;
    std::unique_ptr<FormulaTokenizer> tokenizer;
    
    // Флаги для отслеживания состояния
    bool model_loaded;
    bool vocabulary_loaded;
};

// C API для использования в Vala
extern "C" {
    bool load_model_from_file(const char* path);
    bool load_vocabulary_from_file(const char* path);
    char* process_task_with_model(const char* task_text, void* model_ptr);
}

} // namespace formula_teacher
