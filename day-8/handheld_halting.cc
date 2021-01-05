#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <tuple>

#define OPERAND_INDEX 4

using namespace std;

int64_t acc = 0;

vector<pair<string, int64_t>> readBootCode() {
    ifstream boot_code("boot_code.txt");
    if (!boot_code.is_open()) {
        throw runtime_error("Could not open boot_code.txt");
    }

    string line;
    vector<pair<string, int64_t>> instructions; // <operation, operand>
    while (getline(boot_code, line)) {
        if (line.length() > OPERAND_INDEX + 1) { // then we can attempt to process it
            string operation = line.substr(0, OPERAND_INDEX - 1);
            int64_t operand = stoi(line.substr(OPERAND_INDEX + 1));
            if (line[OPERAND_INDEX] == '-') {
                operand *= -1;
            }
            instructions.push_back(make_pair(operation, operand));
        }
    }

    return instructions;
}

void processInstructions(vector<pair<string, int64_t>> &instructions) {
    vector<pair<string, int64_t>> oldInstructions = instructions;
    vector<tuple<string, int64_t, int64_t>> processed;
    for (int i = 0; i < instructions.size(); i++) {
        processed.push_back(make_tuple(instructions[i].first, instructions[i].second, 0));
    }

    int64_t pc = 0;
    stack<int64_t> jmpInd, nopInd;
    bool finished = false;
    while (pc < instructions.size()) {
        int64_t &visited_count = get<2>(processed[pc]);
        visited_count++;
        if (visited_count > 1) {
            // program executes indefinitely, retry with appropriate change
            if (!finished) {
                cout << "acc=" << acc << " before instruction visited twice\n";
            }

            finished = true;
            instructions = oldInstructions;
            pc = 0;
            acc = 0;

            if (!jmpInd.empty()) {
                instructions[jmpInd.top()].first = "nop";
                jmpInd.pop();
            } else if (!nopInd.empty()) {
                instructions[nopInd.top()].first = "jmp";
                nopInd.pop();
            }

            for (auto &p : processed) {
                get<2>(p) = 0;
            }
            
            continue;
        }

        if (instructions[pc].first == "nop") {
            if (!finished) nopInd.push(pc);
            pc++;
        } else if (instructions[pc].first == "jmp") {
            if (!finished) jmpInd.push(pc);
            pc += instructions[pc].second;
        } else if (instructions[pc].first == "acc") {
            acc += instructions[pc].second;
            pc++;
        } else {
            throw runtime_error("Unknown instruction");
        }
    }
    
    cout << "acc=" << acc << " after appropriate change\n";
}

int main() {
    vector<pair<string, int64_t>> instructions = readBootCode();
    processInstructions(instructions);
    return 0;
}
