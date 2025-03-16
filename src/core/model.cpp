#include "model.h"

namespace formula_teacher {

FormulaEncoder::FormulaEncoder(int vocab_size, int embedding_dim, int hidden_dim) {
    embedding = register_module("embedding", torch::nn::Embedding(vocab_size, embedding_dim));
    lstm = register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(embedding_dim, hidden_dim).bidirectional(true)));
    fc = register_module("fc", torch::nn::Linear(hidden_dim * 2, hidden_dim));
}

torch::Tensor FormulaEncoder::forward(torch::Tensor x) {
    auto embedded = embedding(x);
    torch::Tensor output, hidden;
    std::tie(output, hidden) = lstm(embedded);
    return fc(output);
}

FormulaDecoder::FormulaDecoder(int vocab_size, int embedding_dim, int hidden_dim) {
    embedding = register_module("embedding", torch::nn::Embedding(vocab_size, embedding_dim));
    lstm = register_module("lstm", torch::nn::LSTM(torch::nn::LSTMOptions(embedding_dim + hidden_dim, hidden_dim)));
    attn = register_module("attn", torch::nn::Linear(hidden_dim * 2, hidden_dim));
    fc = register_module("fc", torch::nn::Linear(hidden_dim, vocab_size));
}

torch::Tensor FormulaDecoder::forward(torch::Tensor x, torch::Tensor encoder_output) {
    // Реализация механизма внимания
    auto embedded = embedding(x);
    
    auto batch_size = embedded.size(0);
    auto seq_len = embedded.size(1);
    auto enc_seq_len = encoder_output.size(1);
    
    // Расширение для применения внимания
    auto extended_embedded = embedded.unsqueeze(2).repeat({1, 1, enc_seq_len, 1});
    auto extended_encoder = encoder_output.unsqueeze(1).repeat({1, seq_len, 1, 1});
    
    // Вычисление весов внимания
    auto attention_input = torch::cat({extended_embedded, extended_encoder}, 3);
    auto attention_weights = torch::softmax(attn(attention_input).squeeze(3), 2);
    
    // Применение внимания к выходу энкодера
    auto context = torch::bmm(attention_weights, encoder_output);
    
    // Комбинируем с встроенными векторами
    auto rnn_input = torch::cat({embedded, context}, 2);
    
    torch::Tensor output, hidden;
    std::tie(output, hidden) = lstm(rnn_input);
    
    return fc(output);
}

FormulaModel::FormulaModel(int vocab_size, int embedding_dim, int hidden_dim) : 
    vocab_size(vocab_size), hidden_dim(hidden_dim) {
    encoder = register_module("encoder", FormulaEncoder(vocab_size, embedding_dim, hidden_dim));
    decoder = register_module("decoder", FormulaDecoder(vocab_size, embedding_dim, hidden_dim));
}

torch::Tensor FormulaModel::forward(torch::Tensor input_seq) {
    auto encoder_output = encoder->forward(input_seq);
    
    // Первый токен для декодера - специальный токен начала последовательности
    auto decoder_input = torch::full({input_seq.size(0), 1}, 1, input_seq.options());
    
    auto output = decoder->forward(decoder_input, encoder_output);
    return output;
}

std::vector<int> FormulaModel::generate(std::vector<int>& input_tokens, int max_length) {
    // Конвертация входных токенов в тензор
    auto input_tensor = torch::tensor(input_tokens).unsqueeze(0);
    
    // Получаем выход энкодера
    auto encoder_output = encoder->forward(input_tensor);
    
    std::vector<int> output_tokens;
    
    // Первый токен - специальный токен начала последовательности
    auto decoder_input = torch::full({1, 1}, 1, input_tensor.options());
    
    // Генерация токенов один за другим
    for (int i = 0; i < max_length; i++) {
        auto output = decoder->forward(decoder_input, encoder_output);
        
        // Получаем токен с наивысшей вероятностью
        auto top_token = output.argmax(2).item<int>();
        output_tokens.push_back(top_token);
        
        // Проверка на токен конца последовательности
        if (top_token == 2) {
            break;
        }
        
        // Обновляем входные данные декодера
        decoder_input[0][0] = top_token;
    }
    
    return output_tokens;
}

void FormulaModel::save(const std::string& path) {
    torch::save(shared_from_this(), path);
}

FormulaModel FormulaModel::load(const std::string& path, torch::Device device) {
    torch::jit::script::Module module;
    try {
        // Загрузка модуля
        module = torch::jit::load(path, device);
        
        // Преобразование к FormulaModel
        std::shared_ptr<FormulaModel> model;
        torch::load(model, path);
        
        return *model;
    }
    catch (const c10::Error& e) {
        std::cerr << "Error loading the model: " << e.what() << std::endl;
        throw;
    }
}

} // namespace formula_teacher
