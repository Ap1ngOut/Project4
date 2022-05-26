#include "NameTable.h"
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <functional>
using namespace std;

// This class does the real work of the implementation.


class NameTableImpl
{
public:
    NameTableImpl();
    ~NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
    int hashString(const string& id) const;
    void reHash();
    bool addToHash(const string& id, int lineNum, int scopeID);
private:
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

    unsigned int hashSize, items, scope;
    vector<list<Data*>*> *hashTable;
};

NameTableImpl::NameTableImpl() : hashSize(100), items(0), scope(0)  {
    hashTable = new vector<list<Data*>*>;
    for (int i = 0; i < hashSize; i++)
        hashTable->emplace_back(nullptr);
}

NameTableImpl::~NameTableImpl() {
    for (auto i : *hashTable) {
        if (i != nullptr)
            for (auto j : *i)
                delete j;
        delete i;
    }
    delete hashTable;
}

bool NameTableImpl::addToHash(const string &id, const int lineNum, const int scopeID) {
    int hashID = (int) hashString(id);
    if((*hashTable)[hashID] != nullptr) {
        //Check if identical variable has been created in this scope
        for (auto i : *(*hashTable)[hashID]) {
            if(*i == Data(id, lineNum, scopeID))
                return false;
        }
        (*hashTable)[hashID]->emplace_back(new Data(id,lineNum,scopeID));
    } else {
        (*hashTable)[hashID] = new list<Data*>;
        (*hashTable)[hashID]->emplace_back(new Data(id,lineNum,scopeID));
    }
    items++;
    if ((int) items / hashSize > 5) {
        reHash();
    }
    return true;
}

int NameTableImpl::hashString(const string &id) const {
    return (int) (std::hash<string>() (id) % hashSize);
}

void NameTableImpl::reHash() {
    if(hashSize == 20000)
        return;
    if (hashSize * 2 > 20000)
        hashSize = 20000;
    else
        hashSize *= 2;

    auto *newHashTable = new vector<list<Data*>*>;
    for (int i = 0; i < hashSize; i++) {
        newHashTable->emplace_back(nullptr);
    }

    for (auto i : *hashTable) {
        if (i != nullptr) {
            for (auto j : *i) {
                int hashID = (int) hashString(j->ID);
                if((*newHashTable)[hashID] != nullptr) {
                    (*newHashTable)[hashID]->emplace_back(j);
                } else {
                    (*newHashTable)[hashID] = new list<Data*>;
                    (*newHashTable)[hashID]->emplace_back(j);
                }
            }
            delete i;
        }
    }
    delete hashTable;
    hashTable = newHashTable;
}

void NameTableImpl::enterScope()
{
    scope++;
}

bool NameTableImpl::exitScope()
{
    if(scope <= 0)
        return false;
    //Get rid of every variable with scope_current
    for (auto i : *hashTable)
        if (i != nullptr && !i->empty()) {
            for (auto j = (*i).begin(); j != (*i).end();) {
                if ((*j)->scopeID == scope)
                {
                    delete *j;
                    j = i->erase(j);
                }
                else ++j;
            }
            if(i->empty()) {
                delete i;
                i = nullptr;
            }
        }
    scope--;
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    return addToHash(id, lineNum, (int) scope);
}

int NameTableImpl::find(const string& id) const
{
    int found = -1;
    int hashID = (int) hashString(id);
    auto i = hashTable->at(hashID);
    if (i == nullptr || i->empty())
        return found;
    for(auto j = i->rbegin(); j != i->rend(); j++) {
        if((*j)->ID == id) {
            found = (*j)->line;
            break;
        }
    }
    return found;
}

//*********** NameTable functions **************

// For the most part, these functions simply delegate to NameTableImpl's
// functions.

NameTable::NameTable()
{
    m_impl = new NameTableImpl;
}

NameTable::~NameTable()
{
    delete m_impl;
}

void NameTable::enterScope()
{
    m_impl->enterScope();
}

bool NameTable::exitScope()
{
    return m_impl->exitScope();
}

bool NameTable::declare(const string& id, int lineNum)
{
    return m_impl->declare(id, lineNum);
}

int NameTable::find(const string& id) const
{
    return m_impl->find(id);
}