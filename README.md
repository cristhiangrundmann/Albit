# Albit
Search engine project. It searches over the English portion of the Wikicorpus from [https://www.cs.upc.edu/~nlp/wikicorpus/].

# Construction stages
These are the major stages of the engine development

## Basic formatting
  - [x] Downloading the *raw* version of the corpus
  - [x] Renaming the files to `db*` from 0 to 163
  - [x] Removing both *end of article* markers and replacing the *begin of article* to `#doc "title"` (No quotes)
  
 ## Data Structures generation
  - [x] Storing *offset* and *title* data for easier lookup
  - [x] Generating the trie of the entire *corpus* along with the result lists(sorted by weight)
  - [x] Storing the serialized *trie* and *result lists*
 
 ## Simple application
  - [x] Loading the *serialized* data for user searches
  - [x] Word searching
  - [x] Document listings
  - [ ] Merging results
  - [ ] Document viewer
  
 ## Web application
    - TODO
    
 ## Preparing the environment for using the search engine
  1. Download or clone this repository
  2. Download the elaborate database avaliable [here](https://gvmail-my.sharepoint.com/:u:/g/personal/b39346_fgv_edu_br/Edaj5zg-ywBJkrj6vZu7fb8Bx6L2I4RrPFOq4F2evpu6Xg)
  3. Extract the database inside the repository folder so as to overwrite the database folders
  
 #### For prompt using
  4. Compile and run search.cpp to search
  
    `g++ -o search search.cpp`
    
  5. Run search and enjoy the engine
  
    `./search`

 #### For web interface using
  4. Compile test_client to /interface/client with argument -lrt (because of the shared memory)
  
    `g++ -o interface/client test_client.cpp -lrt`
    
  5. Compile test_server.cpp to server with argument -lrt
    
    `g++ -o server test_server.cpp -lrt`
    
  6. Install and configure a server emulator. We used Apache and PHP on Linux, avaliable as:
  
    `sudo apt install apache2`
    `sudo apt install libapache2-mod-php`
    
  7. Configure server emulator's directory to the search engine interface folder. For more information about Apache's direcotory take a look at https://shorturl.at/djAKP
  
  8. Run server
  
    `./server`
    
  9. Access the server adress in your browser and enjoy the search engine
