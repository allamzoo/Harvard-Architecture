#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define INSTR_MEM_SIZE 1024
#define DATA_MEM_SIZE 2048
#define NUM_REGISTERS 64

unsigned char SREG;
#define FLAG_Z 0x01  
#define FLAG_S 0x02  
#define FLAG_N 0x04  
#define FLAG_V 0x08  
#define FLAG_C 0x10  


enum {
    ADD,
    SUB,
    MUL,
    LDI,
    BEQZ,
    AND,
    OR,
    JR,
    SLC,
    SRC,
    LB,
    SB
};
bool branchEncountered = false;
bool jumpEncountered = false;
uint16_t instructionMemory[INSTR_MEM_SIZE];
uint8_t dataMemory[DATA_MEM_SIZE];
uint8_t registers[NUM_REGISTERS];
uint8_t initialRegisters[NUM_REGISTERS];
uint16_t PC = 0;
uint8_t SREG = 0;
void printBinary(uint8_t n);
void printState(int cycle);
void fetch(uint16_t *instruction);
void decode(uint16_t instruction, uint8_t *opcode, uint8_t *operands);
void execute(uint8_t opcode, uint8_t *operands);
void updateFlags(uint8_t opcode, uint8_t operand1, uint8_t operand2, uint8_t result);
void loadInstructionsFromFile(const char *filename);
uint8_t getOpcode(const char *mnemonic);
uint16_t encodeInstruction(uint8_t opcode, uint8_t operand1, uint8_t operand2);

 int main() {
    loadInstructionsFromFile("instructions4.txt");
    memset(registers, 0, NUM_REGISTERS * sizeof(uint8_t));
    memcpy(initialRegisters, registers, NUM_REGISTERS * sizeof(uint8_t));

    uint16_t instructionBuffer[3] = {0}; 
    uint8_t opcodeBuffer[3] = {0}; 
    uint8_t operandsBuffer[3][2] = {{0}}; 

    int numLoadedInstructions = 0;
    while (numLoadedInstructions < INSTR_MEM_SIZE && instructionMemory[numLoadedInstructions] != 0) {
        numLoadedInstructions++;
    }

    int instructionIndex = 0;
    numLoadedInstructions= numLoadedInstructions+2;
            int c = 1;

    for (int i = 0; i < numLoadedInstructions; i++) {
        printf("Clock Cycle %d:\n", c);

        printState(c);
                c++;

        
        

        if (i <= numLoadedInstructions) {
            instructionBuffer[0] = instructionMemory[PC]; 
            printf("Fetch: Instruction at PC=%d: 0x%04X\n", PC, instructionBuffer[0]);
            PC++; 
        }
            if (i >= 1 && i<= numLoadedInstructions + 1) {
        decode(instructionBuffer[1], &opcodeBuffer[1], operandsBuffer[1]);
        printf("Decode: Instruction: 0x%04X, Opcode: %d, Operands: %d, %d\n", instructionBuffer[1], opcodeBuffer[1], operandsBuffer[1][0], operandsBuffer[1][1]);
    }
        if (i >= 2 && i <= numLoadedInstructions + 2) {
            printf("Execute: Opcode: %d, Operands: %d, %d\n", opcodeBuffer[2], operandsBuffer[2][0], operandsBuffer[2][1]);
            execute(opcodeBuffer[2], operandsBuffer[2]);
            if (branchEncountered || jumpEncountered) {
            printf("Branch or Jump Hazard Detected. Flushing the pipeline.\n");
            memset(instructionBuffer, 0, sizeof(instructionBuffer));
            if (branchEncountered) { 
                i=PC-1;
    }   else if (jumpEncountered) {
                PC = (registers[operandsBuffer[2][0]] << 8) | registers[operandsBuffer[2][1]];
                i=PC-1;

    }

    branchEncountered = false;
    jumpEncountered = false;
}
            
        }

        if (i <= numLoadedInstructions) {
            instructionBuffer[2] = instructionBuffer[1];
            instructionBuffer[1] = instructionBuffer[0];
            opcodeBuffer[2] = opcodeBuffer[1];
            opcodeBuffer[1] = opcodeBuffer[0];
            operandsBuffer[2][0] = operandsBuffer[1][0];
            operandsBuffer[2][1] = operandsBuffer[1][1];
            operandsBuffer[1][0] = operandsBuffer[0][0];
            operandsBuffer[1][1] = operandsBuffer[0][1];
        }
        printf("\n");
    }

    printf("Final register values:\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        printf("R%d: %d\n", i, registers[i]);
    }

    return 0;
}


void printState(int cycle) {
    printf("Registers:\n");
    for (int i = 0; i < NUM_REGISTERS; i++) {
        if (registers[i] != initialRegisters[i]) {
            printf("R%d: %d (changed)\n", i, registers[i]);
        }
    }
    printf("\n");
    printf("PC: %d\n", PC);
}
void fetch(uint16_t *instruction) {
    *instruction = instructionMemory[PC];

}


void decode(uint16_t instruction, uint8_t *opcode, uint8_t *operands) {
    *opcode = (instruction >> 12) & 0xF;
    operands[0] = (instruction >> 6) & 0x3F; 
    operands[1] =(instruction & 0x3F);
    
}
void execute(uint8_t opcode, uint8_t *operands) {
    uint8_t result;
    uint8_t operand1 = operands[0];
    uint8_t operand2 = operands[1];

    if (operand1 >= NUM_REGISTERS) {
        printf("Invalid register index\n");
        return;
    }
    if(!(opcode==0000&&operand1==000000&&operand2==000000)){

    printf("Executing opcode: %d\n", opcode);
    printf("Operand1: %d\n", operand1);
    printf("Operand2: %d\n", operand2);

    switch (opcode) {
        case ADD:
            printf("Adding %d and %d and storing the result in R%d\n", registers[operand1], registers[operand2], operand1);
            result = registers[operand1] + registers[operand2];
            updateFlags(opcode, registers[operand1], registers[operand2], result);
            registers[operand1] = result;
            break;
       case SUB:
            printf("Subtracting %d from %d and storing the result in R%d\n", registers[operand2], registers[operand1], operand1);
            result = registers[operand1] - registers[operand2]; 
            updateFlags(opcode, registers[operand1], registers[operand2], result); 
            registers[operand1] = result;
            break;

        case MUL:
            printf("Multiplying %d by %d and storing the result in R%d\n", registers[operand1], registers[operand2], operand1);
            result = registers[operand1] * registers[operand2];
            result &= 0xFF; 
            updateFlags(opcode, registers[operand1], registers[operand2], result);
            registers[operand1] = result;
            break;
        case LDI:
            registers[operand1] = operand2;
            break;
        case BEQZ:
            if (registers[operand1] == 0) {
                printf("Branch taken\n");
                PC += operand2-2;
                branchEncountered=true;
            } else {
                printf("Branch not taken\n");
            }
            break;
        case AND:
            result = registers[operand1] & registers[operand2];
            printf("Performing bitwise AND between R%d and R%d\n", operand1, operand2);
            updateFlags(opcode, registers[operand1], registers[operand2], result);
            registers[operand1] = result;
            break;
        case OR:
            result = registers[operand1] | registers[operand2];
            printf("Performing bitwise OR between R%d and R%d\n", operand1, operand2);
            updateFlags(opcode, registers[operand1], registers[operand2], result);
            registers[operand1] = result;
            break;
        case JR:
            if (operand1 >= NUM_REGISTERS || operand2 >= NUM_REGISTERS) {
                printf("Invalid register index\n");
                break;
            }
            printf("PC before jump: %d\n", PC);
            PC = (registers[operand1] << 8) | registers[operand2];
            printf("PC after jump: %d\n", PC);
            jumpEncountered=true;
            break;
        case SLC:
            result = (registers[operand1] << operand2) | (registers[operand1] >> (8 - operand2));
            result &= 0xFF; // Ensure the result is within 8 bits
            updateFlags(opcode, registers[operand1], 0, result);
            registers[operand1] = result;
            break;
        case SRC:
            result = (registers[operand1] >> operand2) | (registers[operand1] << (8 - operand2));
            result &= 0xFF;
            updateFlags(opcode, registers[operand1], 0, result);
            registers[operand1] = result;
            break;
        case LB:
            if (operand2 >= DATA_MEM_SIZE) {
                printf("Invalid memory address for load\n");
                break;
            }
            registers[operand1] = dataMemory[operand2];
            printf("Loaded byte 0x%02X into R%d from memory address %d\n", dataMemory[operand2], operand1, operand2);
            break;
        case SB:
            if (operand2 >= DATA_MEM_SIZE) {
                printf("Invalid memory address for store\n");
                break;
            }
            dataMemory[operand2] = registers[operand1];
            printf("Stored byte 0x%02X from R%d into memory address %d\n", registers[operand1], operand1, operand2);
            break;
        default:
            printf("Unknown opcode: %d\n", opcode);
            break;
    }
    }
    initialRegisters[operand1] = registers[operand1];
    if (operand2 < NUM_REGISTERS) {
        initialRegisters[operand2] = registers[operand2];
    }
}


void updateFlags(uint8_t opcode, uint8_t operand1, uint8_t operand2, uint8_t result) {
    SREG &= 0xE0;

    uint8_t carry_flag = (result & 0x100) != 0;
    if (carry_flag) SREG |= FLAG_C;
    printf("Setting Carry Flag (C): %d\n", carry_flag);

    uint8_t overflow_flag = 0;
    if (opcode == ADD || opcode == SUB) {
        uint8_t sign_op1 = operand1 >> 7;
        uint8_t sign_op2 = operand2 >> 7;
        uint8_t sign_res = result >> 7;
        
        if ((sign_op1 == sign_op2) && (sign_op1 != sign_res)) {
            overflow_flag = 1;
        }
    }
    if (overflow_flag) SREG |= FLAG_V;
    printf("Setting Overflow Flag (V): %d\n", overflow_flag);
    uint8_t negative_flag = (result & 0x80) != 0; 
    if (negative_flag) SREG |= FLAG_N;
    printf("Setting Negative Flag (N): %d\n", negative_flag);
    if (result == 0) SREG |= FLAG_Z;
    printf("Setting Zero Flag (Z): %d\n", result == 0);
    uint8_t sign_flag = negative_flag ^ overflow_flag;
    if (sign_flag) SREG |= FLAG_S;
    printf("Setting Sign Flag (S): %d\n", sign_flag);

    printf("Updated SREG value: 0x%02X\n", SREG);
}
void loadInstructionsFromFile(const char *filename) {
FILE *file = fopen(filename, "r");
if (file == NULL) {
perror("Error opening file");
return;
}
printf("File opened successfully.\n");
char line[256];
int i = 0;
while (fgets(line, sizeof(line), file)) { 
    printf("Read line from file: %s\n", line);
    char *mnemonic;
    char *op1;
    char *op2;
    mnemonic = strtok(line, " ");
    op1 = strtok(NULL, " ");
op2 = strtok(NULL, " ");
    if (mnemonic == NULL || op1 == NULL || op2 == NULL) {
        printf("Error parsing line: %s\n", line);
        continue;
    }

    printf("Read instruction: mnemonic=%s, op1=%s, op2=%s\n", mnemonic, op1, op2);

    uint8_t opcode = getOpcode(mnemonic);
    if (opcode == 0xFF) {
        printf("Unknown mnemonic: %s\n", mnemonic);
        continue;
    }

    int op1_value = atoi(op1 + 1); 

    int op2_value;
    if (op2[0] == 'R') {
        op2_value = atoi(op2 + 1); 
    } else {
        op2_value = atoi(op2);
    }

    uint16_t instruction = encodeInstruction(opcode, op1_value, op2_value);
    printf("Encoded instruction: 0x%04X\n", instruction);
    instructionMemory[i] = instruction;
    printf("Loaded instruction 0x%04X at memory location %d\n", instruction, i);
    i++;
    if (i >= INSTR_MEM_SIZE) {
        printf("Maximum instruction memory size reached. Ignoring additional instructions.\n");
        break; 
    }
}
fclose(file);
}

uint8_t getOpcode(const char *mnemonic) {
if (strcmp(mnemonic, "ADD") == 0) return ADD;
if (strcmp(mnemonic, "SUB") == 0) return SUB;
if (strcmp(mnemonic, "MUL") == 0) return MUL;
if (strcmp(mnemonic, "LDI") == 0) return LDI;
if (strcmp(mnemonic, "BEQZ") == 0) return BEQZ;
if (strcmp(mnemonic, "AND") == 0) return AND;
if (strcmp(mnemonic, "OR") == 0) return OR;
if (strcmp(mnemonic, "JR") == 0) return JR;
if (strcmp(mnemonic, "SLC") == 0) return SLC;
if (strcmp(mnemonic, "SRC") == 0) return SRC;
if (strcmp(mnemonic, "LB") == 0) return LB;
if (strcmp(mnemonic, "SB") == 0) return SB;
printf("Unknown instruction mnemonic: %s\n", mnemonic);
return 0xFF;
}

uint16_t encodeInstruction(uint8_t opcode, uint8_t operand1, uint8_t operand2) {
return (opcode << 12) | (operand1 << 6) | operand2;
}