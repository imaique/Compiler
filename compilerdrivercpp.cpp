#include <iostream>
#include <string>
#include <filesystem>
#include "Compiler/Compiler.h"

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::filesystem::directory_iterator;

int main() {
    Compiler compiler;

    while (true) {
        cout << "Please enter the name of the file (without the extension) you want to compile, " << 
            "type \"exit\" to stop compiling files or type \"all\" to compile all files in the directory. The avaiables files in the input directory are:" << endl;
        string directory_path = "input";
        for (const auto& file : directory_iterator(directory_path)) {
            string filename = file.path().stem().string();
            cout << "- " << filename << endl;
        }
        string filename;
        cin >> filename;
        if (filename == "exit") {
            cout << "Exiting" << endl;
            break;
        }
        else if (filename == "all") {
            for (const auto& file : directory_iterator(directory_path)) {
                string filename = file.path().stem().string();
                compiler.compile(filename);
            }
        }
        else {
            compiler.compile(filename);
        }
    }
}

