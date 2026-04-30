#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <sstream>
#include <stdexcept>
#include <cassert>
using namespace std;

class FileNotFoundException : public runtime_error {
public:
    explicit FileNotFoundException(const string& message)
        : runtime_error("FileNotFoundException: " + message) {}
};

class InvalidCsvFormatException : public runtime_error {
public:
    explicit InvalidCsvFormatException(const string& message)
        : runtime_error("InvalidCsvFormatException: " + message) {}
};

// --- Data Model ---
struct Book {
    string title;
    string author;
    int year;
    string isbn;
};

// --- Template Utility ---
template <typename Container>
void splitCsvLine(const string& line, Container& cont, char delim = ',') {
    stringstream ss(line);
    string token;
    while (getline(ss, token, delim)) {
        cont.push_back(token);
    }
}

// --- Core Logic ---
set<string> extractUniqueAuthors(const string& csvFilePath) {
    ifstream file(csvFilePath);
    if (!file.is_open()) {
        throw FileNotFoundException("Could not open books CSV file: " + csvFilePath);
    }

    set<string> uniqueAuthors;
    string line;
    int lineNumber = 1;

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> columns;
        splitCsvLine(line, columns);

        if (columns.size() != 4) {
            throw InvalidCsvFormatException("Missing columns at line " + to_string(lineNumber));
        }

        uniqueAuthors.insert(columns[1]); // Author is the 2nd column
        lineNumber++;
    }
    return uniqueAuthors;
}

void saveAuthorsToJson(const set<string>& authors, const string& jsonFilePath) {
    ofstream file(jsonFilePath);
    if (!file.is_open()) throw FileNotFoundException("Could not create JSON file: " + jsonFilePath);

    file << "[\n";
    for (set<string>::const_iterator it = authors.begin(); it != authors.end(); ++it) {
        file << "  \"" << *it << "\"";
        if (next(it) != authors.end()) {
            file << ",";
        }
        file << "\n";
    }
    file << "]\n";
}

// --- Unit Tests ---
void runTask1Tests() {
    cout << "Running Task 1 Unit Tests...\n";
    try {
        extractUniqueAuthors("non_existent_books.csv");
        assert(false && "Should have thrown FileNotFoundException");
    }
    catch (const FileNotFoundException& e) {
        cout << "[PASS] Task 1: FileNotFoundException handled properly.\n";
    }
}

// --- Main ---
int main() {
    runTask1Tests();

    // Setup mock data
    ofstream mockBooks("books.csv");
    mockBooks << "C++ Primer,Lippman,2012,111-111\n";
    mockBooks << "Effective C++,Meyers,2005,222-222\n";
    mockBooks << "More Effective C++,Meyers,1996,333-333\n";
    mockBooks.close();

    try {
        cout << "Executing Task 1...\n";
        set<string> authors = extractUniqueAuthors("books.csv");
        saveAuthorsToJson(authors, "authors.json");
        cout << "Success! Saved to authors.json\n";
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << "\n";
    }

    remove("books.csv"); // Cleanup
    return 0;
}
