# RAGnarok

RAGnarok is a simple Retrieval-Augmented Generation (RAG) engine written in C++. It uses the Gemini API for LLM generation and embeddings, and MongoDB Atlas for vector search.

The project avoids heavy frameworks like LangChain in favor of a direct, low-level approach using standard C++ tools.

## Tech Stack
- C++17
- CMake & vcpkg (dependency management)
- `cpr` for HTTP requests
- `nlohmann/json` for JSON parsing
- `mongocxx` for MongoDB connections

## Prerequisites
- C++ Compiler (GCC 9+, Clang 10+, or MSVC 19.29+)
- CMake (3.20+)
- vcpkg
- MongoDB Atlas Account with a Vector Search Index named `vector_index` on `ragnarok.knowledge_base`
- Gemini API Key

### MongoDB Index Config
```json
{
  "fields": [
    {
      "numDimensions": 768,
      "path": "embedding",
      "similarity": "cosine",
      "type": "vector"
    }
  ]
}
```

## Setup & Build

1. **Clone the repo**
   ```bash
   git clone https://github.com/the-aadarsh/RAGnarok.git
   cd RAGnarok
   ```

2. **Configure environment variables**
   Create a `.env` file in the root directory:
   ```env
   GEMINI_API_KEY=your_gemini_api_key
   MONGO_URI=mongodb+srv://user:pass@cluster.mongodb.net/?retryWrites=true&w=majority
   ```

3. **Build the project**
   Make sure you provide the correct path to your `vcpkg.cmake` toolchain file.

   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/path/to/your/vcpkg/scripts/buildsystems/vcpkg.cmake
   cmake --build . --config Release
   ```

## Usage

After building, you can run the executable from the project root:

```bash
.\build\Release\ragnarok.exe
```

## Data Ingestion

There is a Python script included to help populate the MongoDB database with data.

```bash
cd scripts
pip install -r requirements.txt
python ingest_w3schools.py
```