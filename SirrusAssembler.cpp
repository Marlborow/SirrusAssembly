#include "SirrusAssembler.h"
#define LABEL_ERROR \
    std::cerr << "Error: Label not found (line " << ip << ") \
    [" << line << "]\nLabel Expected, Got: " << tokens[1] << "\n"; 
#define REGISTER_ERROR(x) \
    std::cerr << "Error: Register not found (line " << ip << ") \
    [" << line << "]\nRegister Expected, Got: " << x << "\n";
#define VARIABLE_ERROR(x) \
    std::cerr << "Error: Variable not found (line " << ip << ") \
    [" << line << "]\nVariable Expected, Got: " << x << "\n";



int SirrusAssembler::evaluateExpression(std::string src)
{
    size_t plusPos = src.find('+');
    std::string token = src.substr(0,plusPos);;
    
    if (plusPos == std::string::npos) {
        return variables[token][0];
    } else {
        std::string remainder = src.substr(plusPos + 1);
        if(registers.find(remainder) != registers.end())
        {
            return variables[token][registers[remainder]];
        }
        else if(variables.find(remainder) != variables.end())
        {
            return variables[token][variables[remainder][0]];
        }
        else if(isNumber(remainder))
        {
            return variables[token][std::stoi(remainder)];
        }
    }
}

int SirrusAssembler::evaluateExpressionIndex(std::string src)
{
    size_t plusPos = src.find('+');
    std::string token = src.substr(0,plusPos);
    
    if (plusPos == std::string::npos) {
        return 0;
    } else {
        std::string remainder = src.substr(plusPos + 1);
        if(registers.find(remainder) != registers.end())
        {
            return registers[remainder];
        }
        else if(variables.find(remainder) != variables.end())
        {
            return variables[remainder][0];
        }
        else if(isNumber(remainder))
        {
            return std::stoi(remainder);
        }
    }
}



std::string SirrusAssembler::extractVariableFromExpression(const std::string & expression)
{
    std::string result;
    size_t pos = expression.find('+');
    
    if (pos != std::string::npos) {
        result = expression.substr(0, pos);
    } else {
        result = expression;
    }
    
    return result;
}

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
    if (value.size() >= 4 && value.substr(0, 2) == "~[" && value[value.size() - 1] == ']') {
        std::string referencedName = value.substr(2, value.size() - 3);
        if (variables.find(referencedName) != variables.end()) {
            variables[varName].push_back(variables[referencedName].size());
            for (int val : variables[referencedName]) {
                variables[varName].push_back(val);
            }
            return INT_FLAGS::OK;
        } else {
            return INT_FLAGS::ERROR_VALUE;
        }
    }

    if (value.size() >= 6 && value.substr(1, 3) == "str") {
        value = value.substr(6, value.size() -7);
        std::string tempVal = value;
        int64_t encodedValue = 0;
        for (size_t i = 0; i < tempVal.size(); i++) {
            if (i + 2 < tempVal.size() && tempVal[i] == '^' && std::isdigit(tempVal[i + 1]) && std::isdigit(tempVal[i + 2])) {
                variables[varName].push_back(encodedValue * 256 + std::stoi(value.substr(i + 1, 2)));
                i += 2;
            } else {
                variables[varName].push_back(encodedValue * 256 + static_cast<int>(value[i]));
            }
            #ifdef DEBUG    
                std::cout << "\033[31m> DEBUG: Variable: \"" << varName << "\"  PUSH VALUE " << variables[varName].back() << "| Size: " << variables[varName].size() << "\033[0m\n";
            #endif
        }
        return INT_FLAGS::OK;
    }

    if (!isNumber(value))
        return INT_FLAGS::ERROR_VALUE;

    if (isNumber(varName))
        return INT_FLAGS::ERROR_NAME;

    int varValue = std::stoi(value);
    variables[varName].push_back(varValue);
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
                if (variables.find(extractVariableFromExpression(src)) != variables.end()) {
                    registers[dest] = evaluateExpression(src);
                    return true;
                } else {
                    VARIABLE_ERROR(src);
                    return false;
                }
            } else   if (dest[0] == '[' && dest[dest.size() - 1] == ']') {
                dest = dest.substr(1, dest.size() - 2);
                if (variables.find(extractVariableFromExpression(dest)) != variables.end()) {
                    variables[extractVariableFromExpression(dest)][evaluateExpressionIndex(dest)] = registers[src];
                    return true;
                } else {
                    VARIABLE_ERROR(src);
                    return false;
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
            std::cerr << "Error: Invalid instruction (line " << ip << ")\"" << line << "\"\n";
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
            if(variables[src2].size() < 2)
                registers[src1] += evaluateExpression(src2);
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
            registers[src1] -= evaluateExpression(src2);
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
    if(src == "newline"){
        std::cout << "\n";
    }
    else
    {
        if (src.size() >= 6 && src.substr(1, 3) == "str") {
            std::string regName = src.substr(5, src.size());
            if (registers.find(regName) != registers.end()) {
                int64_t value = registers[regName];
                std::string output;
                while (value > 0) {
                    output = static_cast<char>(value % 256) + output;
                    value /= 256;
                }
                std::cout << output;
            } 
            else
            {
                REGISTER_ERROR(regName);
            }
        }
        else if (registers.find(src) != registers.end()) 
        {
                std::cout << registers[src];
        }
        else REGISTER_ERROR(src);
    }

    /*
    if (registers.find(src) != registers.end()) 
        std::cout << registers[src] << "\n";
    else 
        REGISTER_ERROR(src);
    */ 
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
            registers[src1] *= evaluateExpression(src2);
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
    else
    {
        if(isNumber(src2))
        {
            if  (registers[src1] < std::stoi(src2))
            {
                cmp_reg = CMP_OUT::LT;
                return true;
            } 
            else if (registers[src1] > std::stoi(src2))  
            {
                cmp_reg = CMP_OUT::GT;
                return true;
            }
            else if (registers[src1] == std::stoi(src2)) 
            {
                cmp_reg = CMP_OUT::EQ;
                return true;
            }
        }
        return false;
    }
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
        //std::cout << line << std::endl;
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
        if (cmd == "print")
        {
            cmd_print(tokens[1],line, ip);
        } 
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
