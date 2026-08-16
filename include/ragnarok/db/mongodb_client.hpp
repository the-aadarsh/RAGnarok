#pragma once

#include <string>
#include <vector>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

namespace ragnarok {
namespace db {

struct SearchResult {
    std::string title;
    std::string content;
    std::string url;
    double score;
};

class MongoDBClient {
public:
    // Initialize the MongoDB client with URI, db name, and collection name
    MongoDBClient(const std::string& uri, const std::string& db_name, const std::string& coll_name);
    
    // Perform a vector search using an embedding
    std::vector<SearchResult> vector_search(const std::vector<float>& query_embedding, int limit = 3);

    // Upsert a document with its vector embedding
    void upsert(const std::string& id, const std::string& url, const std::string& title, const std::string& content, const std::vector<float>& embedding);

private:
    mongocxx::client client_;
    mongocxx::database db_;
    mongocxx::collection collection_;
};

} // namespace db
} // namespace ragnarok
