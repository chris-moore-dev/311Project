#include <chrono>
#include <thread>
#include "./MariosBST.cpp"
#include "queue"
#include <iostream>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <string>

using std::string;

using namespace std;


// shared queue
vector<opsStruct> commands;

//shared map
EvansMap *evansMap = new EvansMap(750);
MariosBST *mariosBST = new MariosBST();



int EVANS_MAP = 0;
int MARIOS_BST = 1;

// change this to use buffer to read large files
vector<opsStruct> getCommands(string fileName);
float run(int object, string file, bool singleThreaded); 
void writeOutput(string fileName);
bool CompareST(string arg1, string arg2);


void* threadFunction(void* args) {
    struct opsStruct *op = (struct opsStruct*) args;
    if (op->object == EVANS_MAP) {
        op->result = evansMap->runOp(op);
    } 
    else {
        op->result = mariosBST->runOp(op);
    }
    return NULL;
}


int main(int argc, char const *argv[])
{
    if (argc != 5) {
        cout << "./userinterface has format: <inputfile.txt> <outputfile.txt> <0/1> <0/1>\n" 
        <<  "where <0/1> = <hashmap/BST>, and <0/1> = <don't/do compare to single threaded>, respectively." << endl;
        return -1;
    }

    // get the file name from the command line
    string inputFile = argv[1];
    string outputFile = argv[2];
    // Get the expected output file name
    // if input file is test.txt then expected output file is test_expected.txt
    string expectedOutputFile = inputFile.substr(0, inputFile.find_last_of(".")) + "_expected.txt";
   
    int structure = std::stoi(argv[3]);  // 0 = Hashmap, 1 = BST
    int compSingleThreaded = std::stoi(argv[4]);  // 0 = Don't compare, 1 = Do compare performance to single threaded version
    if (structure != 1 && structure != 0)
        structure = 0;
    if (compSingleThreaded != 1 && compSingleThreaded != 0)
        compSingleThreaded = 0;

    // run the program 
    // Added singleThreaded boolean for testing purposes (default = false)
    if (compSingleThreaded == 0) {
        float mt = run(structure, inputFile, false);
        cout << "Elapsed time for " << inputFile << ": " << mt << " s" << endl;
        writeOutput(outputFile);
    } else {
        // run this 10 times and average the results DONT WRITE TO FILE
        float stavg = 0;
        // stavg += run(structure, inputFile, true);
        // writeOutput("ST_" + outputFile);
        for (int i = 0; i < 10; i++) {
            stavg += run(structure, inputFile, true);
        }
        stavg /= 10;  // ST average
        cout << "Average elapsed time for single-threaded baseline: " << stavg << " s" << endl;

        
        // run this once then print the output using the new print output function
        float t2avg = 0;
        t2avg += run(structure, inputFile, false);
        writeOutput(outputFile);
        // run un 9 more times and average the results
        for (int i = 0; i < 9; i++) {
            t2avg += run(structure, inputFile, false);  // Based on input (possibly multi-threaded)
        }
        t2avg /= 10;
        cout << "Average elapsed time for " << inputFile << ": " << t2avg << " s" << endl;
        // Compare MT and ST
        CompareST(outputFile, expectedOutputFile);
        float percent = (stavg/t2avg * 100) - 100;
        if (percent < 0)
            percent *= -1;
        cout << "Test was " << percent << "% " << (stavg > t2avg ? "faster" : "slower") << " than the single-threaded version" << endl;
    }
    return 0;
}

// Compares output file with expected output
// File must have accompanying test_expected.txt file generated by FileGenerator.cpp!!
bool CompareST(string arg1, string arg2) {
    ifstream file1(arg1);
    ifstream file2(arg2);
    int count = 1;
    string compare1, compare2;
    while (file1.good()) {
        // skip the first line
        if (count == 1) {
            getline(file1, compare1);
            getline(file2, compare2);
            count++;
            continue;
        }
        file1 >> compare1;
        file2 >> compare2;
        if (compare1 != compare2) {
            cout << "[FAIL]\n" << arg1 << " produced: " << compare1 <<
            "\nBut expected: " << compare2 << "\nAt string " << count << endl;
            return false;
        }
        count++;
    }
    file1.close();
    file2.close();
    cout << "[SUCCESS]\n<" << arg1 << "> matches the single-threaded output." << endl;
    return true;
}

void writeOutput(string fileName) {
    // write to outputFile
    ofstream out;
    out.open(fileName, ios::out);
    for (int i = 0; i < commands.size(); i++) {
        out << commands[i].result << endl;
    }
    out.close();
}

// run the program with a given object inputfile and outputfile
// will read the input file and run the commands with the number of specified threads
// will write the output to the output file
float run(int object, string file, bool singleThreaded) {
    commands.clear();
    // get the commands from the file
    commands = getCommands(file);
    int opsIndex = 0;

    mariosBST->clear(); // Still need to be implemented
    evansMap->clear();

    int mapSize = commands.size() * .30;
    evansMap = new EvansMap(mapSize);

    int maxNumThreads;
    if (!singleThreaded)
        maxNumThreads = commands[opsIndex].key;
    else
        maxNumThreads = 1;

    if (maxNumThreads < 1) {
        maxNumThreads = 1;
    } else if (maxNumThreads > 32) {
        maxNumThreads = 32;
    }
        
    commands[opsIndex].result = "Using " + to_string(maxNumThreads) + " threads";
    commands[opsIndex].object = object;
    opsIndex++;


    // while there are still commands to run
    // create a thread for each command for the max number of threads
    // then join the threads
    // time this
    auto start = std::chrono::high_resolution_clock::now();
    while(opsIndex < commands.size()) {
        int commandsLeft = commands.size() - opsIndex;
        int numThreads = commandsLeft < maxNumThreads ? commandsLeft : maxNumThreads;

        // threads to use
        pthread_t threads[numThreads];

        for (int i = 0; i < numThreads; i++) {
            commands[opsIndex].object = object;
            pthread_create(&threads[i], NULL, &threadFunction, (void *) &commands[opsIndex]);
            opsIndex++;
        }

        for (int i = 0; i < numThreads; i++) {
            pthread_join(threads[i], NULL);
        }

    }
    auto finish = std::chrono::high_resolution_clock::now();

    // get the time
    std::chrono::duration<double> elapsed = finish - start;

    return elapsed.count();
}

// reads in a file with commands to be added to a global queue
// will change to use a buffer to handle files larger than memory
vector<opsStruct> getCommands(string fileName) {
    vector<opsStruct> commands;

    // Use a buffer
    size_t bufferSize = 1024;
    char buffer[bufferSize];
    ifstream file(fileName);
    while (file.good()) {
        file.getline(buffer, bufferSize);
        string line(buffer);
        if (line.length() > 0) {
            opsStruct command;
            
            char c = line[0];
            int space1 = line.find(" ");
            int space2 = line.find(" ", space1 + 1);

            int key;
            string value;

            if (space2 != -1) {
                key = stoi(line.substr(space1 + 1, space2 - space1 - 1));
                string badValue = line.substr(space2 + 1);
                for (int i = 1; i < badValue.length() - 1; i++) {
                    value += badValue[i];
                }
            } else {
                key = stoi(line.substr(space1 + 1));
                value = "";
            }
            opsStruct op = {c, key, value};
            commands.push_back(op);
        }
    }
    file.close();
    return commands;
}

