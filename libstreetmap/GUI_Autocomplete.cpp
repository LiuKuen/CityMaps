#include "GUI_Autocomplete.h"

void AutoCompleteHandler::updateQueries(Trie& database, int keysym, string prefix){
    if(keysym != XK_Tab){
        autocompleteQueries.clear();
        int startingRow = 0;
        
        // prefix remains the same as the user types
        TrieNode* prefixNode = database.findPrefixLocation(prefix);
        database.getWordsFromPrefix(prefixNode, prefix, startingRow, autocompleteQueries);
        
        // Change the first letter to uppercase/lowercase
        if(!prefix.empty()){
            if(islower(prefix.at(0))){
                prefix[0]+='A'-'a';
            }
            else
                prefix[0]+='a'-'A';
            std::vector<string> otherQuery;
            //cout << prefix << endl;
            prefixNode = database.findPrefixLocation(prefix);
            //cout << prefixNode->getContent() << endl;
            database.getWordsFromPrefix(prefixNode, prefix, startingRow, otherQuery);
            autocompleteQueries.insert(autocompleteQueries.end(),otherQuery.begin(),otherQuery.end());
        }
    }
    return;
}

void AutoCompleteHandler::autoCompleteString(int keysym, string& input,
        int currentHighlighting){
    if(currentHighlighting>0)
        currentHighlighting--;
    if (keysym == XK_Tab){
        if(autocompleteQueries.size() > 0){
            input = autocompleteQueries[currentHighlighting];
        }
    }
}
