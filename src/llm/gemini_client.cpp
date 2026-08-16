#include "ragnarok/llm/gemini_client.hpp"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>

namespace ragnarok {
namespace llm {

using json = nlohmann::json;

GeminiClient::GeminiClient(const std::string& api_key) : api_key_(api_key) {}

std::vector<float> GeminiClient::embed_content(const std::string& text) {
    std::string url = base_url_ + "gemini-embedding-2:embedContent";

    json payload = {
        {"model", "models/gemini-embedding-2"},
        {"content", {
            {"parts", {
                {{"text", text}}
            }}
        }}
    };

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Header{{"Content-Type", "application/json"},
                                            {"x-goog-api-key", api_key_}},
                                cpr::Body{payload.dump()});

    if (r.status_code != 200) {
        throw std::runtime_error("Gemini API Error (Embedding): " + r.text);
    }

    json response_json = json::parse(r.text);
    
    std::vector<float> embedding;
    if (response_json.contains("embedding") && response_json["embedding"].contains("values")) {
        for (const auto& val : response_json["embedding"]["values"]) {
            embedding.push_back(val.get<float>());
        }
    } else {
        throw std::runtime_error("Unexpected JSON response structure from Gemini API (Embedding).");
    }

    return embedding;
}

std::string GeminiClient::generate_content(const std::string& prompt) {
    std::string url = base_url_ + "gemini-3.6-flash:generateContent";

    json payload = {
        {"contents", {
            {
                {"parts", {
                    {{"text", prompt}}
                }}
            }
        }}
    };

    cpr::Response r = cpr::Post(cpr::Url{url},
                                cpr::Header{{"Content-Type", "application/json"},
                                            {"x-goog-api-key", api_key_}},
                                cpr::Body{payload.dump()});

    if (r.status_code != 200) {
        throw std::runtime_error("Gemini API Error (Generation): " + r.text);
    }

    json response_json = json::parse(r.text);
    
    if (response_json.contains("candidates") && response_json["candidates"].is_array() && !response_json["candidates"].empty()) {
        auto& candidate = response_json["candidates"][0];
        if (candidate.contains("content") && candidate["content"].contains("parts") && candidate["content"]["parts"].is_array() && !candidate["content"]["parts"].empty()) {
            return candidate["content"]["parts"][0]["text"].get<std::string>();
        }
    }

    throw std::runtime_error("Unexpected JSON response structure from Gemini API (Generation).");
}

} // namespace llm
} // namespace ragnarok
