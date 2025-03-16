#include "tokenizer.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace formula_teacher {

FormulaTokenizer::FormulaTokenizer() : formula_regex("\\$(.*?)\\$|\\\\\\[(.*?)\\\\\\]|\\\\begin\\{equation\\}(.*?)\\\\end\\{equation\\}") {
    // Инициализация специальных токенов
    token_to_id["<pad>"] = PAD;
    token_to_id["<sos>"] = SOS;
    token_to_id["<eos>"] = EOS;
    token_to_id["<unk>"] = UNK;
    
    id_to_token[PAD] = "<pad>";
    id_to_token[SOS] = "<sos>";
    id_to_token[EOS] = "<eos>";
    id_to_token[UNK] = "<unk>";
}

FormulaTokenizer::FormulaTokenizer(const std::string& vocab_path) : FormulaTokenizer() {
    std::ifstream file(vocab_path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть файл словаря: " + vocab_path);
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        int id;
        if (iss >> token >> id) {
            token_to_id[token] = id;
            id_to_token[id] = token;
        }
    }
}

std::vector<std::string> FormulaTokenizer::split_text(const std::string& text) {
    std::vector<std::string> tokens;
    
    // Находим формулы и заменяем их специальными токенами
    std::string processed_text = text;
    std::smatch match;
    
    while (std::regex_search(processed_text, match, formula_regex)) {
        // Добавляем текст до формулы
        std::string prefix = match.prefix().str();
        std::istringstream prefix_stream(prefix);
        std::string word;
        while (prefix_stream >> word) {
            tokens.push_back(word);
        }
        
        // Обрабатываем формулу
        std::string formula;
        for (size_t i = 1; i < match.size(); ++i) {
            if (match[i].matched) {
                formula = match[i].str();
                break;
            }
        }
        std::string processed_formula = process_formula(formula);
        tokens.push_back(processed_formula);
        
        // Продолжаем с текста после формулы
        processed_text = match.suffix().str();
    }
    
    // Добавляем оставшийся текст
    std::istringstream remaining_stream(processed_text);
    std::string word;
    while (remaining_stream >> word) {
        tokens.push_back(word);
    }
    
    return tokens;
}

std::string FormulaTokenizer::process_formula(const std::string& formula) {
    // Заменяем пробелы на специальные токены
    std::string processed = formula;
    std::replace(processed.begin(), processed.end(), ' ', '_');
    
    // Добавляем префикс для обозначения формулы
    return "<formula>" + processed + "</formula>";
}

std::vector<int> FormulaTokenizer::tokenize(const std::string& text) {
    std::vector<std::string> tokens = split_text(text);
    std::vector<int> token_ids;
    
    for (const auto& token : tokens) {
        if (token_to_id.find(token) != token_to_id.end()) {
            token_ids.push_back(token_to_id[token]);
        } else {
            token_ids.push_back(UNK);
        }
    }
    
    return token_ids;
}

std::string FormulaTokenizer::detokenize(const std::vector<int>& tokens) {
    std::string result;
    bool in_formula = false;
    
    for (int token_id : tokens) {
        if (token_id == EOS) break;
        if (token_id == SOS || token_id == PAD) continue;
        
        std::string token = (id_to_token.find(token_id) != id_to_token.end()) ? 
                            id_to_token[token_id] : "<unk>";
        
        if (token == "<formula>") {
            in_formula = true;
            result += " $";
        } else if (token == "</formula>") {
            in_formula = false;
            result += "$ ";
        } else {
            if (in_formula) {
                // В формулах заменяем подчеркивания на пробелы
                std::string processed = token;
                std::replace(processed.begin(), processed.end(), '_', ' ');
                result += processed;
            } else {
                result += " " + token;
            }
        }
    }
    
    return result;
}

void FormulaTokenizer::build_vocabulary(const std::string& corpus_path, int max_vocab_size) {
    std::ifstream file(corpus_path);
    if (!file) {
        throw std::runtime_error("Не удалось открыть корпус: " + corpus_path);
    }
    
    std::unordered_map<std::string, int> token_counts;
    std::string line;
    
    while (std::getline(file, line)) {
        auto tokens = split_text(line);
        for (const auto& token : tokens) {
            token_counts[token]++;
        }
    }
    
    // Сортировка токенов по частоте
    std::vector<std::pair<std::string, int>> sorted_tokens(token_counts.begin(), token_counts.end());
    std::sort(sorted_tokens.begin(), sorted_tokens.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Ограничение размера словаря
    int vocab_limit = std::min(max_vocab_size, static_cast<int>(sorted_tokens.size()));
    
    // Создание словаря
    int next_id = 4;  // Первые 4 индекса зарезервированы для специальных токенов
    for (int i = 0; i < vocab_limit; ++i) {
        const auto& token = sorted_tokens[i].first;
        if (token_to_id.find(token) == token_to_id.end()) {
            token_to_id[token] = next_id;
            id_to_token[next_id] = token;
            next_id++;
        }
    }
    
    std::cout << "Создан словарь размером: " << token_to_id.size() << " токенов" << std::endl;
}

void FormulaTokenizer::save_vocabulary(const std::string& vocab_path) {
    std::ofstream file(vocab_path);
    if (!file) {
        throw std::runtime_error("Не удалось создать файл словаря: " + vocab_path);
    }
    
    for (const auto& [token, id] : token_to_id) {
        file << token << " " << id << "\n";
    }
    
    std::cout << "Словарь сохранен в: " << vocab_path << std::endl;
}

} // namespace formula_teacher
