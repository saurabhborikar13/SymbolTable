#include <iostream>
#include <unordered_map>
#include <stack>
#include <string>
#include <vector>
#include <iomanip>

using namespace std;

int scopeCounter = 0; // Unique counter for each scope

// Class to represent symbol information
class SymbolInfo {
public:
    string nature; // Nature of the symbol (local, para, etc.)
    string type;   // Type of the symbol (int, float, etc.)
    int value;     // Value assigned to the symbol
    int address;   // Address info (simulated offset)

    SymbolInfo(string nature = "", string type = "", int value = 0, int address = 0)
        : nature(nature), type(type), value(value), address(address) {}
};

// Class to represent a scope
class Scope {
public:
    int id; // Unique ID for each scope
    unordered_map<string, SymbolInfo> table; // Symbol table for variables in the current scope
    Scope* parent; // Pointer to the parent scope
    vector<Scope*> children; // List of child scopes

    Scope(Scope* parentScope = nullptr) {
        parent = parentScope;
        id = ++scopeCounter;
        if (parentScope != nullptr) {
            parentScope->children.push_back(this); // Link this scope as a child of its parent
        }
    }
};

// Global pointer to keep track of the current scope
Scope* currentScope = nullptr;

// Function to initialize a new scope
void beginScope() {
    currentScope = new Scope(currentScope);
}

// Function to finalize (end) the current scope
void endScope() {
    if (currentScope != nullptr) {
        currentScope = currentScope->parent;
    } else {
        cout << "No scope to end.\n";
    }
}

// Function to assign a value to a variable in the current scope
void assignVariable(const string& var, int value, const string& nature, const string& type, int address) {
    if (currentScope != nullptr) {
        currentScope->table[var] = SymbolInfo(nature, type, value, address);
    } else {
        cout << "No active scope to assign variable.\n";
    }
}

// Function to lookup the value of a variable across nested scopes
bool lookupVariable(const string& var, SymbolInfo& info) {
    Scope* temp = currentScope;
    while (temp != nullptr) {
        if (temp->table.find(var) != temp->table.end()) {
            info = temp->table[var];
            return true;
        }
        temp = temp->parent;
    }
    return false;
}

// Function to print the value of a variable
void printVariable(const string& var) {
    SymbolInfo info;
    if (lookupVariable(var, info)) {
        cout << "Value: " << info.value << ", Type: " << info.type << ", Nature: " << info.nature << ", Address: " << info.address << endl;
    } else {
        cout << "Variable " << var << " not found.\n";
    }
}

// Function to display the hierarchy and variables in a tabular format
void displayHierarchy(Scope* scope, int level = 0) {
    if (scope == nullptr) return;

    // Print table header
    cout << "+---------+------------+--------------+---------+---------+---------+-------------+\n";
    cout << "| ScopeID | ParentID   | Variable Name| Nature  | Type    | Address | Variable Val|\n";
    cout << "+---------+------------+--------------+---------+---------+---------+-------------+\n";

    // Print the current scope information
    for (const auto& var : scope->table) {
        cout << "| " << setw(7) << scope->id
             << " | " << setw(10) << (scope->parent ? to_string(scope->parent->id) : "None")
             << " | " << setw(12) << var.first
             << " | " << setw(7) << var.second.nature
             << " | " << setw(7) << var.second.type
             << " | " << setw(7) << var.second.address
             << " | " << setw(11) << var.second.value << " |\n";
    }

    if (scope->table.empty()) {
        cout << "| " << setw(7) << scope->id
             << " | " << setw(10) << (scope->parent ? to_string(scope->parent->id) : "None")
             << " | " << setw(12) << "---"
             << " | " << setw(7) << "---"
             << " | " << setw(7) << "---"
             << " | " << setw(7) << "---"
             << " | " << setw(11) << "---" << " |\n";
    }

    cout << "+---------+------------+--------------+---------+---------+---------+-------------+\n";

    for (Scope* child : scope->children) {
        displayHierarchy(child, level + 1);
    }
}

int main() {
    beginScope(); // Global scope
    assignVariable("a", 1, "local", "int", 1001);
    assignVariable("b", 2, "local", "int", 1002);

    beginScope(); // Nested scope 1
    assignVariable("a", 3, "para", "int", 2001);
    assignVariable("c", 4, "local", "int", 2002);

    printVariable("b"); // Expected output: 2

    beginScope(); // Nested scope 2
    assignVariable("c", 5, "local", "int", 3001);

    printVariable("b"); // Expected output: 2
    printVariable("a"); // Expected output: 3
    printVariable("c"); // Expected output: 5

    endScope(); // End Nested scope 2

    printVariable("a"); // Expected output: 3
    printVariable("c"); // Expected output: 4

    endScope(); // End Nested scope 1

    printVariable("a"); // Expected output: 1

    // Display the hierarchy of scopes and variables
    cout << "\nScope Hierarchy and Variables:\n";
    displayHierarchy(currentScope);

    endScope(); // End global scope

    return 0;
}