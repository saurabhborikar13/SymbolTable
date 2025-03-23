#include <iostream>
#include <vector>
#include <list>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

int scopeCounter = -1; // Unique counter for each scope

// Hash node for separate chaining
class HashNode {
public:
    string key;
    string nature;
    int value;

    HashNode(string key, string nature, int value) : key(key), nature(nature), value(value) {}
};

// Custom unordered_map using separate chaining
class MyHashMap {
private:
    static const int SIZE = 100; // Size of hash table
    vector<list<HashNode>> table;

    int hashFunction(const string& key) {
        int hash = 0;
        for (char ch : key) {
            hash = (hash * 31 + ch) % SIZE;
        }
        return hash;
    }

public:
    MyHashMap() : table(SIZE) {}

    // Insert or update variable
    void insert(const string& key, const string& nature, int value) {
        int index = hashFunction(key);
        for (auto& node : table[index]) {
            if (node.key == key) {
                node.value = value; // Update existing value
                return;
            }
        }
        table[index].push_back(HashNode(key, nature, value));
    }

    // Find a variable
    bool find(const string& key, string& nature, int& value) {
        int index = hashFunction(key);
        for (auto& node : table[index]) {
            if (node.key == key) {
                nature = node.nature;
                value = node.value;
                return true;
            }
        }
        return false;
    }

    // Get all entries in the hash map
    vector<HashNode> getAllEntries() {
        vector<HashNode> entries;
        for (int i = 0; i < SIZE; i++) {
            for (auto& node : table[i]) {
                entries.push_back(node);
            }
        }
        return entries;
    }
};

class Scope {
public:
    int id;
    void* address;
    MyHashMap table;
    Scope* parent;
    vector<Scope*> children;

    Scope(Scope* parentScope = nullptr) {
        parent = parentScope;
        id = ++scopeCounter;
        address = static_cast<void*>(this);
        if (parentScope != nullptr) {
            parentScope->children.push_back(this);
        }
    }

    // Assign variable
    void assignVariable(const string& var, int value, const string& nature = "local") {
        table.insert(var, nature, value);
    }

    // Lookup variable in current scope
    bool lookupVariable(const string& var, string& nature, int& value) {
        return table.find(var, nature, value);
    }
};

Scope* currentScope = nullptr;

void beginScope() {
    currentScope = new Scope(currentScope);
}

void endScope() {
    if (currentScope != nullptr) {
        currentScope = currentScope->parent;
    } else {
        cout << "No scope to end.\n";
    }
}

void assignVariable(const string& var, int value, const string& nature = "local") {
    if (currentScope != nullptr) {
        currentScope->assignVariable(var, value, nature);
    } else {
        cout << "No active scope to assign variable.\n";
    }
}

bool lookupVariable(const string& var, string& nature, int& value) {
    Scope* temp = currentScope;
    while (temp != nullptr) {
        if (temp->lookupVariable(var, nature, value)) {
            return true;
        }
        temp = temp->parent;
    }
    return false;
}

void printVariable(const string& var) {
    Scope* temp = currentScope;
    while (temp != nullptr) {
        string nature;
        int value;
        if (temp->lookupVariable(var, nature, value)) {
            cout << "value : " << value << " | " << " Scope ID : " << temp->id << " | " << " Nature: " << nature << " |\n";
            return;
        }
        temp = temp->parent;
    }
    cout << "Variable " << var << " not found in any active scope.\n";
}

void displayHierarchy(Scope* scope, int level = 0) {
    if (scope == nullptr) return;

    if (level == 0) {
        cout << "+---------+------------------+------------+------------------+--------------+-------------+---------+\n";
        cout << "| ScopeID | Address          | ParentID   | Parent Address   | Variable Name| Variable Val| Nature  |\n";
        cout << "+---------+------------------+------------+------------------+--------------+-------------+---------+\n";
    }

    // Get all entries in the current scope's hash map
    vector<HashNode> entries = scope->table.getAllEntries();

    // Print scope details
    for (const auto& entry : entries) {
        cout << "| " << setw(7) << scope->id
             << " | " << setw(16) << scope->address
             << " | " << setw(10) << (scope->parent ? to_string(scope->parent->id) : "None")
             << " | " << setw(16) << (scope->parent ? scope->parent->address : nullptr)
             << " | " << setw(12) << entry.key
             << " | " << setw(11) << entry.value
             << " | " << setw(7) << entry.nature << " |\n";
    }

    if (entries.empty()) {
        cout << "| " << setw(7) << scope->id
             << " | " << setw(16) << scope->address
             << " | " << setw(10) << (scope->parent ? to_string(scope->parent->id) : "None")
             << " | " << setw(16) << (scope->parent ? scope->parent->address : nullptr)
             << " | " << setw(12) << "---"
             << " | " << setw(11) << "---"
             << " | " << setw(7) << "---" << " |\n";
    }

    cout << "+---------+------------------+------------+------------------+--------------+-------------+---------+\n";

    // Recursively display child scopes
    for (Scope* child : scope->children) {
        displayHierarchy(child, level + 1);
    }
}

void processCommandsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string command;
        ss >> command;

        if (command == "begin") {
            beginScope();
        } else if (command == "assign") {
            string var;
            int value;
            ss >> var >> value;
            assignVariable(var, value);
        } else if (command == "print") {
            string var;
            ss >> var;
            printVariable(var);
        } else if (command == "end") {
            endScope();
        } else {
            cout << "Unknown command: " << command << endl;
        }
    }

    file.close();
}

int main() {
    beginScope();
    Scope* rootScope = currentScope;

    processCommandsFromFile("input.txt");

    cout << "\nScope Hierarchy and Variables:\n";
    displayHierarchy(rootScope);

    return 0;
}