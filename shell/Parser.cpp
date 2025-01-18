#include "Parser.h"
#include <cstddef> // For size_t

using namespace std;

// Function to parse command tokens into a Command structure
Command Parser::parse(const  vector< string>& tokens) {
    Command cmd;
    if (!tokens.empty()) {
        cmd.name = tokens[0];
        for (size_t i = 1; i < tokens.size(); ++i) {
            cmd.arguments.push_back(tokens[i]);
        }
    }
    return cmd;
}

// Function to parse a directory path into parent path and directory name
 pair< string,  string> Parser::parsePath(const  string& dirPath) {
    string parentPath;
    string dirName;

    size_t lastSlash = dirPath.find_last_of("\\");
    if (lastSlash == string::npos) {
        // No slashes, the path is a single directory name
        parentPath = "";
        dirName = dirPath;
    }
    else {
        // Split into parent path and directory name
        parentPath = dirPath.substr(0, lastSlash);
        dirName = dirPath.substr(lastSlash + 1);
    }

    return { parentPath, dirName };
}
