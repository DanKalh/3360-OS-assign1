//**Please use all 3 grace days, plus a couple days late. Sorry 
//Dan Kalhori 2153552
//COSC3360 Programming Assignment 1 - Pipes and Process Synchronization

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// Helper function to split a string by a delimiter and return a vector of tokens
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

char isOp(char c) {
    return (c == '+' || c == '-' || c == '/' || c=='*') ? c : -1;
}

int matchesVar(string test, vector<string> vars) {
    for (size_t i = 0; i < vars.size(); i++) {
        if(test == vars[i]) {
            return i;
        }
    }    
    return -1;
}

void writePipe(int *pipefds, int valueToSend) {
    // close 0
    close(pipefds[0]);
    // write to 1
    write(pipefds[1], &valueToSend, sizeof(valueToSend));
}

int readPipe(int *pipefds) {
    // close 1
    close(pipefds[1]);
    // read from 0
    int readValue;
    read(pipefds[0], &readValue, sizeof(readValue));
    return readValue;
}

int main(int argc, char* argv[]) {

    ifstream process1(argv[1]);
    ifstream input1(argv[2]);

    string line;
    vector<int> initialValues(10, 0); // Vector to store variable values, initialized to 0
    int variableLength=0;

    // make a bunch of pipes
    int pipes[20][2];

    // Read and store initial variable values from the values file "input1"
    if (getline(input1, line)) {
        vector<string> values = split(line, ',');
        for (size_t i = 0; i < values.size() && i < initialValues.size(); i++) {
            initialValues[i] = stoi(values[i]);
            variableLength++;
        }
    }
    vector<int> pValues(10, 0); // values of p variables
    
    vector<pair<string, pair<int, int>>> operations;

    // process vars
    getline(process1, line);
    line = line.substr(10);
    line = line.substr(0, line.length() - 2);
    vector<string> varNames = split(line, ',');
    cout << "variables: ";
    for (size_t i = 0; i < varNames.size(); i++) {
        cout << varNames[i] << "=" << initialValues[i] << ' ';
    }
    cout << endl;

    // process internal vars
    getline(process1, line);
    line = line.substr(13);
    line = line.substr(0, line.length() - 2);
    vector<string> pVars = split(line, ',');
    cout << "p variables: ";
    for (size_t i = 0; i < pVars.size(); i++) {
        cout << pVars[i] << ' ';
    }
    cout << endl;

    int pipeIndex = 0;
    while (getline(process1, line)) {
        pipe(pipes[pipeIndex]);
        operations.push_back({line, {pipes[pipeIndex][0], pipes[pipeIndex][1]}});
        pipeIndex++;
    }

    // create one process for each pvar
    string identity = "parent"; // parent
    for (int i = 0; i < pVars.size(); i++) {
        pid_t pid = fork();
        if(pid == 0) {
            // child process
            identity = pVars[i];
            // cout << "process " << identity << " starting" << endl;
            break;
        }
    }

    int pipeNumber = 0;
    int myValue = 0;
    for (auto& op : operations) {
        auto line = op.first;
        line = line.substr(0, line.length() - 2);
        if (identity == "parent") {
            // if line starts with alpha or operand, send the value down the pipe
            auto isop = isOp(line[0]);
            // cout << isop << endl;
            if (isop != -1) {
                line = line.substr(2);
            }
            // alpha
            string start(1, line[0]);

            if (line[0] != 'p') {
                // cout << "parent processing: " << line << endl;
                int varIndex = matchesVar(start, varNames);
                if (varIndex != -1) {
                    // it's a var
                    line = line.substr(5);
                    // line is now pvar
                    int pVarIndex = matchesVar(line, pVars);
                    int valueToSend = initialValues[varIndex];
                    // cout << identity << " writing " << valueToSend << " to pipe " << pipeNumber << endl;
                    writePipe(pipes[pipeNumber], valueToSend);
                }
            }
        }
        else {
            // child
            // cout << identity << " processing: " << line << endl;
            auto op = isOp(line[0]);
            if (op != -1) {
                line = line.substr(2);
            }

            string lastTwoChars = line.substr(line.length() - 2);
            string firstTwoChars = line.substr(0, 2);
            if (identity == lastTwoChars) {
                // if we're on the right side of the line, we read a value from the pipe
                // and optionally do a math op
                int newValue = readPipe(pipes[pipeNumber]);
                // cout << identity << " read " << newValue << " from pipe " << pipeNumber << endl;
                if (op != -1) {
                    switch(op) {
                        case '+':
                            myValue += newValue;
                            break;
                        case '-':
                            myValue -= newValue;
                            break;
                        case '*':
                            myValue *= newValue;
                            break;
                        case '/':
                            myValue /= newValue;
                            break;
                    }
                    // cout << identity << " did op " << op << ", new value " << myValue << endl;
                }
                else {
                    myValue = newValue;
                }
            }
            else if(identity == firstTwoChars) {
                // if we're on the left side, write our value to the pipe
                writePipe(pipes[pipeNumber], myValue);
                // cout << identity << " wrote " << myValue << " to pipe " << pipeNumber << endl;
            }
            
        }

        pipeNumber++;
    }

    if(identity != "parent") {
        cout << identity << " final value " << myValue << endl;
        exit(0);
    }

    // // Wait for child processes to complete
    for (size_t i = 0; i < operations.size(); i++) {
        wait(NULL);
    }

    // // Close all pipe ends in parent
    for (const auto& op : operations) {
        close(op.second.first);
        close(op.second.second);
    }

    process1.close();
    input1.close();
    return 0;
}