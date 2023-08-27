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
#define NULLPTR_ERROR(x) \
    std::cerr << "Error: Variable you are trying to reference is null (line " << ip << ") \
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

std::vector<std::string> SirrusAssembler::split(const std::string & s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) 
    {
        // Remove any trailing whitespace characters
        if (!token.empty()) 
        {
            token = token.substr(0, token.find_last_not_of(" \t\r\n") + 1);
            tokens.push_back(token);
        }
    }
    return tokens;
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

int SirrusAssembler::processMacroDefinition(const std::string & line,int & ip)
{
    std::vector<std::string> tokens = split(line, ' ');
    if (!tokens.empty() && tokens[0].front() == '%')
    {
        if (tokens[0] == "%macro") {
            Macro macro;
            macro.name = tokens[1];
            macro.point = ip;
            //check if the macro name is a register or a label (and stop the program if so)
            if (isNumber(macro.name) || registers.find(macro.name) != registers.end() || labels.find(macro.name) != labels.end())
            {
                std::cerr << "Error: Invalid Macro definition: Incorrect Label  (line " << ip << ")\"" << line << "\"\n";
                return MACRO_FLAGS::ERROR_MNAME;
            }

            if (tokens.size() > 2) 
            {
                //check if macro paramiters is a number and then append the value of the given paramiters to the current macro.
                if(isNumber(tokens[2]))
                {
                    if(std::stoi(tokens[2]) > 4)
                    {
                        std::cerr << "Error: Macro definition Limit Exeeded: " << tokens[2] << "/4 (line " << ip << ")\"" << line << "\"\n";
                        return MACRO_FLAGS::ERROR_HIGHNUM;
                    }
                    macro.parameters = std::stoi(tokens[2]);
                }
                else 
                {
                    std::cerr << "Error: Invalid Macro definition: Number Expected (line " << ip << ")\"" << line << "\"\n";
                    return MACRO_FLAGS::ERROR_MPARAM;
                }
            }
            macros[macro.name] = macro;
            return MACRO_FLAGS::MOK;
        }
        else
        {
            return MACRO_FLAGS::MOK;
        }
    }
    return MACRO_FLAGS::MOK;

}

int SirrusAssembler::processMacroInvocation(const std::string & line, int & ip)
{
    std::vector<std::string> tokens = split(line, ' ');
    std::string label = tokens[0];
    if(tokens.size() > 1)
    {
        std::string expression = tokens[1];
        // Removing square brackets and spaces
        //first check if the expression is incorrect format
        if(expression[0] != '[' ||  expression[expression.size() -1] != ']')
        {
            std::cerr << "Error: Invalid Invocation of Macro: Expression Error \"" << line << "\"\n";
            return MACRO_FLAGS::ERROR_MEXPRESSION;
        }
        
        expression = expression .substr(1, expression .size() - 2);
        std::istringstream iss(expression );

        std::string value;
        int i = 0;
        while (std::getline(iss, value, ',')) 
        {
            i++;
            if(i > macros[tokens[0]].parameters) 
            {
                std::cerr << "Error: Macro \"" << tokens[0] << "\" "<< macros[tokens[0]].parameters <<" Param Limit Exeeded " << "(line " << ip << ")\"" << line << "\"\n";
                return MACRO_FLAGS::ERROR_HIGHNUM;
            }

            std::string key = "%" + std::to_string(i);

            //Check if value is a variable
            if(variables.find(value) != variables.end())
            {
                pregisters[key] = variables[value];
            }

            if(registers.find(value) != registers.end())
            {
                pregisters[key][0] = registers[value];
            }
        }
    }

    //create retpoint
    //and move instruction pointer
    retpoints.push_back(ip + 1);
    ip = (macros[tokens[0]].point);
    return MACRO_FLAGS::MOK;
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
    std::cout <<"\n";
    std::cout << "REGISTERS:" << std::endl;
    for (const auto& pair : registers) 
    {
        std::cout << " >" << pair.first << " : " << pair.second << "\n";
    }

    std::cout << "VARIABLES:" << std::endl;
    for (const auto& pair : variables) 
    {
        std::cout << " >" << pair.first << " : " << pair.second.size() << "\n";
    }
    std::cout <<"\n";
    std::cout <<"\n";

}

int SirrusAssembler::cmd_var(std::string varName, std::string value)
{
    bool isArrayFormat = true;
    for (char c : value) 
    {
        if (!std::isdigit(c) && c != ',') 
        {
            isArrayFormat = false;
            break;
        }
    }

    if (isArrayFormat) 
    {
        std::vector<int> values;
        std::stringstream ss(value);
        std::string item;
        
        while (getline(ss, item, ',')) 
        {
            values.push_back(std::stoi(item));
        }
        
        // Assign the 'values' vector to the 'varName' in the 'variables' map
        variables[varName] = values;
        
        return INT_FLAGS::OK;
    }
    else
    {

        bool isBrokenArrayFormat = false;
        for (char c : value) {
            if (c == ',') {
                isBrokenArrayFormat = true;
                break;
            }
        }

        if(isBrokenArrayFormat)
            return INT_FLAGS::ERROR_BADARRAY;


        if (value.size() >= 4 && value.substr(0, 2) == "~[" && value[value.size() - 1] == ']') {
            std::string referencedName = value.substr(2, value.size() - 3);
            if (variables.find(referencedName) != variables.end()) {
                
                //if referenced Variable is empty/null error handle!
                if(variables[referencedName].empty())
                    return INT_FLAGS::ERROR_NULLPTR;


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

        if(value == "nullptr") 
        {
            variables[varName] = std::vector<int>();
            return INT_FLAGS::OK;
        }

        if (!isNumber(value))
        {
            return INT_FLAGS::ERROR_VALUE;
        }

        if (isNumber(varName))
            return INT_FLAGS::ERROR_NAME;

        int varValue = std::stoi(value);
        variables[varName].push_back(varValue);
        return INT_FLAGS::OK;
    }
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
                if(isInMacro)
                {
                    if(pregisters.find(extractVariableFromExpression(src)) != pregisters.end())
                    {
                        if (pregisters[extractVariableFromExpression(src)].empty())
                        {
                            registers[dest] = -1;
                            return true;
                        }
                        registers[dest] = pregisters[extractVariableFromExpression(src)][evaluateExpressionIndex(src)];
                        return true;
                    }
                }

                if (variables.find(extractVariableFromExpression(src)) != variables.end()) {

                    //check if variable is null and if it is then return -1
                    if (variables[extractVariableFromExpression(src)].empty())
                    {
                        registers[dest] = -1;
                        return true;
                    }

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

void SirrusAssembler::cmd_input()
{
    t_stack.clear();
    std::string tempstr;
    std::getline(std::cin, tempstr);
    std::reverse(tempstr.begin(), tempstr.end());

    int64_t encodedValue = 0;
    for (size_t i = 0; i < tempstr.size(); i++) 
    {
        t_stack.push_back(encodedValue * 256 + static_cast<int>(tempstr[i]));
    }

    t_stack.push_back(tempstr.size());
}

int SirrusAssembler::cmd_pop(std::string src1, std::string line, int & ip)
{
    if(registers.find(src1) != registers.end())
    {
        int res = t_stack.back();
        t_stack.pop_back();
        registers[src1] = res;
        return true;
    }
    REGISTER_ERROR(src1)
    return false;
}

int SirrusAssembler::cmd_push(std::string dest, std::string src, std::string line, int & ip)
{   
    if (dest[0] == '[' && dest[dest.size() - 1] == ']') 
    {
        dest = dest.substr(1, dest.size() - 2);
        if (variables.find(extractVariableFromExpression(dest)) != variables.end()) 
        {
            //continue to check if src is number or register
            if(isNumber(src))
            {
                variables[extractVariableFromExpression(dest)].push_back(std::stoi(src));
                return PUSH_FLAGS::POK;
            }

            if(registers.find(src) != registers.end())
            {
                variables[extractVariableFromExpression(dest)].push_back(registers[src]);
                return PUSH_FLAGS::POK;
            }
        } else 
        {
            VARIABLE_ERROR(dest);
            return PUSH_FLAGS::ERROR_PNAME;
        }
    } 
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
    if (isNumber(label)) {
        ip += std::stoi(label);
        return true;
    } 
    else if (labels.find(label) != labels.end()) 
    {

        if(!isInMacro)
            retpoints.push_back(ip + 1);

        ip = labels[label];  // Jump to the specified label
        return true;
    }
    return false;
}

int SirrusAssembler::cmd_je(std::string label, int & ip)
{

    if (cmp_reg == CMP_OUT::EQ) 
    {
        if (isNumber(label)) {
            ip += std::stoi(label);
            return JMP_FLAGS::TRUE;
        }

        if (labels.find(label) == labels.end()) 
        return JMP_FLAGS::ERROR;

        if(!isInMacro)
            retpoints.push_back(ip + 1);

        ip = labels[label];  // Jump to the specified label
        return JMP_FLAGS::TRUE;
    }
    return JMP_FLAGS::FALSE;
}

int SirrusAssembler::cmd_jg(std::string label, int & ip)
{

    if (cmp_reg == CMP_OUT::GT) 
    {
         if (isNumber(label)) {
            ip += std::stoi(label);
            return JMP_FLAGS::TRUE;
        }

        if (labels.find(label) == labels.end()) 
        return JMP_FLAGS::ERROR;

        if(!isInMacro)
            retpoints.push_back(ip + 1);
        
        ip = labels[label];  // Jump to the specified label
        return JMP_FLAGS::TRUE;
    }
    return JMP_FLAGS::FALSE;
}

int SirrusAssembler::cmd_jl(std::string label, int & ip)
{
    if (cmp_reg == CMP_OUT::LT) 
    {
        if (isNumber(label)) {
            ip += std::stoi(label);
            return JMP_FLAGS::TRUE;
        }

        if (labels.find(label) == labels.end()) 
        return JMP_FLAGS::ERROR;

        if(!isInMacro)
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
    bool quitProgram = false;
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) 
    {
        std::cerr << "Error: Unable to open file " << filename << "\n";
        return;
    }
    

    std::vector<std::string> program;
    std::string line;


    while (std::getline(inputFile, line)) 
    {
        // Remove comments (lines after the semicolon)
        size_t commentPos = line.find(';');
        if (commentPos != std::string::npos) 
        {
            line = line.substr(0, commentPos);
        }

        // Skip empty lines or lines with only whitespace
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) 
        {
            continue;
        }

        program.push_back(line);
    }

    program = removeIndents(program);

    // Populate the labels map and Macro's
    for (int i = 0; i < program.size(); ++i)
    {
        std::string line = program[i];
        std::vector<std::string> tokens = split(line, ' ');
        if (!tokens.empty() && tokens[0].back() == ':')
        {
            std::string label = tokens[0].substr(0, tokens[0].size() - 1);
            labels[label] = i;
        }
        if(processMacroDefinition(line,i) != MACRO_FLAGS::MOK)
        {
            std::cout << "error!" << std::endl;
            quitProgram = true;
        }
    }

    if(quitProgram) return;

    int ip = 0;  // Instruction pointer

    std::string mainLabel = "_main";
    if (labels.find(mainLabel) == labels.end()) {
        std::cerr << "Error: Main function does not exist!\n";
        return;
    }

    ip = labels[mainLabel];

    while (ip < program.size()) 
    {
        std::string line = program[ip];
        std::vector<std::string> tokens = split(line, ' ');

        //std::cout << line << std::endl;

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

        if(cmd == "%endmacro")
        {
            if(cmd_ret(ip)) {
                isInMacro = false;
                continue;
            }
        }
 
        if (cmd == "input")
        {
            cmd_input();
        }
        if (cmd == "push")
        {
           switch(cmd_push(tokens[1], tokens[2], line, ip))
           {
                case PUSH_FLAGS::ERROR_PNAME:
                    return;
                    break;
                case PUSH_FLAGS::ERROR_PVALUE:
                    return;
                    break;
                case PUSH_FLAGS::ERROR_OOS:
                    return;
                    break;
           }
        }
        if (cmd == "pop")
        {
            cmd_pop(tokens[1],line,ip);
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
                    std::cerr << "Error: Tried to initialize a Variable with a String Value (line " << ip << ") [" << line << "]\nNumber Expected, Got: " << tokens[2] << "\n";
                    return;
                    break;
                case INT_FLAGS::ERROR_NULLPTR:
                    NULLPTR_ERROR(tokens[2])
                    return;
                    break;
                case INT_FLAGS::ERROR_BADARRAY:
                    std::cerr << "Error: Tried to initialize an Array Variable with none-Numerical Values (line " << ip << ") [" << line << "]\nNumbers Expected, Got: " << tokens[2] << "\n";
                    return;
                    break;
                default:
                break;
            }
        }

        if(macros.find(cmd) != macros.end())
        {
            isInMacro = true;
            processMacroInvocation(line,ip);
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
}
