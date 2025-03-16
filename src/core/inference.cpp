#include "inference.h"
#include <iostream>
#include <memory>

namespace formula_teacher {

// Глобальный объект для работы с библиотекой из C API
static FormulaInference g_inference;

FormulaInference::FormulaInference() : model_loaded(false), vocabulary_loaded(false) {
    // Инициализация
}

bool FormulaInference::load_model(const std::string& model_path) {
    try {
        model = FormulaModel::load(model_path);
        model_loaded = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке модели: " << e.what() << std::endl;
        model_loaded = false;
        return false;
    }
}

bool FormulaInference::load_vocabulary(const std::string& vocab_path) {
    try {
        tokenizer = std::make_unique<FormulaTokenizer>(vocab_path);
        vocabulary_loaded = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке словаря: " << e.what() << std::endl;
        vocabulary_loaded = false;
        return false;
    }
}

std::string FormulaInference::solve_task(const std::string& task_text) {
    if (!model_loaded || !vocabulary_loaded) {
        return "Ошибка: Модель или словарь не загружены";
    }
    
    try {
        // Токенизация входного текста
        std::vector<int> input_tokens = tokenizer->tokenize(task_text);
        
        // Добавление специальных токенов
        input_tokens.insert(input_tokens.begin(), FormulaTokenizer::SOS);
        input_tokens.push_back(FormulaTokenizer::EOS);
        
        // Генерация решения
        std::vector<int> output_tokens = model->generate(input_tokens);
        
        // Детокенизация результата
        std::string solution = tokenizer->detokenize(output_tokens);
        
        return solution;
    } catch (const std::exception& e) {
        return std::string("Ошибка при решении задачи: ") + e.what();
    }
}

} // namespace formula_teacher

// C API реализация
extern "C" {

bool load_model_from_file(const char* path) {
    return formula_teacher::g_inference.load_model(path);
}

bool load_vocabulary_from_file(const char* path) {
    return formula_teacher::g_inference.load_vocabulary(path);
}

char* process_task_with_model(const char* task_text, void*) {
    std::string result = formula_teacher::g_inference.solve_task(task_text);
    
    // Необходимо выделить память для строки, которую можно будет освободить в Vala
    char* c_result = (char*)malloc(result.length() + 1);
    strcpy(c_result, result.c_str());
    
    return c_result;
}

}
