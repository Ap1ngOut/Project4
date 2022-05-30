#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <cassert>
using namespace std;

struct Data {
    Data(const string& id, const int lineNum, const int scopeid) : line(lineNum), scopeID(scopeid), ID(id) {

    }
    int line, scopeID;
    string ID;
    bool operator==(const Data& rhs) const
    {
        return ID == rhs.ID && scopeID == rhs.scopeID;
    }
    bool operator!=(const Data& rhs) const
    {
        return !(ID == rhs.ID && scopeID == rhs.scopeID);
    }
};

bool addToHash(const string &id, const int lineNum, const int scopeID, vector<list<Data*>*>*& hashTable) {
    int hashID = 50;
    if((*hashTable)[hashID] != nullptr) {
        //Check if identical variable has been created in this scope
        for (auto i : *((*hashTable)[hashID])) {
            if(*i == Data(id, lineNum, scopeID))
                return false;
        }
        (*hashTable)[hashID]->emplace_back(new Data(id,lineNum,scopeID));
    } else {
        (*hashTable)[hashID] = new list<Data*>;
        (*hashTable)[hashID]->emplace_back(new Data(id,lineNum,scopeID));
    }
    return true;
}

int main () {
    auto hashTable = new vector<list<Data*>*>;
    for (int i = 0; i < 100; i++)
        hashTable->emplace_back(nullptr);

    if ((*hashTable)[50] == nullptr)
        cout << "111" << endl;

    addToHash("asdf", 10, 10, hashTable);
    addToHash("asdfasdf", 10, 10, hashTable);
    addToHash("asdfasdfasdf", 10, 10, hashTable);
    assert(addToHash("asdf", 10, 10, hashTable) == false);
    auto j = *(*(*hashTable)[50]).rbegin();
    if(j->ID == "asdfasdfasdf")
        cout << "TRUE" << endl;

    if ((*hashTable)[50] == nullptr)
        cout << "111" << endl;

    for (auto i : *hashTable)
        if (i != nullptr)
            for (auto j : *i)
                cout << (*j).ID << endl;



    for (auto i : *hashTable) {
        if (i != nullptr)
            for (auto j : *i)
                delete j;
        delete i;
    }
    delete hashTable;

}