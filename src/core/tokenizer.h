#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <fstream>

namespace formula_teacher {

class FormulaTokenizer {
public:
    // Конструктор для создания нового токенизатора
    FormulaTokenizer();
    
    // Конструктор для загрузки существующего словаря
    explicit FormulaTokenizer(const std::string& vocab_path);
    
    // Токенизация текста с формулами
    std::vector<int> tokenize(const std::string& text);
    
    // Детокенизация - превращение токенов в текст
    std::string detokenize(const std::vector<int>& tokens);
    
    // Создание словаря из текстового корпуса
    void build_vocabulary(const std::string& corpus_path, int max_vocab_size = 50000);
    
    // Сохранение словаря
    void save_vocabulary(const std::string& vocab_path);
    
    // Получить размер словаря
    int vocab_size() const { return token_to_id.size(); }
    
    // Специальные токены
    enum SpecialTokens {
        PAD = 0,   // Padding
        SOS = 1,   // Start of sequence
        EOS = 2,   // End of sequence
        UNK = 3    // Unknown
    };
    
private:
    // Разделение текста на токены
    std::vector<std::string> split_text(const std::string& text);
    
    // Обработка математических формул
    std::string process_formula(const std::string& formula);
    
    // Словари для преобразования токенов в индексы и обратно
    std::unordered_map<std::string, int> token_to_id;
    std::unordered_map<int, std::string> id_to_token;
    
    // Регулярные выражения для поиска формул
    std::regex formula_regex;
};

} // namespace formula_teacher
