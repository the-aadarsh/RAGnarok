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

void MongoDBClient::upsert(const std::string& id, const std::string& url, const std::string& title, const std::string& content, const std::vector<float>& embedding) {
    try {
        using namespace bsoncxx::builder::stream;
        auto embedding_array = bsoncxx::builder::basic::array{};
        for (float f : embedding) {
            embedding_array.append(static_cast<double>(f));
        }

        auto doc = document{}
            << "_id" << id
            << "url" << url
            << "title" << title
            << "content" << content
            << "embedding" << embedding_array
            << finalize;

        auto filter = document{}
            << "_id" << id
            << finalize;

        auto update = document{}
            << "$set" << doc
            << finalize;

        mongocxx::options::update options;
        options.upsert(true);

        collection_.update_one(filter.view(), update.view(), options);
    } catch (const std::exception& e) {
        std::cerr << "[MongoDB] Upsert failed: " << e.what() << std::endl;
    }
}

} // namespace db
} // namespace ragnarok
