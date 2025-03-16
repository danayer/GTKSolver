#pragma once

#include "model.h"
#include "tokenizer.h"
#include <torch/torch.h>
#include <string>
#include <vector>

namespace formula_teacher {

class FormulaTrainer {
public:
    FormulaTrainer(FormulaModel& model, FormulaTokenizer& tokenizer, 
                   double learning_rate = 0.001);
    
    // Подготовка данных из текстового файла
    void prepare_data(const std::string& text_path);
    
    // Обучение модели
    void train(int epochs, int batch_size, const std::string& checkpoint_path = "");
    
    // Валидация модели
    double validate(int batch_size);
    
    // Тестирование на примере
    std::string test_example(const std::string& input_text);
    
private:
    // Модель и токенизатор
    FormulaModel& model;
    FormulaTokenizer& tokenizer;
    
    // Данные для обучения
    std::vector<std::vector<int>> training_data;
    std::vector<std::vector<int>> validation_data;
    
    // Оптимизатор
    torch::optim::Adam optimizer;
    
    // Создание батчей для обучения
    std::vector<torch::Tensor> create_batches(const std::vector<std::vector<int>>& data, 
                                            int batch_size);
    
    // Подсчет точности
    double calculate_accuracy(const torch::Tensor& predictions, const torch::Tensor& targets);
};

} // namespace formula_teacher
