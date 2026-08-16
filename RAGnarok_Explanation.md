# RAGnarok Project: Defense Q&A and Codebase Explanation

This document contains direct answers to your panel's questions, followed by a comprehensive breakdown of every file and function in the RAGnarok C++ codebase.

---

## Part 1: Answers to the Questions

**1. How did you use the LLM in C++?**
*(kasari llm yse gareko chhau)*
There is no official Google Gemini SDK for C++. To use the LLM, I integrated a networking library called `cpr` (C++ Requests, which is a wrapper around the famous `libcurl`). I used this library to manually construct HTTP POST requests containing JSON payloads and sent them directly to Gemini's REST API endpoints (`generativelanguage.googleapis.com`).

**2. And how did you prepare the knowledge base... did you use file handling?**
*(ani kasari chai knowledge base ready garako chau... file handling gareko ho?)*
No, I did not use local file handling (like reading from `.txt` or `.csv` files locally). The knowledge base was prepared using a separate Python script (`scripts/ingest_w3schools.py`). This script scrapes data from the web, generates vector embeddings for that data using the Gemini API, and inserts it directly into a **MongoDB Atlas** cloud database. The C++ engine then connects to that live cloud database to retrieve the knowledge.

**3. How does it make API calls?**
*(kasari usle api hit garcha?)*
It makes API calls using the `cpr::Post` method from the C++ Requests library. It sets the HTTP Header `Content-Type: application/json` and passes the API key in the `x-goog-api-key` header. The body of the request is a JSON object dynamically constructed using the `nlohmann/json` library, which contains the user's prompt or the text to be embedded.

**4. How are the answers displayed in the CLI?**
*(kasari cli ma answer aucha?)*
In the `main.cpp` file, there is an infinite `while(true)` loop. It uses standard C++ input `std::getline(std::cin, query)` to capture what the user types. This query is passed to the `RAGEngine`, which does all the heavy lifting (embedding, searching, generating). The engine returns the final answer as a standard `std::string`, which is then printed to the terminal using `std::cout << answer`.

**5. What happens if we ask whatever we want (random questions)?**
*(hamle aru j man lagyo tehi sodhyo vane k huncha?)*
Because the system is built with a strictly engineered "System Prompt," it is heavily constrained. If you ask a random question completely unrelated to the knowledge base, the LLM will refuse to answer it using its general knowledge. 

**6. For example: If we ask a question outside the scope of those documents, what kind of answer will we get?**
*(tyo documents bahira ko question sodhyk vane... k answer aucha?)*
If the vector search retrieves context from MongoDB but the context doesn't contain the answer to your random question, the AI will strictly reply with: *"I'm sorry, but I can only answer questions based on the provided context."* It is explicitly programmed to never hallucinate or guess.

---

## Part 2: Codebase & Function Explanation

Here is a breakdown of what each C++ file does and the purpose of the functions inside them.

### 1. `src/main.cpp`
**Purpose:** This is the entry point of the application. It ties all the components together, loads configurations, and runs the interactive terminal loop.
* **`load_env(filepath)`**: A custom, lightweight function that reads the `.env` file line by line to extract your `MONGO_URI` and `GEMINI_API_KEY` so you don't have to hardcode passwords in the C++ source.
* **`main()`**: 
  1. Initializes the MongoDB C++ driver (`mongocxx::instance`).
  2. Loads the environment variables.
  3. Creates the `MongoDBClient` and `GeminiClient` objects.
  4. Passes those clients into the `RAGEngine`.
  5. Starts the `while(true)` loop to continually ask the user for input and print the AI's response.

### 2. `src/llm/gemini_client.cpp`
**Purpose:** Handles all direct network communication with the Google Gemini REST API.
* **`GeminiClient(api_key)`**: Constructor that stores your API key.
* **`embed_content(text)`**: Takes a string of text, constructs a JSON payload, and makes a POST request to the `gemini-embedding-2` model. It parses the JSON response and returns a `std::vector<float>` (an array of numbers representing the semantic meaning of the text).
* **`generate_content(prompt)`**: Takes the massive, final engineered prompt (which includes the user's question AND the MongoDB context), sends it to the `gemini-3.6-flash` model, parses the response, and returns the generated text answer.

### 3. `src/db/mongodb_client.cpp`
**Purpose:** Handles all communication with the MongoDB Atlas cloud database.
* **`MongoDBClient(...)`**: Establishes a TCP connection to your MongoDB cluster and selects the `ragnarok` database and `knowledge_base` collection.
* **`vector_search(query_embedding, limit)`**: This is the most complex database function. It takes the array of floats (the embedded user query) and constructs a MongoDB Aggregation Pipeline using `$vectorSearch`. This tells MongoDB Atlas to perform a mathematically intensive "Cosine Similarity" search across all documents to find the chunks of text most related to the user's query. It returns a list of `SearchResult` objects.
* **`upsert(...)`**: A utility function to insert or update documents in the database from C++ (though primarily data ingestion is handled by the Python script).

### 4. `src/rag/rag_engine.cpp`
**Purpose:** The "Brain" of the operation. It orchestrates the flow of data between the user, the LLM client, and the DB client.
* **`RAGEngine(...)`**: Constructor that takes references to the database and LLM clients.
* **`build_prompt(query, results)`**: This function creates the actual "System Prompt". It combines strict instructions (e.g., "ONLY answer based on the provided context"), the chunks of text retrieved from MongoDB, the previous chat history, and the user's new question into one massive string.
* **`ask(query)`**: The main orchestration function called by `main.cpp`. It follows this exact sequence:
  1. Calls `llm_client.embed_content(query)` to turn the user's question into numbers.
  2. Calls `db_client.vector_search(...)` using those numbers to find relevant context.
  3. Calls `build_prompt(...)` to stitch the context and query together.
  4. Calls `llm_client.generate_content(...)` to get the final answer.
  5. Saves the question and answer into the chat history and returns the answer.
