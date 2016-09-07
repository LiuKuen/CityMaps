#pragma once

#include <iostream>
#include <vector>
using namespace std;

class TrieNode{
public:
    TrieNode() {mContent = ' '; mMarker = false; }
    ~TrieNode() {}
    char getContent() { return mContent; }
    void setContent(char c) { mContent = c; }
    bool getWordMarker() {return mMarker; }
    void setWordMarker() { mMarker = true; }
    int numberOfChildren() { return mChildren.size(); }
    TrieNode* getChild(int i) { return mChildren.at(i); };
    TrieNode* findChild(char c);
    void appendChild(TrieNode* child) { mChildren.push_back(child); }
private:
    char mContent;
    bool mMarker;
    std::vector<TrieNode*> mChildren;
};

class Trie {
public:
    Trie();
    ~Trie();
    void addWord(string s);
    bool searchWord(string s);
    //void deleteWord(string s);
    void freeTrie();
    void destructorHelper(TrieNode*);
    TrieNode* findPrefixLocation(string s);
    //stores all the words of a given prefix into a 2D vector
    void getWordsFromPrefix(TrieNode*, string, int, std::vector<string>&);
private:
    TrieNode* root;
};

//we want to hold all uppercase and lower case letters in our tree