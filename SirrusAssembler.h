#ifndef SIRRUSASSEMBLER_H
#define SIRRUSASSEMBLER_H

#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <stack>
#include <algorithm> 

class SirrusAssembler
{
    private:
    enum CMP_OUT { EQ = 0, GT, LT};
    enum JMP_FLAGS { ERROR = 0, FALSE, TRUE};
    enum INT_FLAGS { ERROR_NAME = 0, ERROR_VALUE, OK, ERROR_NULLPTR, ERROR_BADARRAY};
    enum PUSH_FLAGS {ERROR_PNAME = 0, ERROR_PVALUE, ERROR_OOS,POK};
    enum MACRO_FLAGS {ERROR_MPARAM = 0, ERROR_MNAME, MOK, ERROR_HIGHNUM,ERROR_MEXPRESSION};

    //variables
    std::unordered_map<std::string, int> registers = {
        {"eax",0},
        {"ebx",0},
        {"ecx",0},
        {"edx",0},
    };
    //Macro Parameter Registers
    std::unordered_map<std::string, std::vector<int>> pregisters = {
        {"%1",{}},
        {"%2",{}},
        {"%3",{}},
        {"%4",{}},
    };

    int cmp_reg;

    std::unordered_map<std::string, int> labels;
    std::unordered_map<std::string, std::vector<int>> variables;
    std::vector<int> t_stack; //temporary stack for storing input
    std::vector<int> retpoints;


    struct Macro {
        std::string name;
        int point;
        int parameters;
    };

    std::unordered_map<std::string, Macro> macros;
    
    
//functions

    std::string stringIntDecode(std::vector<int>& encodedValues);
    std::vector<int> stringIntEncode(const std::string & value);

    //expression interpreting
    int evaluateExpression(std::string src);
    int evaluateExpressionIndex(std::string src);
    std::string extractVariableFromExpression(const std::string& expression);

    //macro interpreting
    bool isInMacro = false; 
    int processMacroDefinition(const std::string& line, int & ip);
    int processMacroInvocation(const std::string& line, int & ip);

    std::vector<std::string> split(const std::string &s, char delimiter);
    std::vector<std::string> removeIndents(const std::vector<std::string> &lines);
    void printRegisterValue(const std::string &regName);
    void debugPrintRegisters();
    bool isNumber(const std::string & str);

    int cmd_var     (std::string varName, std::string value);
    bool cmd_mov    (std::string dest, std::string src, std::string line, int & ip);
    bool cmd_add    (std::string src1, std::string src2);
    bool cmd_sub    (std::string src1, std::string src2);
    bool cmd_mul    (std::string src1, std::string src2);
    void cmd_print  (const std::string & src, std::string line, int ip);
    void cmd_input ();
    int  cmd_pop   (std::string src1,std::string line, int & ip);
    int  cmd_push  (std::string dest, std::string src, std::string line, int & ip);

    bool cmd_cmp    (std::string src1,std::string src2);
    bool cmd_jmp    (std::string label, int & ip);
    int cmd_jl     (std::string label, int & ip);
    int cmd_jg     (std::string label, int & ip);
    int cmd_je     (std::string label, int & ip);
    
    bool cmd_ret    (int & ip);
    void cmd_hlt    ();

    public:
    void executeProgram(const std::string &filename);
};

#endif
