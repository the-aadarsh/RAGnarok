<div align="center">
  <h1>⚡ RAGnarok.cpp</h1>
  <p><strong>A bare-metal, high-performance Retrieval-Augmented Generation engine engineered in pure C++.</strong></p>

  [![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=for-the-badge&logo=c%2B%2B)](https://en.wikipedia.org/wiki/C%2B%2B17)
  [![Build Passing](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=for-the-badge&logo=cmake)](https://github.com/the-aadarsh/RAGnarok/actions)
  [![License](https://img.shields.io/badge/License-MIT-blue.svg?style=for-the-badge)](LICENSE)
</div>

<br>

No Python. No LangChain. No bloated abstractions. **RAGnarok.cpp** is a ruthlessly efficient, low-latency Retrieval-Augmented Generation (RAG) architecture built close to the metal. It interfaces directly with the Gemini API for intelligence and leverages MongoDB Atlas Vector Search for high-dimensional semantic retrieval.

Designed for systems where memory footprint and execution speed are critical.

---

## 🏗️ The Architecture

RAGnarok operates on a highly optimized pipeline, executing the entire RAG workflow natively in C++:

1. **Ingestion & Embedding**: Raw text is aggressively parsed and pushed to the **Gemini API** for high-dimensional vector embeddings.
2. **Vector Storage**: Embeddings are serialized and shipped to **MongoDB Atlas**, where they are indexed for rapid semantic similarity search using `$vectorSearch`.
3. **Retrieval**: User queries are embedded on the fly. RAGnarok executes a blistering fast nearest-neighbor search against the Atlas cluster to retrieve the most contextually relevant chunks.
4. **Generation**: The retrieved context is formatted and fed back to the **Gemini LLM**, which synthesizes an accurate, hallucination-free response.

## ⚙️ Tech Stack

RAGnarok is built on a battle-tested, modern C++ foundation:

* **Core**: Modern C++17/20
* **Build System**: CMake
* **Dependency Management**: vcpkg (Manifest Mode)
* **Networking**: `cpr` (C++ Requests - A lightweight curl wrapper)
* **Serialization**: `nlohmann/json` (First-class JSON for Modern C++)
* **Database**: `mongocxx` (Official MongoDB C++ Driver)

## 📋 Prerequisites

Before you compile, ensure your environment meets the strict requirements:

* **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 19.29+
* **CMake**: Version 3.20 or newer
* **vcpkg**: Configured and integrated with CMake
* **MongoDB Atlas Account**: An active cluster with Vector Search configured
* **Gemini API Key**: For embedding generation and LLM inference

## 🚀 Quick Start

Drop into your terminal and execute the following to clone and compile the engine:

```bash
# Clone the repository
git clone https://github.com/the-aadarsh/RAGnarok.git
cd RAGnarok

# Configure environment variables
cat << EOF > .env
GEMINI_API_KEY=your_gemini_api_key
MONGO_URI=mongodb+srv://user:pass@cluster.mongodb.net/?retryWrites=true&w=majority
EOF

# Build the project using CMake and vcpkg
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[path/to/vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## 💻 Usage

Once compiled, invoke the RAGnarok binary to start the interactive CLI chatbot. The engine will handle the embedding, database querying, and LLM inference natively.

```bash
# Execute the binary
./ragnarok

> Initialize System... [OK]
> Connection to MongoDB Atlas... [ESTABLISHED]
> LLM Pipeline... [READY]

User: What is the primary architecture of RAGnarok?
RAGnarok: RAGnarok utilizes a direct integration between the Gemini API for embeddings/generation and MongoDB Atlas Vector Search for semantic retrieval, executing the entire pipeline in pure C++ without intermediate wrappers.
```

## 🧠 Building the "Brain" (Data Ingestion)

While the RAGnarok execution engine is built entirely in uncompromised C++, we've included an AI-powered Python ingestion pipeline to rapidly populate your vector database with knowledge. 

Located in `scripts/ingest_w3schools.py`, this custom scraper:
1. **Crawls** targeted sites (like the W3Schools Python tutorial) mimicking a real browser.
2. **Cleans & Chunks** the HTML into bite-sized, semantically logical pieces.
3. **Embeds** the chunks using the **Gemini API** (`text-embedding-004`).
4. **Ingests** everything directly into your **MongoDB Atlas** cluster.

To run it and build your own "Brain":
```bash
cd scripts
pip install -r requirements.txt
python ingest_w3schools.py
```

## 👨‍💻 Author

**Aadarsh Kumar Gupta**

Driven by a focus on high-performance backend engineering and low-level systems architecture. Building tools that prioritize execution speed, minimal overhead, and uncompromised scalability. 

---
*If you need performance, you write it in C++.*