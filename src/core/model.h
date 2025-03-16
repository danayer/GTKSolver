#pragma once

#include <torch/torch.h>
#include <string>
#include <vector>

namespace formula_teacher {

// Класс для кодирования текста и формул
class FormulaEncoder : public torch::nn::Module {
public:
    FormulaEncoder(int vocab_size, int embedding_dim, int hidden_dim);
    
    // Прямой проход через энкодер
    torch::Tensor forward(torch::Tensor x);
    
private:
    torch::nn::Embedding embedding = nullptr;
    torch::nn::LSTM lstm = nullptr;
    torch::nn::Linear fc = nullptr;
};

// Класс для декодирования ответов
class FormulaDecoder : public torch::nn::Module {
public:
    FormulaDecoder(int vocab_size, int embedding_dim, int hidden_dim);
    
    // Прямой проход через декодер
    torch::Tensor forward(torch::Tensor x, torch::Tensor encoder_output);
    
private:
    torch::nn::Embedding embedding = nullptr;
    torch::nn::LSTM lstm = nullptr;
    torch::nn::Linear fc = nullptr;
    torch::nn::Linear attn = nullptr;
};

// Полная модель Seq2Seq с механизмом внимания для работы с формулами
class FormulaModel : public torch::nn::Module {
public:
    FormulaModel(int vocab_size, int embedding_dim, int hidden_dim);
    
    // Сохранение модели
    void save(const std::string& path);
    
    // Загрузка модели - изменен возвращаемый тип
    static std::shared_ptr<FormulaModel> load(const std::string& path, torch::Device device = torch::kCPU);
    
    // Прямой проход через всю модель
    torch::Tensor forward(torch::Tensor input_seq);
    
    // Генерация ответа на задачу
    std::vector<int> generate(std::vector<int>& input_tokens, int max_length = 100);
    
private:
    int hidden_dim;
    int vocab_size;
    std::shared_ptr<FormulaEncoder> encoder = nullptr;
    std::shared_ptr<FormulaDecoder> decoder = nullptr;
};

} // namespace formula_teacher
