cmake_minimum_required(VERSION 3.20)
project(ragnarok CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies (provided by vcpkg)
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

# Include directories
include_directories(include)

# Define source files
set(SOURCES
    src/main.cpp
    src/db/mongodb_client.cpp
    src/llm/gemini_client.cpp
    src/rag/rag_engine.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Target Include Directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    cpr::cpr
    nlohmann_json::nlohmann_json
    mongo::mongocxx_shared
    mongo::bsoncxx_shared
)
