#pragma once

#include <string>
#include <vector>

namespace ragnarok {
namespace llm {

class GeminiClient {
public:
    explicit GeminiClient(const std::string& api_key);

    // Generates a vector embedding for the given text using text-embedding-004
    std::vector<float> embed_content(const std::string& text);

    // Generates text response for the given prompt using gemini-1.5-flash
    std::string generate_content(const std::string& prompt);

private:
    std::string api_key_;
    std::string base_url_ = "https://generativelanguage.googleapis.com/v1beta/models/";
};

} // namespace llm
} // namespace ragnarok
