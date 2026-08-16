#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <mongocxx/instance.hpp>
#include "ragnarok/db/mongodb_client.hpp"
#include "ragnarok/llm/gemini_client.hpp"
#include "ragnarok/rag/rag_engine.hpp"

std::unordered_map<std::string, std::string> load_env(const std::string& filepath) {
    std::unordered_map<std::string, std::string> env;
    std::ifstream file(filepath);
    if (!file.is_open()) return env;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);
            if (!value.empty() && value.back() == '\r') {
                value.pop_back();
            }
            env[key] = value;
        }
    }
    return env;
}

int main() {
    std::cout << "RAGnarok.cpp Engine" << std::endl;
    std::cout << "Initializing..." << std::endl;

    mongocxx::instance inst{};

    auto env = load_env(".env");
    if (env.find("MONGO_URI") == env.end() || env.find("GEMINI_API_KEY") == env.end()) {
        std::cerr << "Error: missing MONGO_URI or GEMINI_API_KEY in .env" << std::endl;
        return 1;
    }

    try {
        ragnarok::db::MongoDBClient db_client(env["MONGO_URI"], "ragnarok", "knowledge_base");
        ragnarok::llm::GeminiClient llm_client(env["GEMINI_API_KEY"]);
        ragnarok::rag::RAGEngine engine(db_client, llm_client);
        
        std::cout << "Ready.\n" << std::endl;

        std::string query;
        while (true) {
            std::cout << "User: ";
            if (!std::getline(std::cin, query) || query == "exit" || query == "quit") {
                break;
            }
            if (query.empty()) continue;

            std::string answer = engine.ask(query);
            std::cout << "RAGnarok: " << answer << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
