<div align="center">

# RAGnarok.cpp

## A Bare-Metal Retrieval-Augmented Generation Engine in C++17

---

**Submitted by:** Aadarsh Kumar Gupta

**Institution:** Kings College

**Degree Program:** Bachelor of Science in Computer Science (BSCS)

**Subject:** Artificial Intelligence & Machine Learning

**Submission Date:** August 2026

---

</div>

---

## Abstract

This proposal outlines the design and implementation of **RAGnarok.cpp**, a high-performance Retrieval-Augmented Generation (RAG) engine built entirely in C++17. Unlike the prevailing trend of Python-centric AI tooling and high-level orchestration frameworks such as LangChain, RAGnarok.cpp is engineered close to the hardware, achieving minimal latency and maximum resource efficiency. The system integrates directly with Google's Gemini API for text embedding and large language model (LLM) inference, and leverages MongoDB Atlas Vector Search as the remote, cloud-hosted vector database. The core research question this project addresses is: *Can a production-grade RAG pipeline be implemented natively in C++ without sacrificing developer ergonomics or system correctness?* This project will yield a fully functional, command-line interactive RAG chatbot as its primary deliverable, completed within a one-week development sprint.

---

## 1. Problem Statement

Retrieval-Augmented Generation has emerged as the dominant architectural pattern for grounding large language model outputs in factual, domain-specific knowledge (Lewis et al., 2020). However, the current ecosystem of RAG tooling is almost exclusively built upon Python-based stacks. This introduces several systemic limitations:

- **Runtime Overhead:** Python's interpreted execution model and the Global Interpreter Lock (GIL) impose significant latency penalties on I/O-bound and CPU-bound operations, which are both prevalent in a RAG pipeline.
- **Memory Inefficiency:** High-level frameworks abstract away memory allocation, preventing fine-grained control over embedding buffer sizes, connection pool lifetimes, and BSON document serialization.
- **Dependency Bloat:** Frameworks like LangChain introduce hundreds of transitive dependencies, making deployment environments fragile and build times unpredictable.
- **Ecosystem Gap:** There is a notable absence of a production-ready, modular RAG implementation in C++, despite C++ being the language of choice for latency-critical backend systems (game servers, real-time trading platforms, inference runtimes).

This project directly addresses this gap by implementing a clean, modular, and dependency-minimal RAG engine in C++17.

---

## 2. Proposed Solution & Objectives

**RAGnarok.cpp** proposes a native C++ implementation of a complete RAG pipeline, decomposed into three independent, reusable modules:

1. **`GeminiClient`** — A type-safe HTTP client module responsible for interfacing with the Google Gemini REST API. It handles both the `embedContent` endpoint (for vector generation) and the `generateContent` endpoint (for LLM inference).
2. **`MongoDBClient`** — A module wrapping the official `mongocxx` driver, responsible for upserting embedded documents into MongoDB Atlas and executing `$vectorSearch` aggregation pipelines for nearest-neighbor retrieval.
3. **`RagEngine`** — The core orchestrator that composes the embedding, retrieval, and generation stages into a single, coherent query-response loop.

### Objectives

- To design and implement a functional RAG pipeline without any Python or high-level AI framework dependency.
- To achieve a clean, modular architecture following C++ best practices (header/source separation, RAII, const-correctness).
- To integrate with production-grade external services: Google Gemini API and MongoDB Atlas.
- To validate the implementation through a working interactive CLI chatbot capable of answering questions from a custom knowledge base.

---

## 3. System Architecture & Design

The RAGnarok.cpp pipeline follows a two-phase architecture:

### Phase 1 — Ingestion Pipeline

```
Raw Text Documents
       │
       ▼
 GeminiClient::embed()
 [POST /embedContent → Gemini API]
       │
       ▼
 Float Vector (768-dim)
       │
       ▼
 MongoDBClient::upsert()
 [Insert doc + vector into Atlas Collection]
```

### Phase 2 — Retrieval & Generation Pipeline

```
User Query (std::string)
       │
       ▼
 GeminiClient::embed()
 [Embed the query on-the-fly]
       │
       ▼
 MongoDBClient::vectorSearch()
 [MongoDB $vectorSearch → top-k chunks]
       │
       ▼
 Context Assembly (RagEngine)
 [Concatenate retrieved chunks into prompt]
       │
       ▼
 GeminiClient::generate()
 [POST /generateContent → Gemini LLM]
       │
       ▼
 Final Grounded Response → stdout
```

### Project File Structure

```
RAGnarok/
├── CMakeLists.txt
├── vcpkg.json
├── include/
│   └── ragnarok/
│       ├── db/mongodb_client.hpp
│       ├── llm/gemini_client.hpp
│       └── rag/rag_engine.hpp
└── src/
    ├── main.cpp
    ├── db/mongodb_client.cpp
    ├── llm/gemini_client.cpp
    └── rag/rag_engine.cpp
```

---

## 4. Tech Stack & Tools

| Component | Technology | Justification |
|---|---|---|
| Core Language | C++17 | Performance, memory control, zero-cost abstractions |
| Build System | CMake 3.20+ | Industry-standard, cross-platform, IDE-agnostic |
| Dependency Manager | vcpkg (Manifest Mode) | Reproducible builds, hermetic dependency graph |
| HTTP Client | `cpr` (C++ Requests) | Thin, curl-based wrapper with a clean modern API |
| JSON Parsing | `nlohmann/json` | Header-only, expressive, de-facto standard in C++ |
| Database Driver | `mongocxx` (Official MongoDB C++ Driver) | Official, maintained, full Atlas feature support |
| LLM & Embeddings | Google Gemini API | State-of-the-art embedding and generation quality |
| Vector Database | MongoDB Atlas Vector Search | Managed, scalable, no separate infrastructure needed |

---

## 5. Implementation Plan

Given the one-week development timeline, work is structured into focused daily milestones:

| Day | Milestone | Deliverable |
|---|---|---|
| **Day 1** | Project scaffolding | CMake + vcpkg configured, dependencies resolving |
| **Day 2** | `GeminiClient` — Embedding | `embed()` method hitting Gemini API, returning `std::vector<float>` |
| **Day 3** | `MongoDBClient` — Storage | Atlas connection, document upsert with vector field |
| **Day 4** | `MongoDBClient` — Retrieval | `$vectorSearch` aggregation pipeline returning top-k chunks |
| **Day 5** | `GeminiClient` — Generation | `generate()` method accepting prompt, returning `std::string` |
| **Day 6** | `RagEngine` — Orchestration | Full end-to-end pipeline wired and functional |
| **Day 7** | Testing, CLI polish & documentation | Working interactive chatbot, finalized README |

---

## 6. Expected Outcomes & Deliverables

Upon completion of the one-week sprint, the following deliverables are expected:

- **Primary Deliverable:** A fully functional, command-line interactive RAG chatbot (`ragnarok` binary) capable of ingesting a custom text corpus, embedding it via Gemini, storing it in MongoDB Atlas, and answering user queries using the retrieved context.
- **Source Code:** A clean, well-commented C++17 codebase hosted on GitHub under the repository `the-aadarsh/RAGnarok`, adhering to the modular architecture described above.
- **Build Artifacts:** A reproducible CMake + vcpkg build system that compiles successfully on Windows (MSVC/Clang) and Linux (GCC/Clang).
- **Documentation:** A comprehensive `README.md` covering architecture, prerequisites, quick-start instructions, and usage examples.

---

## 7. References

Lewis, P., Perez, E., Piktus, A., Petroni, F., Karpukhin, V., Goyal, N., Küttler, H., Lewis, M., Yih, W., Rocktäschel, T., Riedel, S., & Kiela, D. (2020). Retrieval-augmented generation for knowledge-intensive NLP tasks. *Advances in Neural Information Processing Systems*, *33*, 9459–9474. https://proceedings.neurips.cc/paper/2020/hash/6b493230205f780e1bc26945df7481e5-Abstract.html

MongoDB, Inc. (2024). *Atlas Vector Search documentation*. MongoDB. https://www.mongodb.com/docs/atlas/atlas-vector-search/

Google DeepMind. (2024). *Gemini API documentation: Embeddings*. Google. https://ai.google.dev/gemini-api/docs/embeddings

Lospinoso, J. (2019). *C++ crash course: A fast-paced introduction*. No Starch Press.

International Organization for Standardization. (2017). *ISO/IEC 14882:2017 — Programming languages: C++*. ISO. https://www.iso.org/standard/68564.html

---

<div align="center">

*Submitted in partial fulfillment of the requirements for the Bachelor of Science in Computer Science at Kings College.*

</div>
