#include "ragnarok/rag/rag_engine.hpp"
#include <iostream>
#include <sstream>

namespace ragnarok {
namespace rag {

RAGEngine::RAGEngine(ragnarok::db::MongoDBClient& db_client, ragnarok::llm::GeminiClient& llm_client)
    : db_client_(db_client),
      llm_client_(llm_client) {
    std::cout << "[RAG Engine] Initialized with DB and LLM clients." << std::endl;
}

std::string RAGEngine::build_prompt(const std::string& query, const std::vector<ragnarok::db::SearchResult>& results) {
    std::ostringstream prompt;
    prompt << "You are RAGnarok, an expert AI assistant powered by a high-performance Retrieval-Augmented Generation engine.\n\n";
    prompt << "INSTRUCTIONS:\n";
    prompt << "- Synthesize the retrieved context below into a comprehensive, well-structured answer.\n";
    prompt << "- Write in a clear, engaging, and informative tone. Provide depth and insight, not just surface-level bullet points.\n";
    prompt << "- Where appropriate, include examples, explanations, and practical details.\n";
    prompt << "- Use markdown formatting for readability (headings, bold, code blocks, etc.).\n";
    prompt << "- If the context does not contain enough information, say so honestly, but share what you can infer.\n";
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
    prompt << "Provide a thorough, well-organized answer:\n";

    return prompt.str();
}

std::string RAGEngine::ask(const std::string& query) {
    std::cout << "[RAG Engine] Embedding query..." << std::endl;
    auto query_embedding = llm_client_.embed_content(query);

    std::cout << "[RAG Engine] Retrieving relevant chunks from MongoDB Atlas..." << std::endl;
    auto search_results = db_client_.vector_search(query_embedding, 5); // Get top 5 chunks

    std::cout << "[RAG Engine] Retrieved " << search_results.size() << " chunks. Generating response..." << std::endl;
    std::string prompt = build_prompt(query, search_results);

    std::string answer = llm_client_.generate_content(prompt);
    history_.push_back({query, answer});
    return answer;
}

} // namespace rag
} // namespace ragnarok
