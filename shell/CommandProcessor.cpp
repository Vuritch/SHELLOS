#include "CommandProcessor.h"
#include "Directory.h"
#include"Mini_FAT.h"
#include <algorithm>
#include <cstring>
#include <cctype>
#include <sstream> 
#include "File_Entry.h"
#include<iostream>
#include <iomanip>
#include <ios>
#include "Parser.h"
#include <fstream>    
#include <filesystem>
namespace fs =  filesystem;
using namespace std;


// Constructor for the CommandProcessor class
// Initializes the command help map and sets the current directory pointer.
CommandProcessor::CommandProcessor(Directory** currentDirPtr)
    : currentDirectoryPtr(currentDirPtr) // Initialize member variable with provided pointer
{
    // Add help command details to the commandHelp map
    commandHelp["help"] = {
        "Provides help information for commands.", // Brief description of the command
        "Usage:\n"
        "  help\n"
        "  help [command]\n\n"
        "Syntax:\n"
        "  - General Help: `help`\n"
        "  - Command-Specific Help: `help [command_name]`\n\n"
        "Description:\n"
        "  - Displays a list of all available commands with brief descriptions.\n"
        "  - For a specific command, provides detailed information, including its usage and syntax."
    };



    // Add the "rd" (remove directory) command details to the commandHelp map
    commandHelp["rd"] = {
        // Brief description of the command
        "Removes one or more directories.",

        // Detailed usage instructions
        "Usage:\n"
        "  rd [directory_name]+\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Remove a single directory: `rd [directory_name]`\n"
        "  - Remove multiple directories: `rd [directory1] [directory2] ...`\n\n"

        // Description of the command
        "Description:\n"
        "  - Deletes the specified directory or directories.\n"
        "  - Each directory must be empty before it can be deleted."
    };



    // Add the "md" (make directory) command details to the commandHelp map
    commandHelp["md"] = {
        // Brief description of the command
        "Creates a new directory.",

        // Usage examples for the command
        "Usage:\n"
        "  md [path]\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Create a directory: `md [directory_name]`\n"
        "  - Create a directory with a specific path: `md [path/to/directory]`\n\n"

        // Detailed description
        "Description:\n"
        "  - Creates a new directory in the specified path or current directory."
    };



    // Add the "echo" command details to the commandHelp map
    // Provides information about creating new empty files
    commandHelp["echo"] = {
        // Brief description of the command
        "Creates a new empty file.",

        // Usage instructions
        "Usage:\n"
        "  echo [path]\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Create a file: `echo [file_name]`\n"
        "  - Create a file in a specific path: `echo [path/to/file]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Creates a new empty file at the specified path or in the current directory."
    };



    // Add the "write" command details to the commandHelp map
    // Provides information about writing content to an existing file
    commandHelp["write"] = {
        // Brief description of the command
        "Writes content to an existing file.",

        // Usage instructions
        "Usage:\n"
        "  write [file_path]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Write to a file: `write [file_name]`\n"
        "  - Write to a file in a specific path: `write [path/to/file]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Opens the specified file for writing.\n"
        "  - Allows input of multiple lines of text until a specific termination input is given."
    };



    // Add the "dir" command details to the commandHelp map
    // Provides information about listing the contents of a directory
    commandHelp["dir"] = {
        // Brief description of the command
        "Lists the contents of a directory.",

        // Usage instructions
        "Usage:\n"
        "  dir\n"
        "  dir [directory_path]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - List current directory: `dir`\n"
        "  - List contents of a specific directory: `dir [directory_path]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Displays files and subdirectories in the specified directory.\n"
        "  - Includes detailed statistics such as:\n"
        "    - File count\n"
        "    - Directory count\n"
        "    - Total used space\n"
        "    - Free space"
    };



    // Add the "del" command details to the commandHelp map
    // Provides information about deleting one or more files
    commandHelp["del"] = {
        // Brief description of the command
        "Deletes one or more files.",

        // Usage instructions
        "Usage:\n"
        "  del [file|directory]+\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Delete a single file: `del [file_name]`\n"
        "  - Delete multiple files: `del [file1] [file2] ...`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Deletes the specified file(s).\n"
        "  - Does not delete subdirectories or their contents.\n"
        "  - Wildcards (e.g., `*` or `?`) can be used to match multiple files (if supported)."
    };




    // Add the "cd" command details to the commandHelp map
    // Provides information about changing the current working directory
    commandHelp["cd"] = {
        // Brief description of the command
        "Changes the current directory.",

        // Usage instructions
        "Usage:\n"
        "  cd\n"
        "  cd [directory]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Show current directory: `cd`\n"
        "  - Change to a specific directory: `cd [directory_name]`\n"
        "  - Move up one level: `cd ..`\n"
        "  - Move up multiple levels: `cd ../../..`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Changes the current working directory to the specified one.\n"
        "  - Accepts relative or absolute paths.\n"
        "  - Using `cd` without arguments displays the current directory."
    };



    // Add the "pwd" command details to the commandHelp map
    // Provides information about displaying the current working directory path
    commandHelp["pwd"] = {
        // Brief description of the command
        "Displays the full path of the current directory.",

        // Usage instructions
        "Usage:\n"
        "  pwd\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Display the current directory: `pwd`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Prints the absolute path of the current working directory.\n"
        "  - Useful for confirming your location in the directory structure."
    };



    // Add the "type" command details to the commandHelp map
    // Provides information about displaying the content of a file
    commandHelp["type"] = {
        // Brief description of the command
        "Displays the content of a file.",

        // Usage instructions
        "Usage:\n"
        "  type [file_path]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Display file content: `type [file_name]`\n"
        "  - Display content of a file in a specific path: `type [path/to/file]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Reads and displays the content of the specified file.\n"
        "  - Displays an error if the file is not found or is a directory.\n"
        "  - Supports text-based files; non-readable formats may display as gibberish."
    };



    // Add the "history" command details to the commandHelp map
    // Provides information about displaying the history of executed commands
    commandHelp["history"] = {
        // Brief description of the command
        "Displays the history of executed commands.",

        // Usage instructions
        "Usage:\n"
        "  history\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Show command history: `history`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Lists all the commands entered in the current session.\n"
        "  - Useful for reviewing past actions or re-executing commands.\n"
        "  - Command entries are indexed, allowing easy selection if re-execution functionality is supported."
    };



    // Add the "import" command details to the commandHelp map
    // Provides information about importing files into the virtual disk
    commandHelp["import"] = {
        // Brief description of the command
        "Imports text file(s) from your computer into the virtual disk.",

        // Usage instructions
        "Usage:\n"
        "  import [source]\n"
        "  import [source] [destination]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Import a file: `import [file_path]`\n"
        "  - Import a file to a specific location: `import [file_path] [destination]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Transfers files from your physical disk to the virtual disk.\n"
        "  - If no destination is specified, the file is imported to the current directory on the virtual disk.\n"
        "  - Overwrites existing files in the destination if a file with the same name exists."
    };



    // Add the "export" command details to the commandHelp map
    // Provides information about exporting files from the virtual disk to the physical disk
    commandHelp["export"] = {
        // Brief description of the command
        "Exports text file(s) from the virtual disk to your computer.",

        // Usage instructions
        "Usage:\n"
        "  export [source]\n"
        "  export [source] [destination]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Export a file: `export [file_path]`\n"
        "  - Export a file to a specific location: `export [file_path] [destination]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Transfers files from the virtual disk to your physical disk.\n"
        "  - [source] can be a file name or the full path of a file on the virtual disk.\n"
        "  - [destination] specifies the location on your physical disk where the file will be exported.\n"
        "  - If no destination is provided, the file is exported to the current working directory on the physical disk.\n"
        "  - The command overwrites the file in the destination if it already exists.\n"
        "  - Displays an error if the source file does not exist or cannot be accessed."
    };



    // Add the "rename" command details to the commandHelp map
    // Provides information about renaming files
    commandHelp["rename"] = {
        // Brief description of the command
        "Renames a file.",

        // Usage instructions
        "Usage:\n"
        "  rename [fileName] [new fileName]\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  - Rename a file: `rename [current_name] [new_name]`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Renames a file in the current directory or at a specified path.\n"
        "  - The new file name must not already exist.\n"
        "  - Displays an error if the source file does not exist or if the new name conflicts with an existing file."
    };



    // Add the "quit" command details to the commandHelp map
    // Provides information about exiting the application
    commandHelp["quit"] = {
        // Brief description of the command
        "Exits the application.",

        // Usage instructions
        "Usage:\n"
        "  quit\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Exit the application: `quit`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Terminates the current session and closes the application gracefully.\n"
        "  - Ensures all resources are freed and any pending changes are saved before exiting."
    };



    // Add the "copy" command details to the commandHelp map
    // Provides information about copying files or directories
    commandHelp["copy"] = {
        // Brief description of the command
        "Copies one or more files or directories to another location.",

        // Usage instructions
        "Usage:\n"
        "  copy [source]\n"
        "  copy [source] [destination]\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Copies files or directories from the source to the destination.\n"
        "  - [source] can be a file name, full path to a file, directory name, or full path to a directory.\n"
        "  - [destination] can be a file name, full path to a file, directory name, or full path to a directory.\n"
        "  - If the destination is not provided, the file or directory is copied to the current directory.\n"
        "  - Prompts for confirmation before overwriting if the destination already exists.\n"
        "  - Supports recursive copying of directory contents when the source is a directory.\n\n"

        // Syntax explanation with examples
        "Syntax:\n"
        "  copy [source]\n"
        "  copy [source] [destination]\n"
    };



    // Add the "cls" command details to the commandHelp map
    // Provides information about clearing the screen
    commandHelp["cls"] = {
        // Brief description of the command
        "Clears the screen.",

        // Usage instructions
        "Usage:\n"
        "  cls\n\n"

        // Syntax explanation
        "Syntax:\n"
        "  - Clear the screen: `cls`\n\n"

        // Detailed description of the command
        "Description:\n"
        "  - Removes all previous outputs and displays a clean prompt.\n"
        "  - This command does not delete or modify data; it only refreshes the display."
    };
}

// Function to process user commands and execute the corresponding actions
void CommandProcessor::processCommand(const string& input, bool& isRunning)
{
    // Step 1: Trim leading and trailing spaces from the input command
    string trimmedInput = trimString(input);

    // Step 2: Add the trimmed command to the history if it's not empty
    if (!trimmedInput.empty())
    {
        commandHistory.push_back(trimmedInput);
    }

    // Step 3: Tokenize the trimmed input into individual components
    vector<string> tokens = Tokenizer::tokenize(trimmedInput);

    // If no tokens are generated (empty command), exit the function
    if (tokens.empty())
    {
        return; // No action required
    }

    // Step 4: Parse the tokens into a Command object (command name and arguments)
    Command cmd = Parser::parse(tokens);

    // Convert the command name to lowercase for case-insensitive comparison
    cmd.name = toLower(cmd.name);

    // Step 5: Handle specific commands based on their names
    if (cmd.name == "help")
    {
        // Provide general help or specific command help
        if (cmd.arguments.empty())
        {
            showGeneralHelp();
        }
        else if (cmd.arguments.size() == 1)
        {
            showCommandHelp(cmd.arguments[0]);
        }
        else
        {
            // Invalid usage of the help command
            cout << "Error: Invalid syntax for help command.\n";
            cout << "Usage:\n  help\n  help [command]\n";
        }
    }
    else if (cmd.name == "del")
    {
        // Delete files or directories
        if (cmd.arguments.empty())
        {
            cout << "Error: Invalid syntax for del command.\n";
            cout << "Usage: del [file|directory]+ (e.g., del file1.txt dir1 file2.txt)\n";
        }
        else
        {
            handleDel(cmd.arguments);
        }
    }
    else if (cmd.name == "copy")
    {
        // Copy files or directories
        if (!cmd.arguments.empty() && cmd.arguments.size() <= 2)
        {
            handleCopy(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for copy command.\n";
            cout << "Usage:\n  copy [source]\n  copy [source] [destination]\n";
        }
    }
    else if (cmd.name == "rename")
    {
        // Rename a file
        if (cmd.arguments.size() == 2)
        {
            handleRename(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for rename command.\n";
            cout << "Usage: rename [fileName] [new fileName]\n";
        }
    }
    else if (cmd.name == "type")
    {
        // Display the contents of files
        if (!cmd.arguments.empty())
        {
            handleType(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for type command.\n";
            cout << "Usage: type [file_path]+ (one or more file paths)\n";
        }
    }
    else if (cmd.name == "write")
    {
        // Write content to a file
        if (cmd.arguments.size() == 1)
        {
            handleWrite(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for write command.\n";
            cout << "Usage: write [file_path] or [file_name]\n";
        }
    }
    else if (cmd.name == "echo")
    {
        // Create an empty file
        if (cmd.arguments.size() == 1)
        {
            handleEcho(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for echo command.\n";
            cout << "Usage: echo [file_path]\n";
        }
    }
    else if (cmd.name == "dir")
    {
        // List contents of a directory
        if (cmd.arguments.empty())
        {
            handleDir("");
        }
        else if (cmd.arguments.size() == 1)
        {
            handleDir(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for dir command.\n";
            cout << "Usage:\n  dir\n  dir [path]\n";
        }
    }
    else if (cmd.name == "import")
    {
        // Import files to the virtual disk
        if (!cmd.arguments.empty())
        {
            handleImport(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for import command.\n";
            cout << "Usage:\n  import [source]\n  import [source] [destination]\n";
        }
    }
    else if (cmd.name == "md")
    {
        // Create a new directory
        if (cmd.arguments.size() == 1)
        {
            handleMd(cmd.arguments[0]);
        }
        else
        {
            cout << "Error: Invalid syntax for md command.\n";
            cout << "Usage: md [directory_name]\n";
        }
    }
    else if (cmd.name == "cd")
    {
        // Change the current directory
        if (cmd.arguments.size() <= 1)
        {
            string path = cmd.arguments.empty() ? "" : cmd.arguments[0];
            handleCd(path);
        }
        else
        {
            cout << "Error: Invalid syntax for cd command.\n";
            cout << "Usage:\n  cd\n  cd [directory]\n";
        }
    }
    else if (cmd.name == "pwd")
    {
        // Show the current working directory
        if (cmd.arguments.empty())
        {
            handlePwd();
        }
        else
        {
            cout << "Error: Invalid syntax for pwd command.\n";
        }
    }
    else if (cmd.name == "history")
    {
        // Display the command history
        if (cmd.arguments.empty())
        {
            handleHistory();
        }
        else
        {
            cout << "Error: Invalid syntax for history command.\n";
            cout << "Usage: history\n";
        }
    }
    else if (cmd.name == "cls")
    {
        // Clear the screen
        if (cmd.arguments.empty())
        {
            handleCls();
        }
        else
        {
            cout << "Error: Invalid syntax for cls command.\n";
        }
    }
    else if (cmd.name == "export")
    {
        // Export files from the virtual disk
        if (cmd.arguments.size() >= 1 && cmd.arguments.size() <= 2)
        {
            handleExport(cmd.arguments);
        }
        else
        {
            cout << "Error: Invalid syntax for export command.\n";
            cout << "Usage: export [source_file_or_directory] [destination_file_or_directory]\n";
        }
    }
    else if (cmd.name == "rd")
    {
        // Remove directories
        if (cmd.arguments.empty())
        {
            cout << "Error: Invalid syntax for rd command.\n";
            cout << "Usage: rd [directory]+\n";
        }
        else
        {
            handleRd(cmd.arguments);
        }
    }
    else if (cmd.name == "quit")
    {
        // Exit the application
        if (cmd.arguments.empty())
        {
            handleQuit(isRunning);
        }
        else
        {
            cout << "Error: Invalid syntax for quit command.\n";
            cout << "Usage: quit\n";
        }
    }
    else
    {
        // Handle unknown commands
        cout << "Error: Unknown command '" << cmd.name << "'. Type 'help' to see available commands.\n";
    }
}


// Converts a given string to lowercase
string CommandProcessor::toLower(const string& s)
{
    // Create a copy of the input string to store the result
    string result = s;

    // Transform each character in the string to lowercase
    transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return tolower(c); });

    // Return the lowercase version of the string
    return result;
}



// Converts a given string to uppercase
string CommandProcessor::toUpper(const string& s)
{
    // Create a copy of the input string to store the result
    string result = s;

    // Transform each character in the string to uppercase
    transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return toupper(c); });

    // Return the uppercase version of the string
    return result;
}


// Removes leading and trailing whitespace characters from a string
string CommandProcessor::trimString(const string& input)
{
    string result = input;

    // Trim leading spaces (whitespace characters: space, tab, carriage return, newline)
    result.erase(0, result.find_first_not_of(" \t\r\n"));

    // Trim trailing spaces
    result.erase(result.find_last_not_of(" \t\r\n") + 1);

    // Return the trimmed string
    return result;
}


// Displays a list of available commands with brief descriptions
void CommandProcessor::showGeneralHelp()
{
    cout << "Available Commands:\n";
    cout << "-----------------------------------------------------------------------------------\n";

    // Determine the maximum length of command names for alignment
    size_t maxCommandLength = 0;
    for (const auto& cmd : commandHelp)
    {
        maxCommandLength = max(maxCommandLength, cmd.first.length());
    }

    // Iterate through the commandHelp map and display each command with its description
    int count = 1;
    for (const auto& cmd : commandHelp)
    {
        cout << "  " << setw(2) << count << ". " // Align command numbers
            << left << setw(maxCommandLength + 2) << cmd.first // Align command names
            << "- " << cmd.second.first << "\n"; // Description
        count++;
    }

    cout << "-----------------------------------------------------------------------------------\n";
}

// Displays detailed help information for a specific command
void CommandProcessor::showCommandHelp(const string& command)
{
    // Convert the command name to lowercase to ensure case-insensitive matching
    string cmdLower = toLower(command);

    // Attempt to find the command in the commandHelp map
    auto it = commandHelp.find(cmdLower);

    // If the command is found, display its detailed help
    if (it != commandHelp.end())
    {
        cout << it->second.second << "\n"; // Access and display the detailed description
    }
    else
    {
        // Display an error message if the command is not recognized
        cout << "Error: Command '" << command << "' is not supported.\n";
    }
}




// Clears the console screen
void CommandProcessor::handleCls()
{
    // Use the system command to clear the screen
    // On Windows, the "cls" command clears the console screen
    system("cls");
}



// Handles the "md" command to create a new directory
void CommandProcessor::handleMd(const string& dirPath)
{
    // Step 1: Trim spaces from the input
    string trimmedPath = trimString(dirPath);

    // Step 2: Check if the trimmed input is empty and show an error message if it is
    if (trimmedPath.empty()) {
        cout << "Error: Invalid syntax for md command.\n";
        cout << "Usage: md [directory_name]\n";
        return;
    }

    // Step 3: Parse the path to separate the parent path and the new directory name
    auto [parentPath, dirName] = Parser::parsePath(dirPath);
    
    // Step 4: Navigate to the parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty()) {
        parentDir = *currentDirectoryPtr; // Default to the current directory
    }
    else {
        parentDir = MoveToDir(parentPath); // Navigate to the specified parent path
    }

    // Step 5: Handle the case where the parent directory does not exist
    if (parentDir == nullptr) {
        cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
        return;
    }

    // Step 6: Check if a directory with the same name already exists
    for (const auto& entry : parentDir->DirOrFiles) {
        if (!entry.getIsFile()) { // Ensure we only compare directories
            string existingName = toLower(entry.getName());
            string newName = toLower(dirName);

            if (existingName == newName) {
                cout << "Error: Directory '" << dirName << "' already exists.\n";
                return;
            }
        }
    }

    // Step 7: Allocate a new cluster for the directory
    int newCluster = Mini_FAT::getAvailableCluster();
    if (newCluster == -1) {
        cout << "Error: No available clusters to create directory.\n";
        return;
    }

    // Step 8: Initialize the new directory's FAT pointer
    Mini_FAT::setClusterPointer(newCluster, -1); // -1 indicates end of file (EOF)

    // Step 9: Clean the directory name without altering its case
    string cleanedName = Directory_Entry::cleanTheName(dirName);
    if (cleanedName.empty()) {
        cout << "Error: Invalid directory name.\n";
        return;
    }

    // Step 10: Create a new Directory object
    Directory* newDir = new Directory(cleanedName, 0x10, newCluster, parentDir);
    newDir->readDirectory(); // Initialize directory entries (e.g., add '.' and '..')

    // Step 11: Create a Directory_Entry for the new directory
    Directory_Entry newDirEntry(cleanedName, 0x10, newCluster);

    // Step 12: Manually assign the subDirectory pointer
    newDirEntry.subDirectory = newDir;

    // Step 13: Add the new directory entry to the parent directory's DirOrFiles list
    parentDir->DirOrFiles.push_back(newDirEntry);

    // Step 14: Write the updated parent directory to the virtual disk
    parentDir->writeDirectory();

    // Step 15: Confirm successful creation of the directory
    cout << "Directory '" << cleanedName << "' created successfully.\n";
}



// Handles the "rd" command to delete one or more directories
void CommandProcessor::handleRd(const vector<string>& directories)
{
    // Iterate over each directory specified in the arguments
    for (const auto& dirPath : directories) {
        // Step 1: Confirm deletion from the user
        cout << "Are you sure you want to delete directory '" << dirPath << "'? (y/n): ";
        string input;
        getline(cin, input); // Use getline to handle multi-character input safely

        // If the user does not confirm, skip this directory
        if (input.empty() || (input[0] != 'y' && input[0] != 'Y')) {
            cout << "Skipped deleting directory '" << dirPath << "'.\n";
            continue;
        }

        // Step 2: Parse the path into parent path and directory name
        auto [parentPath, dirName] = Parser::parsePath(dirPath);

        // Step 3: Navigate to the parent directory
        Directory* parentDir = nullptr;
        if (parentPath.empty()) {
            parentDir = *currentDirectoryPtr; // Use the current directory if no parent path is specified
        }
        else {
            parentDir = MoveToDir(parentPath); // Navigate to the specified parent directory
        }

        // Step 4: Handle invalid parent directory
        if (parentDir == nullptr) {
            cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            continue;
        }

        // Step 5: Search for the directory within the parent directory
        int dirIndex = parentDir->searchDirectory(dirName);
        if (dirIndex == -1) {
            cout << "Error: Directory '" << dirName << "' does not exist.\n";
            continue;
        }

        // Step 6: Validate that the entry is a directory
        Directory_Entry dirEntry = parentDir->DirOrFiles[dirIndex];
        if (dirEntry.dir_attr != 0x10) { // 0x10 signifies a directory
            cout << "Error: '" << dirName << "' is not a directory.\n";
            continue;
        }

        // Step 7: Check if the directory is empty
        if (!dirEntry.subDirectory->isEmpty()) {
            cout << "Error: Directory '" << dirPath << "' is not empty.\n";
            continue;
        }

        // Step 8: Proceed with deleting the directory
        delete dirEntry.subDirectory; // Free memory associated with the subdirectory
        parentDir->DirOrFiles.erase(parentDir->DirOrFiles.begin() + dirIndex); // Remove entry from parent directory
        parentDir->writeDirectory(); // Save changes to the virtual disk

        cout << "Directory '" << dirPath << "' deleted successfully.\n";
    }
}





// Handles the "cd" command to navigate directories
void CommandProcessor::handleCd(const string& path)
{
    if (path.empty())
    {
        // Step 1: Display the current directory if no path is provided
        cout << "Current Directory: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        return;
    }

    // Step 2: Handle special cases for "." and ".."
    if (path == ".")
    {
        // "." indicates the current directory; no action is required
        cout << "Navigating to current directory (no change).\n";
        return;
    }
    else if (path == "..")
    {
        // ".." indicates the parent directory
        if ((*currentDirectoryPtr)->parent != nullptr)
        {
            *currentDirectoryPtr = (*currentDirectoryPtr)->parent;
            cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        }
        else
        {
            cout << "Error: Already at the root directory.\n";
        }
        return;
    }

    // Step 3: Determine if the path is absolute
    bool isAbsolute = false;
    Directory* traversalDir = *currentDirectoryPtr;
    string drive = "";
    size_t startIndex = 0;

    // Step 4: Check if the path starts with a drive letter (e.g., "C:\")
    if (path.length() >= 3 && isalpha(path[0]) && path[1] == ':' && path[2] == '\\')
    {
        isAbsolute = true;
        drive = toUpper(path.substr(0, 2)); // Extract the drive letter (e.g., "C:")

        // Traverse to the root directory
        while (traversalDir->parent != nullptr)
        {
            traversalDir = traversalDir->parent;
        }

        // Verify the drive matches
        string traversalDrive = toUpper(traversalDir->name.substr(0, 2));
        if (traversalDrive != drive)
        {
            cout << "Error: Drive '" << drive << "' not found.\n";
            return;
        }

        // Update the path to remove the drive portion
        string updatedPath = path.substr(3); // Remove "C:\"
        vector<string> pathComponents;
        stringstream ss(updatedPath);
        string component;
        while (getline(ss, component, '\\'))
        {
            if (!component.empty())
                pathComponents.push_back(component);
        }

        // Step 5: Traverse the absolute path
        for (const auto& dirName : pathComponents)
        {
            if (dirName == ".")
            {
                // "." means no change; continue to the next component
                continue;
            }
            else if (dirName == "..")
            {
                // ".." means move to the parent directory
                if (traversalDir->parent != nullptr)
                {
                    traversalDir = traversalDir->parent;
                }
                else
                {
                    cout << "Error: Already at the root directory.\n";
                    return;
                }
            }
            else
            {
                // Search for the directory
                int dirIndex = traversalDir->searchDirectory(dirName);
                if (dirIndex == -1)
                {
                    cout << "Error: System cannot find the specified folder '" << dirName << "'.\n";
                    return;
                }

                Directory_Entry* subDirEntry = &traversalDir->DirOrFiles[dirIndex];
                if (subDirEntry->dir_attr != 0x10) // Check if it's a directory
                {
                    cout << "Error: '" << dirName << "' is not a directory.\n";
                    return;
                }

                // Move to the subdirectory
                traversalDir = subDirEntry->subDirectory;
            }
        }

        // Update the current directory pointer
        *currentDirectoryPtr = traversalDir;
        cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
        return;
    }

    // Step 6: Handle relative paths
    vector<string> pathComponents;
    stringstream ssNonAbsolute(path);
    string component;
    while (getline(ssNonAbsolute, component, '\\'))
    {
        if (!component.empty())
            pathComponents.push_back(component);
    }

    // Traverse the relative path
    bool errorOccurred = false;
    for (const auto& dirName : pathComponents)
    {
        if (dirName == ".")
        {
            // "." means no change; continue to the next component
            continue;
        }
        else if (dirName == "..")
        {
            // ".." means move to the parent directory
            if (traversalDir->parent != nullptr)
            {
                traversalDir = traversalDir->parent;
            }
            else
            {
                cout << "Error: Already at the root directory.\n";
                errorOccurred = true;
                break;
            }
        }
        else
        {
            // Search for the directory
            int dirIndex = traversalDir->searchDirectory(dirName);
            if (dirIndex == -1)
            {
                cout << "Error: System cannot find the specified folder '" << dirName << "'.\n";
                errorOccurred = true;
                break;
            }

            Directory_Entry* subDirEntry = &traversalDir->DirOrFiles[dirIndex];
            if (subDirEntry->dir_attr != 0x10) // Check if it's a directory
            {
                cout << "Error: '" << dirName << "' is not a directory.\n";
                errorOccurred = true;
                break;
            }

            // Move to the subdirectory
            traversalDir = subDirEntry->subDirectory;
        }
    }

    // Step 7: Update current directory if no errors occurred
    if (!errorOccurred)
    {
        *currentDirectoryPtr = traversalDir;
        cout << "Changed directory to: " << (*currentDirectoryPtr)->getFullPath() << "\n";
    }
}




// Handles the "pwd" command to display the current directory path
void CommandProcessor::handlePwd()
{
    // Get the current directory pointer
    Directory* currentDir = *currentDirectoryPtr;

    // Display the full path of the current directory
    cout << "Current Directory: " << currentDir->getFullPath() << "\n";
}




// Handles the "history" command to display the list of executed commands
void CommandProcessor::handleHistory()
{
    // Display a header for the command history
    cout << "Command History:\n";
    cout << "----------------\n";

    // Iterate through the command history and display each command with an index
    for (size_t i = 0; i < commandHistory.size(); ++i)
    {
        cout << i + 1 << ": " << commandHistory[i] << "\n";
    }

    // If no commands have been executed, display a friendly message
    if (commandHistory.empty())
    {
        cout << "No commands in history.\n";
    }
}




// Handles the "quit" command to exit the shell
void CommandProcessor::handleQuit(bool& isRunning)
{
    // Display a stylized message indicating the shell is quitting
    cout << "************************************************************************************************************************\n";
    cout << "                                                    Quitting the Shell                                                  \n";
    cout << "************************************************************************************************************************\n";

    // Set the isRunning flag to false to terminate the shell loop
    isRunning = false;
}



// Navigates to a file based on the provided path and returns a File_Entry pointer
File_Entry* CommandProcessor::MoveToFile(string& path)
{
    // **Step 1: Split the Path into Directory Path and File Name**
    // Find the last backslash in the path to separate the directory path from the file name
    size_t lastBackslash = path.find_last_of('\\');
    if (lastBackslash == string::npos)
    {
        // **Error: Invalid File Path Format**
        // If no backslash is found, the path is invalid (e.g., it doesn't specify a directory)
        cout << "Error: Invalid file path format.\n";
        return nullptr; // Return nullptr to indicate failure
    }

    // Extract the directory path (everything before the last backslash)
    string dirPath = path.substr(0, lastBackslash);
    // Extract the file name (everything after the last backslash)
    string fileName = path.substr(lastBackslash + 1);

    // **Step 2: Validate the File Name**
    // Check if the file name is empty (invalid)
    if (fileName.empty())
    {
        cout << "Error: File name is empty.\n";
        return nullptr; // Return nullptr to indicate failure
    }

    // **Step 3: Navigate to the Target Directory**
    // Use the MoveToDir function to navigate to the directory specified in the path
    Directory* targetDir = MoveToDir(dirPath);
    if (targetDir == nullptr)
    {
        // If MoveToDir fails, it will already print an error message
        return nullptr; // Return nullptr to indicate failure
    }

    // **Step 4: Search for the File in the Target Directory**
    // Search for the file in the target directory using the file name
    int fileIndex = targetDir->searchDirectory(fileName);
    if (fileIndex == -1)
    {
        // **Error: File Not Found**
        // If the file is not found, print an error message
        cout << "Error: File '" << fileName << "' not found in '" << targetDir->getFullPath() << "'.\n";
        return nullptr; // Return nullptr to indicate failure
    }

    // **Retrieve the Directory_Entry for the File**
    // Get the Directory_Entry object corresponding to the file
    Directory_Entry& fileEntry = targetDir->DirOrFiles[fileIndex];

    // **Step 5: Validate That the Entry is a File**
    // Check if the entry is a directory (0x10 indicates a directory)
    if (fileEntry.dir_attr == 0x10)
    {
        // **Error: Entry is a Directory**
        // If the entry is a directory, print an error message
        cout << "Error: '" << fileName << "' is a directory.\n";
        return nullptr; // Return nullptr to indicate failure
    }

    // **Step 6: Create and Return a File_Entry Object**
    // Create a new File_Entry object for the file
    File_Entry* file = new File_Entry(fileEntry, targetDir);

    // **Load the File's Content**
    // Read the file's content into memory (if necessary)
    file->readFileContent();

    // Return the File_Entry object
    return file;
}


// Navigates to a directory based on the provided path and returns a Directory pointer
Directory* CommandProcessor::MoveToDir(const string& path)
{
    // **Step 1: Normalize the Input Path**
    // Create a local modifiable copy of the input path to work with
    string normalizedPath = path;

    // **Step 2: Split the Path into Components**
    // Use a stringstream to split the path by the backslash ('\\') delimiter
    vector<string> dirs; // Stores the individual directory components
    stringstream ss(normalizedPath);
    string token;
    while (getline(ss, token, '\\'))
    {
        if (!token.empty()) {
            dirs.push_back(token); // Add non-empty tokens (directory names) to the vector
        }
    }

    // **Step 3: Validate the Path**
    // Check if the path is empty after splitting
    if (dirs.empty())
    {
        cout << "Error: Path is empty.\n";
        return nullptr; // Return nullptr if the path is invalid
    }

    // **Step 4: Start at the Current Directory**
    // Begin traversal from the current directory
    Directory* current = *currentDirectoryPtr;

    // **Step 5: Handle Root Navigation**
    // Check if the path starts with the root drive (e.g., "C:")
    string rootDrive = toUpper(current->getDrive()) + ":"; // Get the current drive (e.g., "C:")
    if (toUpper(dirs[0]) == rootDrive)
    {
        // Navigate to the root directory by moving up the directory tree
        while (current->parent != nullptr)
        {
            current = current->parent; // Move to the parent directory
        }
        dirs.erase(dirs.begin()); // Remove the root drive from the path components
    }

    // **Step 6: Traverse the Path Components**
    // Iterate through each directory component in the path
    for (const auto& dirName : dirs)
    {
        // **Search for the Directory in the Current Directory**
        int dirIndex = current->searchDirectory(dirName); // Search for the directory by name
        if (dirIndex == -1)
        {
            // **Error: Directory Not Found**
            cout << "Error: Directory '" << dirName << "' not found in '" << current->getFullPath() << "'.\n";
            return nullptr; // Return nullptr if the directory is not found
        }

        // **Retrieve the Directory Entry**
        Directory_Entry& entry = current->DirOrFiles[dirIndex]; // Get the directory entry

        // **Validate the Entry Type**
        if (entry.dir_attr != 0x10) // Check if the entry is a directory (0x10 indicates a directory)
        {
            cout << "Error: '" << dirName << "' is not a directory.\n";
            return nullptr; // Return nullptr if the entry is not a directory
        }

        // **Move to the Subdirectory**
        current = entry.subDirectory; // Update the current directory to the subdirectory
        if (!current)
        {
            // **Error: Subdirectory Not Accessible**
            cout << "Error: Subdirectory '" << dirName << "' is not accessible.\n";
            return nullptr; // Return nullptr if the subdirectory is inaccessible
        }
    }

    // **Step 7: Return the Final Directory**
    // After traversing all path components, return the final directory
    return current;
}



// Handles the "dir" command to display the contents of a directory
void CommandProcessor::handleDir(const string& path)
{
    // Step 1: Determine the target directory to list
    Directory* targetDir = *currentDirectoryPtr;

    // Handle special cases for "." and ".."
    if (path == ".") {
        // Stay in the current directory
    }
    else if (path == "..") {
        // Navigate to the parent directory if it exists
        if (targetDir->parent != nullptr) {
            targetDir = targetDir->parent;
        }
        else {
            // Already at the root directory
            cout << "Error: Already at the root directory.\n";
            return;
        }
    }
    else if (!path.empty()) {
        // Resolve the path using MoveToDir
        Directory* resolvedDir = MoveToDir(path);
        if (!resolvedDir) {
            // MoveToDir prints an error if the path is invalid
            return;
        }
        targetDir = resolvedDir;
    }

    // Step 2: Print the header with the directory's full path
    cout << "Directory of " << targetDir->getFullPath() << "\n\n";

    int fileCount = 0;        // Count of files in the directory
    int dirCount = 0;         // Count of subdirectories
    long long totalSize = 0;  // Total size of files in bytes

    vector<Directory_Entry> directories; // List to store directory entries
    vector<Directory_Entry> files;       // List to store file entries

    // Step 3: Separate directories and files
    for (const auto& entry : targetDir->DirOrFiles) {
        if (entry.dir_attr == 0x10) { // Check if entry is a directory
            if (entry.getName() == "." || entry.getName() == "..") continue; // Skip "." and ".."
            directories.push_back(entry);
        }
        else {
            // Entry is a file
            files.push_back(entry);
        }
    }

    // Step 4: Define column widths for formatting
    const int nameWidth = 40;
    const int sizeWidth = 15;

    // Step 5: Print column headers for name and size
    cout << left << setw(nameWidth) << "Name"
        << right << setw(sizeWidth) << "Size\n";
    cout << left << setw(nameWidth) << "----"
        << right << setw(sizeWidth) << "----\n";

    // Step 6: Display directories first
    for (const auto& d : directories) {
        string name = d.getName();
        if (name.empty()) {
            name = "<No Directory Name>";
        }
        string attr = "<DIR>";

        // Print directory entry
        cout << left << setw(nameWidth) << (name + " " + attr)
            << right << setw(sizeWidth) << "-" << "\n";
        dirCount++;
    }

    // Step 7: Display files
    for (const auto& f : files) {
        string name = f.getName();
        if (name.empty()) {
            name = "<No Name>.txt"; // Default to ".txt" if name is empty
        }
        int size = f.getSize(); // Get the size of the file

        // Print file entry
        cout << left << setw(nameWidth) << name
            << right << setw(sizeWidth) << size << " bytes\n";
        fileCount++;
        totalSize += size;
    }

    // Step 8: Calculate free space using Mini_FAT methods
    long long totalClusters = Mini_FAT::getTotalClusters();
    long long freeClusters = Mini_FAT::getFreeClusters();
    long long clusterSize = Mini_FAT::getClusterSize();
    long long freeSpace = freeClusters * clusterSize;

    // Step 9: Print summary
    cout << "\n"
        << left << setw(nameWidth) << ""
        << right << setw(sizeWidth) << fileCount << " File(s)     " << totalSize << " bytes\n"
        << left << setw(nameWidth) << ""
        << right << setw(sizeWidth) << dirCount << " Dir(s)      " << freeSpace << " bytes free\n";
}




// Handles the "echo" command to create a new empty file
void CommandProcessor::handleEcho(const string& filePath)
{
    // Step 1: Trim leading and trailing spaces from the file path
    string trimmed = trimString(filePath);

    // Step 2: Parse the path into parent directory and file name
    pair<string, string> pathParts = Parser::parsePath(trimmed);
    string parentPath = pathParts.first;
    string fileName = pathParts.second;

    // Step 3: Validate the file name
    if (!isValidFileName(fileName)) {
        cout << "Error: Invalid file name '" << fileName << "'.\n";
        return;
    }

    // Step 4: Check for a valid extension, append ".txt" if missing
    size_t dotPos = fileName.find_last_of('.');
    if (dotPos == string::npos || dotPos == 0 || dotPos == fileName.length() - 1) {
        fileName += ".txt"; // Add a default ".txt" extension
    }

    // Step 5: Navigate to the parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty()) {
        // Use the current directory if no parent path is provided
        parentDir = *currentDirectoryPtr;
    }
    else {
        // Resolve the parent directory from the path
        parentDir = MoveToDir(parentPath);
        if (parentDir == nullptr) {
            cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            return;
        }
    }

    // Step 6: Check for duplicate files (case-insensitive comparison)
    for (const auto& entry : parentDir->DirOrFiles) {
        string existingName = toLower(entry.getName());
        string newName = toLower(fileName);

        if (existingName == newName) {
            cout << "Error: File '" << fileName << "' already exists.\n";
            return;
        }
    }

    // Step 7: Create the file entry
    Directory_Entry newFileEntry(fileName, 0x00, 0); // 0x00 indicates a file
    newFileEntry.setIsFile(true); // Mark the entry as a file

    // Step 8: Add the new file to the parent directory
    parentDir->DirOrFiles.push_back(newFileEntry);

    // Step 9: Persist changes to the directory
    parentDir->writeDirectory();

    // Step 10: Confirm file creation
    cout << "File '" << newFileEntry.getName() << "' created successfully.\n";
}




// Handles the "write" command to write content to an existing file
void CommandProcessor::handleWrite(const string& filePath)
{
    // Step 1: Parse the path into parent directory and file name
    pair<string, string> pathParts = Parser::parsePath(filePath);
    string parentPath = pathParts.first;
    string fileName = pathParts.second;

    // Step 2: Validate the file name
    if (!isValidFileName(fileName))
    {
        cout << "Error: Invalid file name '" << fileName << "'.\n";
        return;
    }

    // Step 3: Navigate to the parent directory
    Directory* parentDir = nullptr;
    if (parentPath.empty())
    {
        // Default to the current directory if no parent path is provided
        parentDir = *currentDirectoryPtr;
    }
    else
    {
        parentDir = MoveToDir(parentPath);
    }

    // Handle invalid parent directory
    if (parentDir == nullptr)
    {
        cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
        return;
    }

    // Step 4: Search for the file in the parent directory
    bool fileFound = false;
    for (auto& entry : parentDir->DirOrFiles) // Iterate by reference to modify entries if needed
    {
        string lowerFileName = toLower(fileName); // Convert input file name to lowercase
        string entryName = toLower(entry.getName()); // Convert entry name to lowercase for comparison

        if (entryName == lowerFileName) // Case-insensitive match
        {
            // Step 5: Ensure the entry is a file
            if (!entry.getIsFile())
            {
                cout << "Error: '" << fileName << "' is a directory, not a file.\n";
                return;
            }

            // Step 6: Prompt user for input to write to the file
            cout << "Enter text to write to '" << fileName << "'. Type 'END' on a new line to finish.\n";

            string line;
            string newContent;
            while (true)
            {
                getline(cin, line);
                if (line == "END")
                    break;
                newContent += line + "\n";
            }

            // Step 7: Update the file content
            entry.setContent(newContent);

            // Step 8: Persist the changes to disk
            parentDir->writeDirectory();

            // Step 9: Confirm success
            cout << "Content written to '" << fileName << "' successfully.\n";
            fileFound = true;
            break;
        }
    }

    // Step 10: Handle case where the file is not found
    if (!fileFound)
    {
        cout << "Error: File '" << fileName << "' does not exist.\n";
    }
}



// Validates a file name based on specific rules
bool CommandProcessor::isValidFileName(const string& name)
{
    // Check if the name is empty or exceeds the allowed length
    if (name.empty() || name.length() > 11)
    {
        return false;
    }

    // Step 1: Check for invalid characters
    const string invalidChars = R"(/\*?"<>|)";
    for (char c : name)
    {
        if (invalidChars.find(c) != string::npos)
        {
            return false;
        }
    }

    // Step 2: Prevent names starting with a dot
    if (name[0] == '.')
    {
        return false;
    }

    // Step 3: Ensure the file name follows the 8.3 format
    size_t dotPos = name.find_last_of('.');
    if (dotPos != string::npos)
    {
        // Split the base name and extension
        string base = name.substr(0, dotPos);
        string ext = name.substr(dotPos + 1);

        // Validate the base name and extension length
        if (base.empty() || ext.empty() || base.length() > 8 || ext.length() > 3)
        {
            return false;
        }
    }
    else
    {
        // Allow up to 11 characters for names without an extension (e.g., directories)
        if (name.length() > 11)
        {
            return false;
        }
    }

    return true;
}



// Handles the "type" command to display the content of one or more files
void CommandProcessor::handleType(const vector<string>& filePaths)
{
    // Iterate over each file path provided in the command
    for (const string& filePath : filePaths)
    {
        // Step 1: Parse the file path into parent directory and file name
        pair<string, string> pathParts = Parser::parsePath(filePath);
        string parentPath = pathParts.first;
        string fileName = pathParts.second;

        // Step 2: Navigate to the parent directory
        Directory* parentDir = nullptr;
        if (parentPath.empty())
        {
            // Use the current directory if no parent path is specified
            parentDir = *currentDirectoryPtr;
        }
        else
        {
            // Resolve the parent directory from the path
            parentDir = MoveToDir(parentPath);
        }

        // Handle invalid parent directory
        if (parentDir == nullptr)
        {
            cout << "Error: Directory path '" << parentPath << "' does not exist.\n";
            continue; // Skip to the next file
        }

        // Step 3: Search for the file in the parent directory
        bool fileFound = false;
        for (const auto& entry : parentDir->DirOrFiles)
        {
            // Perform case-insensitive comparison for the file name
            string lowerFileName = toLower(fileName);
            string entryName = toLower(entry.getName());

            if (entryName == lowerFileName) // File found
            {
                // Step 4: Ensure the entry is a file and not a directory
                if (!entry.getIsFile())
                {
                    cout << "Error: '" << fileName << "' is a directory, not a file.\n";
                    fileFound = true; // Mark as found to avoid general not found message
                    break;
                }

                // Step 5: Display the file content
                cout << "Content of '" << fileName << "':\n";
                cout << entry.getContent() << "\n"; // Assuming getContent() retrieves file content
                fileFound = true;
                break;
            }
        }

        // Step 6: Handle case where the file is not found
        if (!fileFound)
        {
            cout << "Error: File '" << fileName << "' does not exist.\n";
        }
    }
}




// Handles the "del" command to delete files or directories
void CommandProcessor::handleDel(const vector<string>& targets)
{
    for (const auto& target : targets)
    {
        Directory* parentDir = nullptr;
        Directory_Entry* dirEntry = nullptr;
        string entryName;
        string dirPath;

        // Step 1: Determine if the target is a full path or relative
        bool isFullPath = (target.find(":\\") != string::npos || target[0] == '\\');
        if (isFullPath)
        {
            // Extract the directory path and entry name
            size_t lastSlash = target.find_last_of("\\");
            if (lastSlash == string::npos || lastSlash == target.length() - 1)
            {
                cout << "Error: Invalid path '" << target << "'.\n";
                continue;
            }

            dirPath = target.substr(0, lastSlash);
            entryName = target.substr(lastSlash + 1);
            parentDir = MoveToDir(dirPath);

            // Check if the parent directory exists
            if (!parentDir)
            {
                cout << "Error: Path '" << dirPath << "' does not exist.\n";
                continue;
            }
        }
        else
        {
            // Use the current directory for relative paths
            parentDir = *currentDirectoryPtr;
            entryName = target;
        }

        // Step 2: Search for the entry in the parent directory
        int entryIndex = parentDir->searchDirectory(entryName);
        if (entryIndex == -1)
        {
            cout << "Error: '" << entryName << "' does not exist in '" << parentDir->getFullPath() << "'.\n";
            continue;
        }

        dirEntry = &parentDir->DirOrFiles[entryIndex];

        // Step 3: Handle directories
        if (dirEntry->dir_attr == 0x10) // Directory
        {
            cout << "Are you sure you want to delete all files in the directory '"
                << dirEntry->getName() << "'? (y/n): ";
            char confirmation;
            cin >> confirmation;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (tolower(confirmation) == 'y')
            {
                string fullPath = parentDir->getFullPath() + "\\" + dirEntry->getName();
                Directory* targetDir = MoveToDir(fullPath);

                if (!targetDir)
                {
                    cout << "Error: Could not access the directory '" << dirEntry->getName() << "'.\n";
                    continue;
                }

                // Delete all files in the directory
                for (auto it = targetDir->DirOrFiles.begin(); it != targetDir->DirOrFiles.end();)
                {
                    if (it->dir_attr != 0x10) // Only delete files
                    {
                        string fileName = it->getName();
                        cout << "Are you sure you want to delete the file '" << fileName << "'? (y/n): ";
                        cin >> confirmation;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');

                        if (tolower(confirmation) == 'y')
                        {
                            File_Entry file(*it, targetDir);
                            file.deleteFile();
                            cout << "File '" << fileName << "' deleted successfully.\n";
                            it = targetDir->DirOrFiles.erase(it); // Remove file from directory
                        }
                        else
                        {
                            ++it;
                        }
                    }
                    else
                    {
                        ++it; // Skip subdirectories
                    }
                }

                targetDir->writeDirectory(); // Persist changes
                cout << "All files in the directory '" << dirEntry->getName() << "' have been processed.\n";
            }
            else
            {
                cout << "Skipped deletion of files in directory '" << dirEntry->getName() << "'.\n";
            }
        }
        else // Step 4: Handle files
        {
            string fileName = dirEntry->getName();
            cout << "Are you sure you want to delete the file '" << fileName << "'? (y/n): ";
            char confirmation;
            cin >> confirmation;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (tolower(confirmation) == 'y')
            {
                File_Entry file(*dirEntry, parentDir);
                file.deleteFile();
                cout << "File '" << fileName << "' deleted successfully.\n";

                // Remove from DirOrFiles and persist changes
                parentDir->DirOrFiles.erase(parentDir->DirOrFiles.begin() + entryIndex);
                parentDir->writeDirectory();
            }
            else
            {
                cout << "Skipped deletion of '" << fileName << "'.\n";
            }
        }
    }
}




// Handles the "rename" command to rename a file in the virtual file system
void CommandProcessor::handleRename(const vector<string>& args)
{
    // Step 1: Validate the input arguments
    if (args.size() != 2)
    {
        cout << "Error: Invalid syntax for rename command.\n";
        cout << "Usage: rename [fileName or fullPath] [new fileName]\n";
        return;
    }

    string filePath = args[0];
    string newFileName = args[1];

    // Step 2: Validate that the new file name does not contain a path
    if (newFileName.find("\\") != string::npos || newFileName.find(":") != string::npos)
    {
        cout << "Error: The new file name should be a file name only. You cannot provide a full path.\n";
        return;
    }

    Directory* targetDir = nullptr;
    string fileName;

    // Step 3: Determine if the input is a full path or a relative name
    if (filePath.find(":\\") != string::npos || filePath[0] == '\\')
    {
        // Extract the directory path and file name from the full path
        string dirPath = filePath.substr(0, filePath.find_last_of("\\"));
        fileName = filePath.substr(filePath.find_last_of("\\") + 1);

        // Navigate to the directory
        targetDir = MoveToDir(dirPath);
        if (!targetDir)
        {
            cout << "Error: Directory path '" << dirPath << "' does not exist.\n";
            return;
        }
    }
    else
    {
        // If the input is relative, use the current directory
        targetDir = *currentDirectoryPtr;
        fileName = filePath;
    }

    // Step 4: Search for the file in the target directory
    int fileIndex = targetDir->searchDirectory(fileName);
    if (fileIndex == -1)
    {
        cout << "Error: File '" << filePath << "' does not exist.\n";
        return;
    }

    Directory_Entry& fileEntry = targetDir->DirOrFiles[fileIndex];

    // Step 5: Ensure the entry is a file, not a directory
    if (fileEntry.dir_attr == 0x10) // 0x10 indicates a directory
    {
        cout << "Error: '" << fileName << "' is a directory. Use 'rd' to rename directories.\n";
        return;
    }

    // Step 6: Check for duplicate file names in the directory
    for (const auto& entry : targetDir->DirOrFiles)
    {
        if (toLower(entry.getName()) == toLower(newFileName))
        {
            cout << "Error: A file with the name '" << newFileName << "' already exists in the directory.\n";
            return;
        }
    }

    // Step 7: Rename the file
    fileEntry.assignDir_Name(newFileName);
    targetDir->writeDirectory(); // Persist the changes to the disk

    // Step 8: Confirm the rename operation
    cout << "File '" << fileName << "' renamed to '" << newFileName << "' successfully.\n";
}

 //Handles the 'copy' command to copy files or directories within the virtual file system.
void CommandProcessor::handleCopy(const vector<string>& args)
{
    // **Case (1): Handle empty arguments (just "copy" command)**
    if (args.empty())
    {
        cout << "Error: Invalid syntax for copy command.\n";
        cout << "Usage: copy [source] [destination]\n";
        return;
    }

    // Extract source and destination paths from arguments
    string sourcePath = args[0];
    string destinationPath = args.size() > 1 ? args[1] : "";

    // **Parse the Source Path**
    Directory* sourceDir = nullptr; // Pointer to the source directory
    string sourceName; // Name of the source file or directory
    size_t lastSlash = sourcePath.find_last_of("/\\"); // Find the last slash in the source path

    if (lastSlash == string::npos)
    {
        // **Case (2): Source is in the current directory**
        sourceDir = *currentDirectoryPtr; // Set source directory to the current directory
        sourceName = sourcePath; // Source name is the entire path (no slashes)
    }
    else
    {
        // **Case (3): Source path includes directories**
        string sourceParentPath = sourcePath.substr(0, lastSlash); // Extract parent directory path
        sourceName = sourcePath.substr(lastSlash + 1); // Extract the source file or directory name
        sourceDir = MoveToDir(sourceParentPath); // Move to the parent directory
    }

    // **Check if Source Directory Exists**
    if (!sourceDir)
    {
        // **Case (5): Full path does not exist**
        cout << "Error: Source path '" << sourcePath << "' does not exist.\n";
        return;
    }

    // **Search for the Source Entry**
    int sourceIndex = sourceDir->searchDirectory(sourceName); // Search for the source entry
    if (sourceIndex == -1)
    {
        // **Case (2): Source file or directory does not exist**
        cout << "Error: Source '" << sourceName << "' does not exist.\n";
        return;
    }

    Directory_Entry& sourceEntry = sourceDir->DirOrFiles[sourceIndex]; // Reference to the source entry

    // **Handle File Copying**
    if (sourceEntry.dir_attr == 0x00) // 0x00 indicates a file
    {
        // **Determine Destination Directory and File Name**
        Directory* destinationDir = nullptr; // Pointer to the destination directory
        string destFileName; // Name of the destination file

        if (destinationPath.empty())
        {
            // **Default Destination: Current Directory with Same Name**
            destinationDir = *currentDirectoryPtr; // Set destination directory to the current directory
            destFileName = sourceName; // Destination file name is the same as the source

            // **Check for Self-Copy**
            if (sourceDir == destinationDir && sourceName == destFileName)
            {
                cout << "The file cannot be copied onto itself.\n";
                cout << "0 file(s) copied.\n";
                return;
            }
        }
        else
        {
            // **Check if Destination Path is Absolute**
            bool isAbsolute = false;
            if (destinationPath.size() >= 3 && destinationPath[1] == ':' &&
                (destinationPath[2] == '\\' || destinationPath[2] == '/'))
            {
                isAbsolute = true; // Path is absolute (e.g., "C:\...")
            }

            if (isAbsolute)
            {
                // **Destination Path is Absolute: Extract File Name**
                size_t destLastSlash = destinationPath.find_last_of("/\\");
                if (destLastSlash == string::npos)
                {
                    // **Invalid Absolute Path (No File Name)**
                    cout << "Error: Invalid destination path.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }
                destFileName = destinationPath.substr(destLastSlash + 1); // Extract file name
                destinationDir = *currentDirectoryPtr; // Copy to the current directory
            }
            else
            {
                // **Destination Path is Relative**
                size_t destLastSlash = destinationPath.find_last_of("/\\");
                if (destLastSlash == string::npos)
                {
                    // **Destination is in Current Directory**
                    destinationDir = *currentDirectoryPtr; // Set destination directory to the current directory
                    destFileName = destinationPath; // Destination file name is the provided path
                }
                else
                {
                    // **Destination Includes Directory Path**
                    string destParentPath = destinationPath.substr(0, destLastSlash); // Extract parent directory
                    destFileName = destinationPath.substr(destLastSlash + 1); // Extract file name
                    destinationDir = MoveToDir(destParentPath); // Move to the parent directory
                }
            }
        }

        // **Check if Destination Directory Exists**
        if (!destinationDir)
        {
            // **Case (5): Destination Directory Not Found**
            cout << "Error: Destination directory does not exist.\n";
            cout << "0 file(s) copied.\n";
            return;
        }

        // **Check if Destination is a Directory**
        int destIndex = destinationDir->searchDirectory(destFileName); // Search for the destination entry
        bool destIsDirectory = false; // Flag to check if the destination is a directory

        if (destIndex != -1)
        {
            // **Destination Exists**
            if (destinationDir->DirOrFiles[destIndex].dir_attr == 0x10) // 0x10 indicates a directory
            {
                // **Destination is a Directory**
                destIsDirectory = true;
                destinationDir = destinationDir->DirOrFiles[destIndex].subDirectory; // Move to the subdirectory
                destFileName = sourceName; // Copy with the same name into the destination directory
            }
        }

        if (destIsDirectory)
        {
            // **Case (13): Copy File into Destination Directory**
            // Check if File Already Exists in Destination Directory
            int existingIndex = destinationDir->searchDirectory(sourceName);
            if (existingIndex != -1)
            {
                // **Case (14): Destination File Exists - Prompt for Overwrite**
                cout << "Error: File with the name '" << sourceName << "' already exists in the destination directory.\n";
                cout << "Do you want to overwrite it? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (tolower(choice) != 'y')
                {
                    cout << "Copy operation canceled for '" << sourceName << "'.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }

                // **Overwrite Existing File**
                Directory_Entry& existingEntry = destinationDir->DirOrFiles[existingIndex];
                existingEntry = sourceEntry; // Assuming shallow copy is sufficient
                cout << "File '" << sourceName << "' overwritten successfully in the destination directory.\n";
                cout << "1 file(s) copied.\n";
                return;
            }

            // **Destination File Does Not Exist - Proceed to Copy**
            Directory_Entry newFileEntry = sourceEntry;
            newFileEntry.assignDir_Name(sourceName); // Assign the same name

            if (!destinationDir->canAddEntry(newFileEntry))
            {
                // **Case (6): Not Enough Space**
                cout << "Error: Not enough space to copy file '" << sourceName << "'.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            destinationDir->addEntry(newFileEntry); // Add the new file to the destination directory
            cout << "File '" << sourceName << "' copied successfully to the destination directory.\n";
            cout << "1 file(s) copied.\n";
            return;
        }
        else
        {
            // **Destination is a File or Intended to be a File**
            // Check for Self-Copy
            if (sourcePath == destinationPath)
            {
                // **Case (3) & (4): Self-Copy Detected**
                cout << "Error: The file cannot be copied onto itself.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            // **Check if Destination File Exists**
            if (destinationDir->searchDirectory(destFileName) != -1)
            {
                // **Case (14): Destination File Exists - Prompt for Overwrite**
                cout << "Error: File with the name '" << destFileName << "' already exists in the destination directory.\n";
                cout << "Do you want to overwrite it? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (tolower(choice) != 'y')
                {
                    cout << "Copy operation canceled for '" << destFileName << "'.\n";
                    cout << "0 file(s) copied.\n";
                    return;
                }

                // **Overwrite Existing File**
                Directory_Entry& existingEntry = destinationDir->DirOrFiles[destIndex];
                existingEntry = sourceEntry; // Assuming shallow copy is sufficient
                cout << "File '" << destFileName << "' overwritten successfully.\n";
                cout << "1 file(s) copied.\n";
                return;
            }

            // **Destination File Does Not Exist - Proceed to Copy**
            Directory_Entry newFileEntry = sourceEntry;
            newFileEntry.assignDir_Name(destFileName); // Assign the new file name

            if (!destinationDir->canAddEntry(newFileEntry))
            {
                // **Case (6): Not Enough Space**
                cout << "Error: Not enough space to copy file '" << sourceName << "'.\n";
                cout << "0 file(s) copied.\n";
                return;
            }

            destinationDir->addEntry(newFileEntry); // Add the new file to the destination directory
            cout << "File '" << sourceName << "' copied successfully as '" << destFileName << "'.\n";
            cout << "1 file(s) copied.\n";
            return;
        }
    }

    // **Handle Directory Copying**
    if (sourceEntry.dir_attr == 0x10) // 0x10 indicates a directory
    {
        // **Case (7) (BONUS): Copy All Files from Source Directory to Current Directory**
        // **Case (10): Copy All Files from Source Directory to Existing Destination Directory**
        // **Case (14): Handle Overwrite Scenarios**

        Directory* destinationDir = nullptr; // Pointer to the destination directory
        string destDirName; // Name of the destination directory

        if (destinationPath.empty())
        {
            // **Case (7) (BONUS): No Destination Path Provided - Copy to Current Directory**
            destinationDir = *currentDirectoryPtr; // Set destination directory to the current directory
        }
        else
        {
            // **Destination Path Provided - Determine if Directory Exists**
            size_t destLastSlash = destinationPath.find_last_of("/\\");
            if (destLastSlash == string::npos)
            {
                // **Destination is in Current Directory**
                destinationDir = *currentDirectoryPtr; // Set destination directory to the current directory
                destDirName = destinationPath; // Destination directory name is the provided path
            }
            else
            {
                // **Destination Includes Directory Path**
                string destParentPath = destinationPath.substr(0, destLastSlash); // Extract parent directory
                destDirName = destinationPath.substr(destLastSlash + 1); // Extract directory name
                destinationDir = MoveToDir(destParentPath); // Move to the parent directory
            }
        }

        if (!destinationDir)
        {
            // **Case (9): Destination Directory Not Found**
            cout << "Error: Destination directory '" << destinationPath << "' does not exist.\n";
            return;
        }

        // **Check if Destination Path is an Existing Directory**
        bool destIsDirectory = false; // Flag to check if the destination is a directory
        if (!destDirName.empty())
        {
            int destIndex = destinationDir->searchDirectory(destDirName); // Search for the destination directory
            if (destIndex != -1 && destinationDir->DirOrFiles[destIndex].dir_attr == 0x10)
            {
                // **Destination is an Existing Directory**
                destIsDirectory = true;
                destinationDir = destinationDir->DirOrFiles[destIndex].subDirectory; // Move to the subdirectory
            }
            else if (destIndex != -1 && destinationDir->DirOrFiles[destIndex].dir_attr != 0x10)
            {
                // **Destination Exists but is Not a Directory**
                cout << "Error: Destination path '" << destinationPath << "' is not a directory.\n";
                return;
            }
            else
            {
                // **Destination Directory Does Not Exist**
                cout << "Error: Destination directory '" << destinationPath << "' does not exist.\n";
                return;
            }
        }

        // **Iterate Through Source Directory Entries and Copy Files**
        int filesCopied = 0; // Counter for the number of files copied
        for (const auto& entry : sourceEntry.subDirectory->DirOrFiles)
        {
            if (entry.dir_attr == 0x00) // Only Copy Files (0x00 indicates a file)
            {
                string srcFileName = entry.getName(); // Get the source file name
                int destIndex = destinationDir->searchDirectory(srcFileName); // Check if the file already exists in the destination

                if (destIndex != -1)
                {
                    // **Case (14): Destination File Exists - Prompt for Overwrite**
                    cout << "Error: File with the name '" << srcFileName << "' already exists in the destination directory.\n";
                    cout << "Do you want to overwrite it? (y/n): ";
                    char choice;
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (tolower(choice) != 'y')
                    {
                        cout << "Copy operation skipped for '" << srcFileName << "'.\n";
                        continue;
                    }

                    // **Overwrite Existing File**
                    Directory_Entry& existingEntry = destinationDir->DirOrFiles[destIndex];
                    existingEntry = entry; // Assuming shallow copy is sufficient
                    cout << "File '" << srcFileName << "' overwritten successfully in destination directory.\n";
                    filesCopied++;
                    continue;
                }

                // **Destination File Does Not Exist - Proceed to Copy**
                Directory_Entry newFileEntry = entry;
                newFileEntry.assignDir_Name(srcFileName); // Assign the same name

                if (!destinationDir->canAddEntry(newFileEntry))
                {
                    // **Case (6): Not Enough Space**
                    cout << "Error: Not enough space to copy file '" << srcFileName << "'.\n";
                    continue;
                }

                destinationDir->addEntry(newFileEntry); // Add the new file to the destination directory
                cout << "File '" << srcFileName << "' copied successfully to destination directory.\n";
                filesCopied++;
            }
            // **Note**: Skipping subdirectories as per initial requirements
        }

        // **Output Summary of Copied Files**
        cout << filesCopied << " file(s) copied from directory '" << sourceName << "'.\n";
        return;
    }

    // **Unsupported Entry Type**
    cout << "Error: Unsupported entry type for '" << sourceName << "'.\n";
}

void CommandProcessor::handleImport(const vector<string>& args) {
    // **Validate the number of arguments**
    // Ensure the number of arguments is either 1 (source only) or 2 (source and destination)
    if (args.empty() || args.size() > 2) {
        cout << "Error: Invalid syntax for import command.\n";
        cout << "Usage:\n  import [source]\n  import [source] [destination]\n";
        return;
    }

    // Extract the source path from the arguments
    string source = args[0];
    // If a destination is provided, use it; otherwise, leave it empty
    string destination = (args.size() == 2) ? args[1] : "";

    // Convert the source path to a filesystem path object for easier manipulation
    fs::path sourcePath(source);

    // **Resolve the source path if it is relative**
    // If the source path is not absolute, resolve it relative to the current working directory
    if (!sourcePath.is_absolute()) {
        fs::path currentDirPath = fs::current_path(); // Physical disk's current working directory
        sourcePath = currentDirPath / sourcePath; // Append the relative path to the current directory

        // If the resolved path does not exist, check in the Debug folder
        if (!fs::exists(sourcePath)) {
            fs::path debugFolder = "C:\\Users\\omara\\Desktop\\SHELL\\SHELL\\shell\\x64\\Debug";
            sourcePath = debugFolder / source; // Append the source to the Debug folder path
        }
    }

    // **Check if the source path exists**
    if (!fs::exists(sourcePath)) {
        cout << "Error: Source path '" << source << "' does not exist.\n";
        return;
    }

    // Initialize counters and storage for imported files
    int importedFileCount = 0; // Tracks the total number of files imported
    vector<string> importedFiles; // Stores the names of successfully imported files

    // **Handle importing a single file**
    if (fs::is_regular_file(sourcePath)) {
        string fileName = sourcePath.filename().string(); // Extract the file name from the path

        Directory* targetDir = *currentDirectoryPtr; // Get the current directory in the virtual disk

        // **Check if the file already exists in the target directory**
        bool fileExists = false;
        for (const auto& entry : targetDir->DirOrFiles) {
            if (entry.getName() == fileName && entry.getIsFile()) {
                fileExists = true; // File already exists
                break;
            }
        }

        // **Prompt for overwrite if the file exists**
        if (fileExists) {
            cout << "File '" << fileName << "' already exists. Do you want to overwrite it? (yes/no): ";
            string userChoice;
            getline(cin, userChoice);
            transform(userChoice.begin(), userChoice.end(), userChoice.begin(), ::tolower);
            if (userChoice != "yes") {
                cout << "Skipped importing '" << fileName << "'.\n";
                return; // Skip import if the user chooses not to overwrite
            }
        }

        // **Read the content of the source file**
        ifstream inputFile(sourcePath, ios::binary);
        if (!inputFile.is_open()) {
            cout << "Error: Unable to open source file '" << sourcePath.string() << "'. Skipping import.\n";
            return; // Skip if the file cannot be opened
        }
        string fileContent((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
        inputFile.close();

        // **Create a new Directory_Entry for the file**
        Directory_Entry newFile(fileName, 0x00, static_cast<int>(fileContent.size()));
        newFile.setIsFile(true); // Mark it as a file
        newFile.setContent(fileContent); // Set the file content

        // **Add or overwrite the file in the virtual disk**
        if (fileExists) {
            // Overwrite the existing file
            for (auto& entry : targetDir->DirOrFiles) {
                if (entry.getName() == fileName && entry.getIsFile()) {
                    entry = newFile; // Replace the existing entry
                    break;
                }
            }
        }
        else {
            // Add the new file to the directory
            targetDir->addEntry(newFile);
            importedFiles.push_back(fileName); // Track the imported file
        }

        importedFileCount++; // Increment the imported file count
        cout << "File '" << fileName << "' imported successfully.\n";
    }
    // **Handle importing a directory**
    else if (fs::is_directory(sourcePath)) {
        // Iterate over all entries in the source directory
        for (const auto& entry : fs::directory_iterator(sourcePath)) {
            // Only process regular files with a .txt extension
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                string fileName = entry.path().filename().string(); // Extract the file name

                // **Check if the file already exists in the virtual disk**
                bool fileExists = false;
                for (const auto& dirEntry : (*currentDirectoryPtr)->DirOrFiles) {
                    if (dirEntry.getName() == fileName && dirEntry.getIsFile()) {
                        fileExists = true; // File already exists
                        break;
                    }
                }

                // **Prompt for overwrite if the file exists**
                if (fileExists) {
                    cout << "File '" << fileName << "' already exists. Do you want to overwrite it? (yes/no): ";
                    string userChoice;
                    getline(cin, userChoice);
                    transform(userChoice.begin(), userChoice.end(), userChoice.begin(), ::tolower);
                    if (userChoice != "yes") {
                        cout << "Skipped importing '" << fileName << "'.\n";
                        continue; // Skip this file if the user chooses not to overwrite
                    }
                }

                // **Read the content of the source file**
                ifstream inputFile(entry.path(), ios::binary);
                if (!inputFile.is_open()) {
                    cout << "Error: Unable to open source file '" << entry.path().string() << "'. Skipping import.\n";
                    continue; // Skip if the file cannot be opened
                }
                string fileContent((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
                inputFile.close();

                // **Create a new Directory_Entry for the file**
                Directory_Entry newFile(fileName, 0x00, static_cast<int>(fileContent.size()));
                newFile.setIsFile(true); // Mark it as a file
                newFile.setContent(fileContent); // Set the file content

                // **Add or overwrite the file in the virtual disk**
                if (fileExists) {
                    for (auto& dirEntry : (*currentDirectoryPtr)->DirOrFiles) {
                        if (dirEntry.getName() == fileName && dirEntry.getIsFile()) {
                            dirEntry = newFile; // Replace the existing entry
                            break;
                        }
                    }
                }
                else {
                    (*currentDirectoryPtr)->addEntry(newFile); // Add the new file
                    importedFiles.push_back(fileName); // Track the imported file
                }

                importedFileCount++; // Increment the imported file count
                cout << "File '" << fileName << "' imported successfully.\n";
            }
        }
    }
    // **Handle invalid source paths**
    else {
        cout << "Error: Source path is not a valid file or directory.\n";
        return;
    }

    // **Display a summary of imported files**
    cout << "\nTotal files imported: " << importedFileCount << "\n";
    for (const auto& file : importedFiles) {
        cout << "  - " << file << "\n"; // List all imported files
    }
}

void CommandProcessor::handleExport(const vector<string>& args)
{
    // **Check for valid number of arguments**
    // Ensure the number of arguments is either 1 (source only) or 2 (source and destination)
    if (args.size() < 1 || args.size() > 2)
    {
        cout << "Error: Invalid syntax for export command.\n";
        cout << "Usage: export [source_file_or_directory] [destination_file_or_directory]\n";
        return;
    }

    // Extract the source path from the arguments
    string sourcePath = args[0];
    string destinationPath;

    // **Determine the destination path**
    // If a destination path is provided, use it; otherwise, default to the Debug folder
    if (args.size() == 2)
    {
        destinationPath = args[1];
    }
    else
    {
        // Default destination path: Debug folder
        destinationPath = "C:\\Users\\omara\\Desktop\\SHELL\\SHELL\\shell\\x64\\Debug";
    }

    // Get the current directory pointer
    Directory* currentDir = *currentDirectoryPtr;
    Directory_Entry* sourceEntry = nullptr;

    // **Check if the source path is absolute**
    // An absolute path typically starts with a drive letter (e.g., "C:\")
    bool isSourceAbsolutePath = (sourcePath.length() >= 3 && isalpha(sourcePath[0]) && sourcePath[1] == ':' && (sourcePath[2] == '\\' || sourcePath[2] == '/'));

    // **Resolve the source path**
    if (isSourceAbsolutePath)
    {
        // Split the absolute path into directory path and entry name
        string dirPath = sourcePath.substr(0, sourcePath.find_last_of("\\/"));
        string entryName = sourcePath.substr(sourcePath.find_last_of("\\/") + 1);

        // Move to the directory specified in the absolute path
        Directory* resolvedDir = MoveToDir(dirPath);
        if (!resolvedDir)
        {
            cout << "Error: Directory '" << dirPath << "' does not exist.\n";
            return;
        }

        // Search for the entry (file or directory) in the resolved directory
        int entryIndex = resolvedDir->searchDirectory(entryName);
        if (entryIndex == -1)
        {
            cout << "Error: File or directory '" << entryName << "' does not exist in '" << dirPath << "'.\n";
            return;
        }

        // Get the entry from the resolved directory
        sourceEntry = &resolvedDir->DirOrFiles[entryIndex];
    }
    else
    {
        // If the source path is relative, search for the entry in the current directory
        int entryIndex = currentDir->searchDirectory(sourcePath);
        if (entryIndex == -1)
        {
            cout << "Error: File or directory '" << sourcePath << "' does not exist in the current directory.\n";
            return;
        }

        // Get the entry from the current directory
        sourceEntry = &currentDir->DirOrFiles[entryIndex];
    }

    int exportedFiles = 0; // Counter to keep track of the number of files exported

    // **Handle directory export**
    if (sourceEntry->dir_attr == 0x10) // Check if the source entry is a directory
    {
        // Create a Directory object for the source directory
        Directory sourceDir(sourceEntry->getName(), sourceEntry->dir_attr, sourceEntry->dir_firstCluster, currentDir);
        sourceDir.readDirectory(); // Read the contents of the directory

        // Iterate over each entry in the source directory
        for (const auto& entry : sourceDir.DirOrFiles)
        {
            // Only export files (skip subdirectories)
            if (entry.dir_attr != 0x10)
            {
                // Create a File_Entry object for the file
                File_Entry file(entry, &sourceDir);
                file.readFileContent(); // Read the content of the file

                // Construct the destination file path
                string destinationFilePath = (fs::path(destinationPath) / entry.getName()).string();

                // **Check for overwrite**
                // If the destination file already exists, prompt the user for confirmation
                if (fs::exists(destinationFilePath))
                {
                    cout << "File '" << destinationFilePath << "' already exists. Overwrite? (yes/no): ";
                    string choice;
                    getline(cin, choice);
                    transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
                    if (choice != "yes")
                    {
                        cout << "Skipping '" << entry.getName() << "'.\n";
                        continue;
                    }
                }

                // Open the destination file for writing in binary mode
                ofstream outFile(destinationFilePath, ios::binary);
                if (!outFile.is_open())
                {
                    cout << "Error: Unable to open destination file '" << destinationFilePath << "'.\n";
                    continue;
                }

                // Write the file content to the destination file
                outFile.write(file.content.c_str(), file.content.size());
                outFile.close();

                exportedFiles++; // Increment the exported files counter
            }
        }

        // Print the total number of files exported from the directory
        cout << "Total files exported from '" << sourceDir.getFullPath() << "': " << exportedFiles << "\n";
        return;
    }

    // **Handle single file export**
    if (sourceEntry->dir_attr != 0x10) // Check if the source entry is a file
    {
        // Create a File_Entry object for the file
        File_Entry file(*sourceEntry, currentDir);
        file.readFileContent(); // Read the content of the file

        // Determine the destination file path
        string destinationFilePath = destinationPath;
        if (fs::is_directory(destinationPath))
        {
            // If the destination is a directory, append the source file name to it
            destinationFilePath = (fs::path(destinationPath) / sourceEntry->getName()).string();
        }

        // **Check for overwrite**
        // If the destination file already exists, prompt the user for confirmation
        if (fs::exists(destinationFilePath))
        {
            cout << "File '" << destinationFilePath << "' already exists. Overwrite? (yes/no): ";
            string choice;
            getline(cin, choice);
            transform(choice.begin(), choice.end(), choice.begin(), ::tolower);
            if (choice != "yes")
            {
                cout << "Export canceled for '" << sourceEntry->getName() << "'.\n";
                return;
            }
        }

        // Open the destination file for writing in binary mode
        ofstream outFile(destinationFilePath, ios::binary);
        if (!outFile.is_open())
        {
            cout << "Error: Unable to open destination file '" << destinationFilePath << "'.\n";
            return;
        }

        // Write the file content to the destination file
        outFile.write(file.content.c_str(), file.content.size());
        outFile.close();

        exportedFiles++; // Increment the exported files counter

        // Print success message and the total number of files exported
        cout << "File '" << sourceEntry->getName() << "' exported successfully to '" << destinationFilePath << "'.\n";
        cout << "Total files exported: " << exportedFiles << "\n";
        return;
    }
}
