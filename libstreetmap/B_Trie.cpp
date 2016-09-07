#include <B_Trie.h>

TrieNode* TrieNode::findChild(char c){
    for(unsigned i = 0; i < mChildren.size(); i++){
        TrieNode* tmp = mChildren.at(i);
        if(tmp->getContent() == c){
            return tmp;
        }
    }
    return NULL;
}

Trie::Trie(){
    root = new TrieNode();
}

Trie::~Trie(){
    //Free memory
    //freeTrie();
}

void Trie::addWord(string s){
    if(root == NULL){
        root = new TrieNode();
    }
    TrieNode* current = root;
    
    if(s.length() == 0){
        //an empty word
        current->setWordMarker();
        return;
    }
    
    for(unsigned i = 0; i < s.length(); i++){
        TrieNode* child = current->findChild(s[i]);
        if(child != NULL){
            current = child;
        }
        else{
            TrieNode* tmp = new TrieNode();
            tmp->setContent(s[i]);
            current->appendChild(tmp);
            current = tmp;
        }
        if(i == s.length() - 1)
            current->setWordMarker();
    }
}

void Trie::freeTrie(){
    destructorHelper(root);
    root = NULL;
    return;
}

void Trie::destructorHelper(TrieNode* current){
  
    if(current != NULL){
        for(int i = 0; i < current->numberOfChildren(); i++){
            destructorHelper(current->getChild(i));
        }
        TrieNode* temp = current;
        delete temp;
        current = NULL;
        return;
    }
    return;
}

bool Trie::searchWord(string s){
    TrieNode* current = root;
    
    while(current != NULL){
        for(unsigned i = 0; i < s.length(); i++){
            TrieNode* tmp = current->findChild(s[i]);
            if(tmp == NULL)
                return false;
            current = tmp;
        }
        if(current->getWordMarker())
            return true;
        else
            return false;
    }
    return false;
}

TrieNode* Trie::findPrefixLocation(string s){
    TrieNode* current = root;
    TrieNode* tmp = NULL;
    
    while(current != NULL){
        for(unsigned i = 0; i < s.length(); i++){
            tmp = current->findChild(s[i]);
            if(tmp == NULL)
                return NULL;
            current = tmp;
        }
        return tmp;
    }
    return NULL;
}

void Trie::getWordsFromPrefix(TrieNode* current, string baseString, int row, std::vector<string>& wordsFromPrefix){
    if(current != NULL){
        if(current->getWordMarker()){
            wordsFromPrefix.push_back(baseString);
            row++;
        }
        for(int i = 0; i < current->numberOfChildren(); i++){
            baseString += current->getChild(i)->getContent();
            getWordsFromPrefix(current->getChild(i), baseString, row, wordsFromPrefix);
            baseString.pop_back();
        }
        return;
    }
    return;
}