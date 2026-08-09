import os
import requests
from bs4 import BeautifulSoup
from google import genai
from google.genai import types
from pymongo import MongoClient
from pymongo.server_api import ServerApi
from dotenv import load_dotenv
import time
import uuid

# Load environment variables from the .env file in the current directory
load_dotenv()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
MONGO_URI = os.getenv("MONGO_URI")

if not GEMINI_API_KEY:
    print("Error: GEMINI_API_KEY is not set.")
    exit(1)

if not MONGO_URI:
    print("Error: MONGO_URI is not set. Please provide the MongoDB connection string.")
    exit(1)

# Configure Gemini
gemini_client = genai.Client(api_key=GEMINI_API_KEY)

# Configure MongoDB
client = MongoClient(MONGO_URI, server_api=ServerApi('1'))
try:
    client.admin.command('ping')
    print("Pinged your deployment. You successfully connected to MongoDB!")
except Exception as e:
    print(f"MongoDB connection failed: {e}")
    exit(1)

db = client["ragnarok"]
collection = db["knowledge_base"]

BASE_URL = "https://www.w3schools.com/python/"
START_URL = f"{BASE_URL}default.asp"

def get_page_links():
    """Scrape the left sidebar to get all Python tutorial links."""
    print(f"Fetching links from {START_URL}...")
    headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'}
    response = requests.get(START_URL, headers=headers)
    response.raise_for_status()
    
    soup = BeautifulSoup(response.text, "html.parser")
    sidebar = soup.find("div", id="leftmenuinnerinner")
    
    links = []
    if sidebar:
        for a_tag in sidebar.find_all("a", href=True):
            href = a_tag["href"]
            if href.startswith("python_") or href == "default.asp":
                links.append(BASE_URL + href)
                
    # Remove duplicates but preserve order
    seen = set()
    unique_links = []
    for link in links:
        if link not in seen:
            unique_links.append(link)
            seen.add(link)
            
    print(f"Found {len(unique_links)} pages to scrape.")
    return unique_links

def scrape_page(url):
    """Scrape the main content of a W3Schools page."""
    print(f"Scraping {url}...")
    try:
        headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'}
        response = requests.get(url, headers=headers)
        response.raise_for_status()
    except Exception as e:
        print(f"Failed to fetch {url}: {e}")
        return []
        
    soup = BeautifulSoup(response.text, "html.parser")
    main_content = soup.find("div", id="main")
    
    if not main_content:
        return []
        
    # Remove unnecessary elements
    for element in main_content.find_all(["hr", "div"], class_=["w3-clear", "nextprev"]):
        element.decompose()
        
    # Extract title
    h1 = main_content.find("h1")
    title = h1.text.strip() if h1 else "Unknown Title"
    
    # Simple chunking strategy: split by <h2> tags to get distinct sections
    chunks = []
    current_chunk = []
    
    for element in main_content.children:
        if element.name == "h2":
            if current_chunk:
                chunks.append("\n".join(current_chunk).strip())
            current_chunk = [element.get_text(strip=True)]
        elif element.name in ["p", "h3", "h4", "ul", "ol", "div"]:
            text = element.get_text(separator=" ", strip=True)
            if text and not text.startswith("ADVERTISEMENT"):
                current_chunk.append(text)
                
    if current_chunk:
        chunks.append("\n".join(current_chunk).strip())
        
    # If no H2 tags, just group everything
    if not chunks and current_chunk:
        chunks = ["\n".join(current_chunk).strip()]
        
    # Filter out empty chunks and chunks that are too small
    valid_chunks = [c for c in chunks if len(c) > 50]
    
    return title, valid_chunks

def get_embedding(text):
    """Generate embedding using Gemini API."""
    try:
        result = gemini_client.models.embed_content(
            model="gemini-embedding-2",
            contents=text,
            config=types.EmbedContentConfig(task_type="RETRIEVAL_DOCUMENT")
        )
        return result.embeddings[0].values
    except Exception as e:
        print(f"Embedding failed: {e}")
        return None

def process_and_ingest():
    links = get_page_links()
    total_chunks_inserted = 0
    
    for link in links:
        result = scrape_page(link)
        if not result:
            continue
            
        title, chunks = result
        
        for i, chunk in enumerate(chunks):
            print(f"Embedding chunk {i+1}/{len(chunks)} of '{title}'...")
            embedding = get_embedding(chunk)
            
            if embedding:
                doc = {
                    "_id": str(uuid.uuid4()),
                    "url": link,
                    "title": title,
                    "content": chunk,
                    "embedding": embedding
                }
                
                try:
                    collection.insert_one(doc)
                    total_chunks_inserted += 1
                except Exception as e:
                    print(f"Failed to insert to MongoDB: {e}")
            
            # Rate limiting sleep for Gemini API
            time.sleep(1)
            
    print(f"\nIngestion complete! Successfully inserted {total_chunks_inserted} chunks into MongoDB.")

if __name__ == "__main__":
    print("Starting W3Schools Python Ingestion Pipeline...")
    process_and_ingest()
