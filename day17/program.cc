#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;

class Instruction
{
    private:
        bool debugapply;
    public:
        int opcode;
        int operand;

        Instruction(int opcode, int operand, bool debugapply) : opcode(opcode), operand(operand), debugapply(debugapply)
        {
            if (debugapply) { cout << "Made Instruction, opcode: " << opcode << ", operand: " << operand << endl; }
        }
};

class Computer
{
    private:
        // Input
        long regA, regB, regC;
        string program;
        bool debugapply;
        // Internal
        int instrPtr;
        // Map of program instr. (opcode and operand) hashed by their memory index
        // (0 based digit index, ignoring commas)
        //   e.g. 2,3,6,1,6,7
        //            ^--------- 2
        unordered_map<int, Instruction> prog;
        vector<int> output;
    public:
        Computer(long regA, long regB, long regC, string program, bool debugapply) :
            regA(regA),
            regB(regB),
            regC(regC),
            program(program),
            debugapply(debugapply)
        {
            instrPtr = 0;
            ParseProgram();
        }

    private:
        void ParseProgram()
        {
            // Parse program instructions, 2 digits at a time (opcode + operand)
            int pos = 0;
            int ptr = 0;
            int opcode, operand;
            while (pos < program.length())
            {
                opcode = stoi(program.substr(pos,1));
                operand = stoi(program.substr(pos+2,1));
                Instruction i(opcode, operand, debugapply);
                if (debugapply) { cout << "Storing instruction at mem address " << ptr << endl; }
                prog.insert({ptr, i});
                pos += 4;
                ptr += 2; // halved because we care about digits not commas
            }
            cout << "Parsed " << prog.size() << " program instructions\n";
        }
        int ComboOperand(int operand)
        {
            // Special operand used by some operations
            // Interpret the literal operand into the combo one
            if (operand >= 0 && operand <= 3)
            {
                return operand;
            }
            else if (operand == 4)
            {
                return regA;
            }
            else if (operand == 5)
            {
                return regB;
            }
            else if (operand == 6)
            {
                return regC;
            }
            else if (operand == 7)
            {
                throw runtime_error("Combo operand 7 is invalid");
            }
        }
        int RunInstruction(Instruction instr)
        {
            // Instruction Logic Here

            if (debugapply) { cout << "Executing instruction at mem address " << instrPtr << "\n"; }
            // Parse and execute instruction here then update instrPtr

            double num;
            double denom;
            switch (instr.opcode) {
                case 0:
                    // The adv instruction (opcode 0) performs division. The
                    // numerator is the value in the A register. The denominator
                    // is found by raising 2 to the power of the instruction's
                    // combo operand. (So, an operand of 2 would divide A by 4
                    // (2^2); an operand of 5 would divide A by 2^B.) The result
                    // of the division operation is truncated to an integer and
                    // then written to the A register.
                    num = regA;
                    denom = pow(2, ComboOperand(instr.operand));
                    regA = (int) (num / denom);
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 1:
                    // The bxl instruction (opcode 1) calculates the bitwise XOR
                    // of register B and the instruction's literal operand, then
                    // stores the result in register B.
                    regB = instr.operand ^ regB;
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 2:
                    // The bst instruction (opcode 2) calculates the value of its
                    // combo operand modulo 8 (thereby keeping only its lowest 3
                    // bits), then writes that value to the B register.
                    regB = ComboOperand(instr.operand) % 8;
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 3:
                    // The jnz instruction (opcode 3) does nothing if the A
                    // register is 0. However, if the A register is not zero, it
                    // jumps by setting the instruction pointer to the value of
                    // its literal operand; if this instruction jumps, the
                    // instruction pointer is not increased by 2 after this
                    // instruction.
                    if (regA == 0)
                    {
                        // Advance instruction the normal amount
                        instrPtr += 2;
                    }
                    else
                    {
                        // Next instruction to the value of this operand
                        instrPtr = instr.operand;
                    }
                    break;
                case 4:
                    // The bxc instruction (opcode 4) calculates the bitwise XOR
                    // of register B and register C, then stores the result in
                    // register B. (For legacy reasons, this instruction reads an
                    // operand but ignores it.)
                    regB = regB ^ regC;
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 5:
                    // The out instruction (opcode 5) calculates the value of its
                    // combo operand modulo 8, then outputs that value. (If a
                    // program outputs multiple values, they are separated by
                    // commas.)
                    output.push_back(ComboOperand(instr.operand) % 8);
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 6:
                    // The bdv instruction (opcode 6) works exactly like the adv
                    // instruction except that the result is stored in the B
                    // register. (The numerator is still read from the A
                    // register.)
                    num = regA;
                    denom = pow(2, ComboOperand(instr.operand));
                    regB = (int) (num / denom);
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                case 7:
                    // The cdv instruction (opcode 7) works exactly like the adv
                    // instruction except that the result is stored in the C
                    // register. (The numerator is still read from the A
                    // register.)
                    num = regA;
                    denom = pow(2, ComboOperand(instr.operand));
                    regC = (int) (num / denom);
                    // Advance instruction the normal amount
                    instrPtr += 2;
                    break;
                default:
                    break;
            }

            return instrPtr;
        }
        bool RunInstruction()
        {
            // Fetch instruction at instrPtr and execute it
            // If none, then halt and return false
            if (prog.find(instrPtr) != prog.end())
            {
                if (debugapply) { cout << "Found instruction at mem " << instrPtr << endl; }
                instrPtr = RunInstruction(prog.at(instrPtr));
                return true;
            }
            else
            {
                if (debugapply) { cout << "No instruction at mem " << instrPtr << ", program HALT\n"; }
                return false;
            }
        }

    public:
        string Run()
        {
            bool returnCode;
            do
            {
                returnCode = RunInstruction();
            }
            while (returnCode);

            // Concat and return program output as comma delimited string
            string outstring = "";
            for (int i = 0; i < output.size() - 1; i++)
            {
                outstring += to_string(output[i]) + ",";
            }
            outstring += to_string(output.back());
            return outstring;
        }
};

int main(int argc, char* argv[])
{
    cout << "--\nAoC Day 17\n--\n";
    // For testing
    int debuglimit = 5;
    int debug = 0;
    bool debugapply = false;

    // User args
    string filename = "input";
    if (argc == 1) {
        cout << "Assume default input file '" << filename << "'\n";
    }
    else if (argc > 1) {
        filename = argv[1];
        cout << "Taking CLI input file name '" << filename << "'\n";
    }
    if (argc > 2) {
        cout << "Reading 2nd input param\n    -d / --debug for debug printing\n";
        if (string{argv[2]} == "-d" || string{argv[2]} == "--debug") {
            debugapply = true;
        }
        else {
            debugapply = false;
        }
    }
    if (debugapply) {
        cout << "--\nDEBUG MODE : ON\nLINE LIMIT : " << debuglimit << "\n--" << endl;
    }

    // Input file
    ifstream input(filename);

    // Variables for output
    long sum = 0;

    // Read file
    string line;
    long regA, regB, regC;
    string program;
    while (getline(input, line) && (!debugapply || debug < debuglimit))
    {
        debug++;
        if (debugapply) {
            cout << "DEBUG: Ingest line " << debug << ":\n" << line << endl;
        }
        // 3 lines, Register A, B, C, followed by empty line then line of Program commands
        int pos;
        string search = "Register A: ";
        pos = line.find(search);
        if (pos != string::npos)
        {
            regA = stol(line.substr(search.length()));
            continue;
        }
        search = "Register B: ";
        pos = line.find(search);
        if (pos != string::npos)
        {
            regB = stol(line.substr(search.length()));
            continue;
        }
        search = "Register C: ";
        pos = line.find(search);
        if (pos != string::npos)
        {
            regC = stol(line.substr(search.length()));
            continue;
        }
        search = "Program: ";
        pos = line.find(search);
        if (pos != string::npos)
        {
            program = line.substr(search.length());
            continue;
        }
    }
    if (debugapply) {
        cout << "DEBUG: Ingest complete\n--\n";
    }

    // Finished with input file
    input.close();

    // Processing
    cout << "Register A: " << regA << endl;
    cout << "Register B: " << regB << endl;
    cout << "Register C: " << regC << endl;
    cout << "Program: " << program << endl;
    Computer comp(regA, regB, regC, program, debugapply);
    // Run
    string out = comp.Run();

    // Output
    cout << "--\n";
    cout << "Program output = " << out << endl;

    cout << "--\nEnd.\n";
    return 0;
}

