#include "ragnarok/db/mongodb_client.hpp"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <iostream>

namespace ragnarok {
namespace db {

MongoDBClient::MongoDBClient(const std::string& uri, const std::string& db_name, const std::string& coll_name)
    : client_(mongocxx::uri{uri}),
      db_(client_[db_name]),
      collection_(db_[coll_name]) {
    std::cout << "[MongoDB] Connected to database: " << db_name << ", collection: " << coll_name << std::endl;
}

std::vector<SearchResult> MongoDBClient::vector_search(const std::vector<float>& query_embedding, int limit) {
    std::vector<SearchResult> results;

    try {
        using namespace bsoncxx::builder::stream;

        // Build the embedding array for bson
        auto embedding_array = bsoncxx::builder::basic::array{};
        for (float f : query_embedding) {
            embedding_array.append(static_cast<double>(f));
        }

        // Construct the vectorSearch pipeline stage
        // Note: MongoDB Atlas requires the index name. Ensure you have created 
        // a vector search index named "vector_index" in Atlas on the 'embedding' field.
        auto vector_search_doc = document{}
            << "$vectorSearch" << open_document
                << "index" << "vector_index"
                << "path" << "embedding"
                << "queryVector" << embedding_array
                << "numCandidates" << (limit * 10)
                << "limit" << limit
            << close_document
            << finalize;

        // Project the required fields and compute the score
        auto project_doc = document{}
            << "$project" << open_document
                << "_id" << 0
                << "title" << 1
                << "content" << 1
                << "url" << 1
                << "score" << open_document
                    << "$meta" << "vectorSearchScore"
                << close_document
            << close_document
            << finalize;

        // Create the pipeline
        mongocxx::pipeline pipeline;
        pipeline.append_stage(vector_search_doc.view());
        pipeline.append_stage(project_doc.view());

        // Execute the pipeline
        auto cursor = collection_.aggregate(pipeline, mongocxx::options::aggregate{});

        for (auto&& doc : cursor) {
            SearchResult res;
            if (doc["title"]) {
                res.title = std::string(doc["title"].get_string().value);
            }
            if (doc["content"]) {
                res.content = std::string(doc["content"].get_string().value);
            }
            if (doc["url"]) {
                res.url = std::string(doc["url"].get_string().value);
            }
            if (doc["score"]) {
                res.score = doc["score"].get_double().value;
            }
            results.push_back(res);
        }

    } catch (const std::exception& e) {
        std::cerr << "[MongoDB] Vector search failed: " << e.what() << std::endl;
    }

    return results;
}

} // namespace db
} // namespace ragnarok
