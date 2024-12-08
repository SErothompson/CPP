#include <iostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <map>

std::vector<std::string> tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    boost::split(tokens, text, boost::is_any_of(" \t\n,.!?"), boost::token_compress_on);
    return tokens;
}

std::map<std::string, int> calculateWordFrequency(const std::vector<std::string>& tokens) {
    std::map<std::string, int> wordFreq;
    for (const auto& token : tokens) {
        if (!token.empty()) {
            ++wordFreq[token];
        }
    }
    return wordFreq;
}

int main() {
    std::string text = "Hello, world! Welcome to the world of NLP using C++.";
    std::vector<std::string> tokens = tokenize(text);

    std::map<std::string, int> wordFreq = calculateWordFrequency(tokens);

    std::cout << "Word Frequencies:" << std::endl;
    for (const auto& [word, freq] : wordFreq) {
        std::cout << word << ": " << freq << std::endl;
    }

    return 0;
}