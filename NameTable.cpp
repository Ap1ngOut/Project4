#include "NameTable.h"
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include<stack>
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
    };

    unsigned int hashSize, items, scope;
    vector<list<Data>*> *hashTable;
    stack<string> *scopeNames;
};

NameTableImpl::NameTableImpl() : hashSize(100), items(0), scope(0)  {
    hashTable = new vector<list<Data>*>;
    scopeNames = new stack<string>;
    for (int i = 0; i < hashSize; i++)
        hashTable->emplace_back(nullptr);
}

NameTableImpl::~NameTableImpl() {
    for (auto i : *hashTable)
        delete i;
    hashTable->clear();
    delete scopeNames;
    delete hashTable;
}

bool NameTableImpl::addToHash(const string &id, const int lineNum, const int scopeID) {
    int hashID = (int) hashString(id);
    if((*hashTable)[hashID] != nullptr) {
        //Check if identical variable has been created in this scope
        for (const auto& i : *((*hashTable)[hashID])) {
            if(i.ID == id && i.scopeID == scopeID)
                return false;
        }
        (*hashTable)[hashID]->emplace_front(Data(id,lineNum,scopeID));
    } else {
        (*hashTable)[hashID] = new list<Data>;
        (*hashTable)[hashID]->emplace_front(Data(id,lineNum,scopeID));
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

    auto newHashTable = new vector<list<Data>*>;
    for (int i = 0; i < hashSize; i++) {
        newHashTable->emplace_back(nullptr);
    }

    for (auto i : *hashTable) {
        if (i != nullptr) {
            for (const auto& j : *i) {
                int hashID = (int) hashString(j.ID);
                if((*newHashTable)[hashID] != nullptr) {
                    (*newHashTable)[hashID]->emplace_back(j);
                } else {
                    (*newHashTable)[hashID] = new list<Data>;
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
    scopeNames->push("");
}

bool NameTableImpl::exitScope()
{
    if(scope <= 0)
        return false;
    //Get rid of every variable with scope_current using the scopeNames stack
    int hashID;
    string current = scopeNames->top();
    scopeNames->pop();
    while(!scopeNames->empty() && !current.empty()) {
        hashID = hashString(current);
        for(auto j = (*hashTable)[hashID]->begin(); j != (*hashTable)[hashID]->end(); ++j) {
            if(j->ID == current && j->scopeID == scope) {
                (*hashTable)[hashID]->erase(j);
                items--;
                break;
            }
        }
        current = scopeNames->top();
        scopeNames->pop();
    }
    scope--;
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    if (id.empty())
        return false;
    scopeNames->push(id);
    return addToHash(id, lineNum, (int) scope);
}

int NameTableImpl::find(const string& id) const
{
    int found = -1;
    int hashID = (int) hashString(id);
    auto i = (*hashTable)[hashID];
    if (i == nullptr || (*i).empty())
        return found;
    for(const auto& j : (*(*hashTable)[hashID])) {
        if (j.ID == id && j.scopeID <= scope) {
            found = j.line;
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