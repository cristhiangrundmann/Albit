# Albit
Search engine project. It searches over the English portion of the Wikicorpus from [https://www.cs.upc.edu/~nlp/wikicorpus/].

# Construction stages
These are the major stages of the engine development

## Basic formatting
  - [x] Downloading the *raw* version of the corpus
  - [x] Renaming the files to `db*` from 0 to 163
  - [x] Removing both *end of article* markers and replacing the *begin of article* to `#doc [title]` (No quotes)
  
 ## Data Structures generation
  - [x] Storing *offset* and *title* data for easier lookup
  - [x] Generating the trie of the entire *corpus* along with the result lists(sorted by weight)
  - [x] Storing the serialized *trie* and *result lists*
 
 ## Simple application
  - [x] Loading the *serialized* data for user searches
  - [x] Word searching
  - [x] Document listings
  - [ ] Document viewer
  
  ## Web application
    - TODO
