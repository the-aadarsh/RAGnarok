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
    prompt << "You are an AI assistant powered by a Retrieval-Augmented Generation (RAG) system.\n";
    prompt << "Use the following retrieved context to answer the user's question.\n";
    prompt << "If the answer is not contained within the context, state that you do not know.\n\n";
    prompt << "CONTEXT:\n";
    
    for (const auto& res : results) {
        prompt << "Title: " << res.title << "\n";
        prompt << "Content: " << res.content << "\n";
        prompt << "URL: " << res.url << "\n\n";
    }

    prompt << "USER QUESTION: " << query << "\n";
    prompt << "ANSWER:\n";
    return prompt.str();
}

std::string RAGEngine::ask(const std::string& query) {
    std::cout << "[RAG Engine] Embedding query..." << std::endl;
    auto query_embedding = llm_client_.embed_content(query);

    std::cout << "[RAG Engine] Retrieving relevant chunks from MongoDB Atlas..." << std::endl;
    auto search_results = db_client_.vector_search(query_embedding, 3); // Get top 3 chunks

    std::cout << "[RAG Engine] Retrieved " << search_results.size() << " chunks. Generating response..." << std::endl;
    std::string prompt = build_prompt(query, search_results);

    return llm_client_.generate_content(prompt);
}

} // namespace rag
} // namespace ragnarok
