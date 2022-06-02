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
private:
    struct Data {
        Data(const string& id, const int lineNum, const int scopeid) : line(lineNum), scopeID(scopeid), ID(id) {

        }
        int line, scopeID;
        string ID;
    };

    int hashSize, scope;
    vector<list<Data>> hashTable;
    stack<string> scopeNames;
public:
    NameTableImpl();
    ~NameTableImpl();
    void enterScope();
    bool exitScope();
    bool declare(const string& id, int lineNum);
    int find(const string& id) const;
    int hashString(const string& id) const;
    bool addToHash(const string& id, int numLine, int scopeid);
};

NameTableImpl::NameTableImpl() : hashSize(19997), scope(0)  {
    for (int i = 0; i < hashSize; i++)
        hashTable.emplace_back(list<Data>());
}

NameTableImpl::~NameTableImpl() {

}

bool NameTableImpl::addToHash(const string& id, int line, int scopeid) {
    int hashID = (int) hashString(id);
    if(!hashTable[hashID].empty()) {
        //Check if identical variable has been created in this scope
        for (const auto& i : hashTable[hashID]) {
            if(i.ID == id && i.scopeID == scopeid)
                return false;
            if(i.scopeID < scope)
                break;
        }
        hashTable[hashID].emplace_front(Data(id, line, scopeid));
    } else {
        hashTable[hashID].emplace_front(Data(id, line, scopeid));
    }
    return true;
}

int NameTableImpl::hashString(const string &id) const {
    return (int) (std::hash<string>() (id) % hashSize);
}

void NameTableImpl::enterScope()
{
    scope++;
    scopeNames.push("");
}

bool NameTableImpl::exitScope()
{
    if(scope <= 0)
        return false;
    //Get rid of every variable with scope_current using the scopeNames stack
    int hashID;
    string current = scopeNames.top();
    scopeNames.pop();
    while(!scopeNames.empty() && !current.empty()) {
        hashID = hashString(current);
        for(auto j = hashTable[hashID].begin(); j != hashTable[hashID].end(); ++j) {
            if(j->ID == current && j->scopeID == scope) {
                hashTable[hashID].erase(j);
                break;
            }
        }
        current = scopeNames.top();
        scopeNames.pop();
    }
    scope--;
    return true;
}

bool NameTableImpl::declare(const string& id, int lineNum)
{
    if (id.empty())
        return false;
    scopeNames.push(id);
    return addToHash(id, lineNum, scope);
}

int NameTableImpl::find(const string& id) const
{
    int found = -1;
    int hashID = (int) hashString(id);
    auto i = hashTable[hashID];
    if (i.empty())
        return found;
    for(const auto& j : hashTable[hashID]) {
        if (j.ID == id) {
            return j.line;
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