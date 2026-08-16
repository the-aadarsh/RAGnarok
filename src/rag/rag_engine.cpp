#include "ragnarok/rag/rag_engine.hpp"
#include <iostream>
#include <sstream>

namespace ragnarok {
namespace rag {

RAGEngine::RAGEngine(ragnarok::db::MongoDBClient& db_client, ragnarok::llm::GeminiClient& llm_client)
    : db_client_(db_client),
      llm_client_(llm_client) {
}

std::string RAGEngine::build_prompt(const std::string& query, const std::vector<ragnarok::db::SearchResult>& results) {
    std::ostringstream prompt;
    prompt << "You are an AI assistant.\n\n";
    prompt << "INSTRUCTIONS:\n";
    prompt << "- Synthesize the retrieved context below into a clear answer.\n";
    prompt << "- Use markdown formatting for readability.\n";
    prompt << "- You must ONLY answer questions using the provided context. If the answer is not contained in the context, simply reply: \"I'm sorry, but I can only answer questions based on the provided context.\" DO NOT attempt to answer from your general knowledge or explain why.\n";
    prompt << "- Cite the source URL when referencing specific information.\n\n";
    prompt << "---\n\n";
    prompt << "RETRIEVED CONTEXT:\n\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        prompt << "[" << (i + 1) << "] Title: " << results[i].title << "\n";
        prompt << "    Content: " << results[i].content << "\n";
        prompt << "    Source: " << results[i].url << "\n\n";
    }

    if (!history_.empty()) {
        prompt << "CONVERSATION HISTORY:\n";
        for (const auto& h : history_) {
            prompt << "User: " << h.first << "\n";
            prompt << "Assistant: " << h.second << "\n";
        }
        prompt << "\n";
    }

    prompt << "---\n\n";
    prompt << "USER QUESTION: " << query << "\n\n";
    prompt << "Answer:\n";

    return prompt.str();
}

std::string RAGEngine::ask(const std::string& query) {
    auto query_embedding = llm_client_.embed_content(query);
    auto search_results = db_client_.vector_search(query_embedding, 5);
    
    std::string prompt = build_prompt(query, search_results);
    std::string answer = llm_client_.generate_content(prompt);
    
    history_.push_back({query, answer});
    return answer;
}

} // namespace rag
} // namespace ragnarok
