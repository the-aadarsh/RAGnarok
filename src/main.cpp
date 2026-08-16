#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <mongocxx/instance.hpp>
#include "ragnarok/db/mongodb_client.hpp"
#include "ragnarok/llm/gemini_client.hpp"
#include "ragnarok/rag/rag_engine.hpp"

// Simple .env parser to avoid adding a heavy dependency
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
    std::cout << "> Initialize System... [OK]" << std::endl;

    // Initialize MongoDB Driver exactly once
    mongocxx::instance inst{};

    // Load environment variables from .env
    auto env = load_env(".env");
    if (env.find("MONGO_URI") == env.end() || env.find("GEMINI_API_KEY") == env.end()) {
        std::cerr << "Error: MONGO_URI or GEMINI_API_KEY not found in .env" << std::endl;
        return 1;
    }

    std::string mongo_uri = env["MONGO_URI"];
    std::string gemini_key = env["GEMINI_API_KEY"];

    try {
        // Initialize clients
        ragnarok::db::MongoDBClient db_client(mongo_uri, "ragnarok", "knowledge_base");
        ragnarok::llm::GeminiClient llm_client(gemini_key);

        // Initialize RAG Engine (Connects DB and LLM)
        ragnarok::rag::RAGEngine engine(db_client, llm_client);
        std::cout << "> LLM Pipeline... [READY]\n" << std::endl;

        // Interactive CLI loop
        std::string query;
        while (true) {
            std::cout << "\nUser: ";
            if (!std::getline(std::cin, query) || query == "exit" || query == "quit") {
                break;
            }
            if (query.empty()) continue;

            std::string answer = engine.ask(query);
            std::cout << "RAGnarok: " << answer << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
