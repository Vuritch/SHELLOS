#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "File_Entry.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Parser.h"    
#include "Tokenizer.h" 

using namespace std;

// Forward declaration for Directory class
class Directory;

class CommandProcessor
{
public:
    // Constructor accepts a pointer to the pointer of the current directory
    CommandProcessor(Directory** currentDirPtr);
    // Process the input command
    void processCommand(const string& input, bool& isRunning);

    // **String Utility Functions**
    
     // Convert a string to lowercase
    string toLower(const string& s);
    // Convert a string to uppercase
    string toUpper(const string& s);
    // Trim leading and trailing whitespace from a string
    string trimString(const string& input);

private:
    void showGeneralHelp();
    void showCommandHelp(const string& command);
    void handleCls();
    void handleMd(const string& dirname);
    void handleRd(const vector<string>& directories);
    void handleCd(const string& dirname);
    void handlePwd();
    void handleQuit(bool& isRunning);
    void handleHistory();
    void handleDir(const string& path);
    void handleEcho(const string& filePath);
    void handleWrite(const string& filePath);
    bool isValidFileName(const string& name);
    void handleType(const vector<string>& filePaths);
    void handleDel(const vector<string>& targets);
    void handleRename(const vector<string>& args);
    void handleCopy(const vector<string>& args);
    void handleImport(const  vector< string>& args);
    void handleExport(const vector<string>& args);

    // **Directory and File Navigation**
    
       // Navigate to a directory specified by a path
    Directory* MoveToDir(const string& path);
    // Navigate to a file specified by a path and return a File_Entry object
    File_Entry* MoveToFile(string& path);


    vector<string> commandHistory;
    unordered_map<string, pair<string, string>> commandHelp;
    Directory** currentDirectoryPtr;
    Directory* currentDir;
    



};

#endif // COMMANDPROCESSOR_H
