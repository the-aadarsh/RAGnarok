#pragma once

#include "ragnarok/db/mongodb_client.hpp"
#include "ragnarok/llm/gemini_client.hpp"
#include <string>
#include <vector>
#include <utility>

namespace ragnarok {
namespace rag {

class RAGEngine {
public:
    RAGEngine(ragnarok::db::MongoDBClient& db_client, ragnarok::llm::GeminiClient& llm_client);

    std::string ask(const std::string& query);

private:
    std::string build_prompt(const std::string& query, const std::vector<ragnarok::db::SearchResult>& results);

    ragnarok::db::MongoDBClient& db_client_;
    ragnarok::llm::GeminiClient& llm_client_;
    std::vector<std::pair<std::string, std::string>> history_;
};

} // namespace rag
} // namespace ragnarok
