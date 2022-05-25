#include "NameTable.h"
#include <string>
#include <vector>
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
    unsigned int hashString(const string& id);
    void reHash();
private:
    struct Data {
        unsigned int line, scopeID;
        string ID;
    };
    unsigned int hashSize, items, scope;
    vector<vector<Data*>*> *hashTable;
};

NameTableImpl::NameTableImpl() : hashSize(100), items(0), scope(0)  {
    for (int i = 0; i < hashSize; i++)
        (*hashTable)[i] = nullptr;
}

NameTableImpl::~NameTableImpl() {
    for (auto i : *hashTable) {
        for (auto j : *i)
            delete j;
        delete i;
    }
    delete hashTable;
}

unsigned int NameTableImpl::hashString(const string &id) {
    return std::hash<string>() (id) % hashSize;
}

void NameTableImpl::reHash() {
    if(hashSize == 20000)
        return;
    if (hashSize * 2 > 20000)
        hashSize = 20000;
    else
        hashSize *= 2;

    auto *newHashTable = new vector<vector<Data*>*>;
    for (int i = 0; i < hashSize; i++) {
        (*newHashTable)[i] = nullptr;
    }

    for (auto i : *hashTable) {
        if (i != nullptr) {
            for (auto j : *i) {

            }
        }
    }
}

void NameTableImpl::enterScope()
{

}

bool NameTableImpl::exitScope()
{

}

bool NameTableImpl::declare(const string& id, int lineNum)
{

}

int NameTableImpl::find(const string& id) const
{

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