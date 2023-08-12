#include "SirrusAssembler.h"
#define LABEL_ERROR std::cerr << "Error: Label not found (line " << ip << ") [" << line << "]\nLabel Expected, Got: " << tokens[1] << "\n";
#define REGISTER_ERROR(x) std::cerr << "Error: Register not found (line " << ip << ") [" << line << "]\nRegister Expected, Got: " << x << "\n";

std::vector<std::string> SirrusAssembler::split(const std::string & s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        // Remove any trailing whitespace characters
        if (!token.empty()) {
            token = token.substr(0, token.find_last_not_of(" \t\r\n") + 1);
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<std::string> SirrusAssembler::removeIndents(const std::vector<std::string>& lines)
{
    std::vector<std::string> result;
    for (const std::string &line : lines) {
        std::string trimmedLine = line;

        // Remove leading whitespace characters
        size_t startPos = trimmedLine.find_first_not_of(" \t");
        if (startPos != std::string::npos) {
            trimmedLine = trimmedLine.substr(startPos);
        }

        result.push_back(trimmedLine);
    }

    return result;
}

void SirrusAssembler::printRegisterValue(const std::string & regName)
{
}

void SirrusAssembler::debugPrintRegisters()
{
      for (const auto& pair : registers) {
        std::cout << pair.first << " : " << pair.second << "\n";
    }
}

int SirrusAssembler::cmd_var(std::string varName, std::string value)
{
   if(!isNumber(value))
    return INT_FLAGS::ERROR_VALUE;

   if(isNumber(varName))
   return INT_FLAGS::ERROR_NAME;

   int varValue = std::stoi(value);
   variables[varName] = varValue;
   return INT_FLAGS::OK;
}

bool SirrusAssembler::isNumber(const std::string & str)
{
    try {
        size_t pos;
        std::stod(str, &pos);
        return (pos == str.size()); 
    } catch (...) {
        return false; 
    }
}

bool SirrusAssembler::cmd_mov(std::string dest, std::string src,std::string line, int & ip)
{
            if (src[0] == '[' && src[src.size() - 1] == ']') {
                src = src.substr(1, src.size() - 2);
                if (variables.find(src) != variables.end()) {
                    registers[dest] = variables[src];
                    return true;
                } else {
                    std::cerr << "Error: Variable not found (line " << ip << ") [" << line << "]\nVariable Expected, Got: " << src << "\n";
                }
            } else   if (dest[0] == '[' && dest[dest.size() - 1] == ']') {
                dest = dest.substr(1, dest.size() - 2);
                if (variables.find(dest) != variables.end()) {
                    variables[dest] = registers[src];
                    return true;
                } else {
                    std::cerr << "Error: Variable not found (line " << ip << ") [" << line << "]\nVariable Expected, Got: " << src << "\n";
                } 
            } else if(registers.find(src) != registers.end())   
            {
                registers[dest] = registers[src];
                return true;
            }
            else 
            {
                if(isNumber(src))
                {
                    registers[dest] = std::stoi(src);
                    return true;
                }
            }
            return false;
}

bool SirrusAssembler::cmd_add(std::string src1, std::string src2)
{
    if(registers.find(src1) != registers.end())
    {
        if(registers.find(src2) != registers.end())
        {
            registers[src1] += registers[src2];
        }
        else if (src2[0] == '[' && src2[src2.size() - 1] == ']') {
            src2 = src2.substr(1, src2.size() - 2);
            registers[src1] += variables[src2];
        }
        else
        {
             registers[src1] += std::stoi(src2);
        }
        return true;
    }
     else
    {
        return false;
    }
}

bool SirrusAssembler::cmd_sub(std::string src1, std::string src2)
{
   if(registers.find(src1) != registers.end())
    {
        if(registers.find(src2) != registers.end())
        {
            registers[src1] -= registers[src2];
        }
        else if (src2[0] == '[' && src2[src2.size() - 1] == ']') {
            src2 = src2.substr(1, src2.size() - 2);
            registers[src1] -= variables[src2];
        }
        else
        {
             registers[src1] -= std::stoi(src2);
        }
        return true;
    }
     else
    {
        return false;
    }
}

void SirrusAssembler::cmd_print(const std::string & src, std::string line, int ip)
{
    if (registers.find(src) != registers.end()) 
        std::cout << registers[src] << "\n";
    else 
        REGISTER_ERROR(src); 
}

bool SirrusAssembler::cmd_mul(std::string src1, std::string src2)
{
   if(registers.find(src1) != registers.end())
    {
        if(registers.find(src2) != registers.end())
        {
            registers[src1] *= registers[src2];
        }
        else if (src2[0] == '[' && src2[src2.size() - 1] == ']') {
            src2 = src2.substr(1, src2.size() - 2);
            registers[src1] *= variables[src2];
        }
        else
        {
             registers[src1] *= std::stoi(src2);
        }
        return true;
    }
     else
    {
        return false;
    }
}

int SirrusAssembler::cmd_inc(std::string src)
{
    return registers[src] + 1;
}

int SirrusAssembler::cmd_dec(std::string src)
{
    return registers[src] - 1;
}

bool SirrusAssembler::cmd_cmp(std::string src1, std::string src2)
{
    if (registers.find(src2) != registers.end())
    {
        if  (registers[src1] < registers[src2])
        {
            cmp_reg = CMP_OUT::LT;
            return true;
        } 
        else if (registers[src1] > registers[src2])  
        {
            cmp_reg = CMP_OUT::GT;
            return true;
        }
        else if (registers[src1] == registers[src2]) 
        {
            cmp_reg = CMP_OUT::EQ;
            return true;
        }
        return false;
    }
    return false;

    std::cout << cmp_reg << std::endl;
}

bool SirrusAssembler::cmd_jmp(std::string label,int & ip)
{
    if (labels.find(label) != labels.end()) {
        retpoints.push_back(ip + 1);
        ip = labels[label];  // Jump to the specified label
        return true;
    }
    return false;
}

int SirrusAssembler::cmd_je(std::string label, int & ip)
{
    if (labels.find(label) == labels.end()) 
    return JMP_FLAGS::ERROR;

    if (cmp_reg == CMP_OUT::EQ) 
    {
        retpoints.push_back(ip + 1);
        ip = labels[label];  // Jump to the specified label
        return JMP_FLAGS::TRUE;
    }
    return JMP_FLAGS::FALSE;
}

int SirrusAssembler::cmd_jg(std::string label, int & ip)
{
    if (labels.find(label) == labels.end()) 
    return JMP_FLAGS::ERROR;

    if (cmp_reg == CMP_OUT::GT) 
    {
        retpoints.push_back(ip + 1);
        ip = labels[label];  // Jump to the specified label
        return JMP_FLAGS::TRUE;
    }
    return JMP_FLAGS::FALSE;
}

int SirrusAssembler::cmd_jl(std::string label, int & ip)
{
    if (labels.find(label) == labels.end()) 
    return JMP_FLAGS::ERROR;

    if (cmp_reg == CMP_OUT::LT) 
    {
        retpoints.push_back(ip + 1);
        ip = labels[label];  // Jump to the specified label
        return JMP_FLAGS::TRUE;
    }
    return JMP_FLAGS::FALSE;
}

bool SirrusAssembler::cmd_ret(int & ip)
{
     if (!retpoints.empty()) {
                ip = retpoints.back();
                retpoints.pop_back();
                return true;
     }
     return false;
}

void SirrusAssembler::cmd_hlt()
{
    return;  // Exit the program
}

void SirrusAssembler::executeProgram(const std::string & filename)
{
       std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return;
    }
    

    std::vector<std::string> program;
    std::string line;


    while (std::getline(inputFile, line)) {
        // Remove comments (lines after the semicolon)
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }

        // Skip empty lines or lines with only whitespace
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        program.push_back(line);
    }

    program = removeIndents(program);

    // Populate the labels map
    for (int i = 0; i < program.size(); ++i) {
        std::string line = program[i];
        std::vector<std::string> tokens = split(line, ' ');
        if (!tokens.empty() && tokens[0].back() == ':') {
            std::string label = tokens[0].substr(0, tokens[0].size() - 1);
            labels[label] = i;
        }
    }

    int ip = 0;  // Instruction pointer

    std::string mainLabel = "_main";
    if (labels.find(mainLabel) == labels.end()) {
        std::cerr << "Error: Main function does not exist!\n";
        return;
    }

    ip = labels[mainLabel];

    while (ip < program.size()) {
        std::string line = program[ip];
        std::vector<std::string> tokens = split(line, ' ');

        

        if (tokens.empty() || tokens[0].back() == ':') {
            ip++;
            continue;
        }

        std::string cmd = tokens[0];

        if (cmd == "cmp")
        {
            if (!cmd_cmp(tokens[1],tokens[2]))
            {
                std::cerr << "Error: Invalid instruction (line " << ip << ")\"" << line << "\"\n";
                return;
            }
        }
        if (cmd == "print")         cmd_print(tokens[1], line, ip);
        if (cmd == "var")
        {
            switch(cmd_var(tokens[1],tokens[2]))
            {
                case INT_FLAGS::ERROR_NAME:
                    std::cerr << "Error: Tried to initialize a Variable with a Numerical Label (line " << ip << ") [" << line << "]\nLabel Expected, Got: " << tokens[1] << "\n";
                    return;
                    break;
                case INT_FLAGS::ERROR_VALUE:
                    std::cerr << "Error: Tried to initialize a Variable with a string value (line " << ip << ") [" << line << "]\nNumber Expected, Got: " << tokens[2] << "\n";
                    return;
                    break;
                default:
                break;
            }
        }
        else if (cmd == "mov")
        {
            if(!cmd_mov(tokens[1],tokens[2],line, ip)) return;
        }
        else if (cmd == "add")
        {
            if(!cmd_add(tokens[1], tokens[2]))
            {
                REGISTER_ERROR(tokens[1])
                return;
            }
        }
        else if (cmd == "sub")     
        {
            if(!cmd_sub(tokens[1], tokens[2]))
            {
                REGISTER_ERROR(tokens[1])
                return;
            }
        }
        else if (cmd == "mul")     
        {
            if(!cmd_mul(tokens[1], tokens[2]))
            {
                REGISTER_ERROR(tokens[1])
                return;
            }
        }
        else if (cmd == "jmp")
        {
            if  (cmd_jmp(tokens[1],ip)) continue;
            else
            {
                LABEL_ERROR
                return;
            }
        }
        else if (cmd == "je")
        {
            auto res = (cmd_je(tokens[1],ip));
            if(res == JMP_FLAGS::TRUE) continue;
            else if(res == JMP_FLAGS::ERROR)
            {
                LABEL_ERROR
                return;
            }
        }
        else if (cmd == "jg")
        {
            auto res = (cmd_jg(tokens[1],ip));
            if(res == JMP_FLAGS::TRUE) continue;
            else if(res == JMP_FLAGS::ERROR) 
            {
                LABEL_ERROR
                return;
            }
        }
        else if (cmd == "jl")
        {
            auto res = (cmd_jl(tokens[1],ip));
            if(res == JMP_FLAGS::TRUE) continue;
            else if(res == JMP_FLAGS::ERROR)
            {
                LABEL_ERROR
                return;
            }
        }
        else if (cmd == "ret")      if(cmd_ret(ip)) continue;
        else if (cmd == "inc")
        {
            registers[tokens[1]] = registers[tokens[1]] + 1;
        }
        else if (cmd == "dec")
        {
            registers[tokens[1]] = registers[tokens[1]] - 1;
        }
        else if (cmd == "hlt")      cmd_hlt();
        else return;
        ip++;  // Move to the next instruction
    }

    //debugPrintRegisters();
}
