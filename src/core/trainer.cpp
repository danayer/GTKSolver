#include "trainer.h"
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

namespace formula_teacher {

FormulaTrainer::FormulaTrainer(FormulaModel& model, FormulaTokenizer& tokenizer, double learning_rate)
    : model(model), tokenizer(tokenizer), optimizer(model.parameters(), learning_rate) {
}

void FormulaTrainer::prepare_data(const std::string& text_path) {
    // Открываем файл с данными
    std::ifstream file(text_path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл с данными: " + text_path);
    }
    
    std::string line;
    std::vector<std::vector<int>> all_data;
    
    // Читаем файл построчно
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Токенизируем текст
        auto tokens = tokenizer.tokenize(line);
        if (tokens.size() > 3) { // Минимальная длина: SOS + хотя бы 1 токен + EOS
            all_data.push_back(tokens);
        }
    }
    
    // Перемешиваем данные
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_data.begin(), all_data.end(), g);
    
    // Разбиваем на тренировочную и валидационную выборки (80% / 20%)
    size_t train_size = all_data.size() * 0.8;
    training_data.assign(all_data.begin(), all_data.begin() + train_size);
    validation_data.assign(all_data.begin() + train_size, all_data.end());
    
    std::cout << "Данные подготовлены: " << training_data.size() << " примеров для обучения, "
              << validation_data.size() << " примеров для валидации." << std::endl;
}

std::vector<torch::Tensor> FormulaTrainer::create_batches(
    const std::vector<std::vector<int>>& data, int batch_size) {
    
    std::vector<torch::Tensor> batches;
    
    // Группируем данные примерно одинаковой длины для эффективности
    std::map<int, std::vector<std::vector<int>>> length_groups;
    for (const auto& example : data) {
        int length_group = (example.size() - 1) / 10; // Группируем по интервалам длины 10
        length_groups[length_group].push_back(example);
    }
    
    // Создаем батчи из каждой группы
    for (auto& [_, group] : length_groups) {
        for (size_t i = 0; i < group.size(); i += batch_size) {
            size_t actual_batch_size = std::min(batch_size, static_cast<int>(group.size() - i));
            
            // Находим максимальную длину в текущем батче
            size_t max_length = 0;
            for (size_t j = i; j < i + actual_batch_size; ++j) {
                max_length = std::max(max_length, group[j].size());
            }
            
            // Создаем тензор для батча
            torch::Tensor batch = torch::zeros({actual_batch_size, max_length}, torch::kLong);
            
            // Заполняем тензор токенами, заполняя PAD токенами короткие последовательности
            for (size_t j = 0; j < actual_batch_size; ++j) {
                const auto& example = group[i + j];
                for (size_t k = 0; k < example.size(); ++k) {
                    batch[j][k] = example[k];
                }
                // Остаток заполняем PAD токенами
                for (size_t k = example.size(); k < max_length; ++k) {
                    batch[j][k] = FormulaTokenizer::PAD;
                }
            }
            
            batches.push_back(batch);
        }
    }
    
    // Перемешиваем батчи для лучшего обучения
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(batches.begin(), batches.end(), g);
    
    return batches;
}

void FormulaTrainer::train(int epochs, int batch_size, const std::string& checkpoint_path) {
    // Переводим модель в режим обучения
    model.train();
    
    for (int epoch = 0; epoch < epochs; ++epoch) {
        double total_loss = 0.0;
        int num_batches = 0;
        
        // Создаем батчи для текущей эпохи
        auto batches = create_batches(training_data, batch_size);
        
        for (const auto& batch : batches) {
            // Обнуляем градиенты
            optimizer.zero_grad();
            
            // Входные данные - все токены кроме последнего
            auto input = batch.slice(1, 0, batch.size(1) - 1);
            // Целевые данные - все токены кроме первого (сдвиг на 1)
            auto target = batch.slice(1, 1, batch.size(1));
            
            // Прямой проход
            auto output = model.forward(input);
            
            // Изменяем размерности для функции потерь
            auto output_flat = output.reshape({-1, output.size(2)});
            auto target_flat = target.reshape({-1});
            
            // Вычисляем функцию потерь
            auto loss = torch::nn::functional::cross_entropy(
                output_flat, target_flat, torch::nn::functional::CrossEntropyFuncOptions().ignore_index(FormulaTokenizer::PAD)
            );
            
            // Обратное распространение
            loss.backward();
            
            // Обновление весов
            optimizer.step();
            
            total_loss += loss.item<double>();
            num_batches++;
        }
        
        // Вычисляем среднюю потерю и проводим валидацию
        double avg_loss = total_loss / num_batches;
        double val_accuracy = validate(batch_size);
        
        std::cout << "Эпоха " << epoch + 1 << "/" << epochs 
                  << " - Потери: " << avg_loss 
                  << " - Точность валидации: " << val_accuracy << std::endl;
        
        // Сохраняем контрольную точку, если указан путь
        if (!checkpoint_path.empty() && (epoch + 1) % 5 == 0) {
            std::string epoch_path = checkpoint_path + "_epoch" + std::to_string(epoch + 1) + ".pt";
            model.save(epoch_path);
            std::cout << "Модель сохранена в " << epoch_path << std::endl;
        }
    }
    
    // Сохраняем финальную модель
    if (!checkpoint_path.empty()) {
        model.save(checkpoint_path);
        std::cout << "Финальная модель сохранена в " << checkpoint_path << std::endl;
    }
}

double FormulaTrainer::validate(int batch_size) {
    // Переводим модель в режим оценки
    model.eval();
    
    double total_accuracy = 0.0;
    int num_batches = 0;
    
    // Создаем валидационные батчи
    auto batches = create_batches(validation_data, batch_size);
    
    // Отключаем вычисление градиентов для ускорения валидации
    torch::NoGradGuard no_grad;
    
    for (const auto& batch : batches) {
        // Входные данные - все токены кроме последнего
        auto input = batch.slice(1, 0, batch.size(1) - 1);
        // Целевые данные - все токены кроме первого
        auto target = batch.slice(1, 1, batch.size(1));
        
        // Прямой проход
        auto output = model.forward(input);
        
        // Вычисляем точность
        double batch_accuracy = calculate_accuracy(output, target);
        total_accuracy += batch_accuracy;
        num_batches++;
    }
    
    // Возвращаем модель в режим обучения
    model.train();
    
    // Возвращаем среднюю точность
    return total_accuracy / num_batches;
}

double FormulaTrainer::calculate_accuracy(const torch::Tensor& predictions, const torch::Tensor& targets) {
    // Получаем индексы с максимальными значениями (предсказанные классы)
    auto predicted_classes = predictions.argmax(2);
    
    // Создаем маску для игнорирования PAD токенов
    auto mask = targets != FormulaTokenizer::PAD;
    
    // Считаем правильные предсказания (с учетом маски)
    auto correct = (predicted_classes == targets) & mask;
    
    // Считаем количество правильных предсказаний и общее количество токенов
    int64_t num_correct = correct.sum().item<int64_t>();
    int64_t total_tokens = mask.sum().item<int64_t>();
    
    // Возвращаем точность
    return static_cast<double>(num_correct) / total_tokens;
}

std::string FormulaTrainer::test_example(const std::string& input_text) {
    // Переводим модель в режим оценки
    model.eval();
    
    // Токенизируем входной текст
    auto input_tokens = tokenizer.tokenize(input_text);
    
    // Генерируем ответ
    auto output_tokens = model.generate(input_tokens);
    
    // Детокенизируем результат
    std::string result = tokenizer.detokenize(output_tokens);
    
    // Возвращаем модель в режим обучения
    model.train();
    
    return result;
}

} // namespace formula_teacher
