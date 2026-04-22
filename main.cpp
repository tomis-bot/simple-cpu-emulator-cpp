#include <iostream>
#include <fstream>

#define CF_FLAG  0b00000001
#define EOF_FLAG 0b00000100
#define ZF_FLAG  0b00010000
#define OF_FLAG  0b00100000

using namespace std;

void IN(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags, ifstream &input_file)
{
    unsigned char reg = (IR >> 8) & 0b00000111;

    char symbol;
    if (input_file.get(symbol))
    {
        regs[reg] = (unsigned char)symbol;
        eflags &= ~EOF_FLAG;
    }
    else
    {
        eflags |= EOF_FLAG;
        cout << "End of file" << endl;
    }

    PC += 2;
}

void MOVC(unsigned short &IR, unsigned char regs[], unsigned char &PC)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char value = IR & 0x00FF;

    regs[reg] = value;
    PC += 2;
}

void JFE(unsigned short &IR, unsigned char &PC, unsigned char &eflags)
{
    unsigned char eof = eflags & EOF_FLAG;
    signed char offset = IR & 0x00FF;

    if (eof)
        PC += offset;
    else
        PC += 2;
}

void XOR(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char mode = (IR >> 7) & 0b00000001;

    if (mode == 0)
    {
        unsigned char regS = (IR >> 4) & 0b00000111;
        regs[reg] = regs[reg] ^ regs[regS];
    }
    else
    {
        unsigned char regS1 = (IR >> 4) & 0b00000111;
        unsigned char regS2 = IR & 0b00000111;
        regs[reg] = regs[regS1] ^ regs[regS2];
    }

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void LSL(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char regS = (IR >> 4) & 0b00000111;

    unsigned char value = regs[regS];

    if (value & 0b10000000)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = value << 1;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void SUB(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char mode = (IR >> 7) & 0b00000001;

    int temp;
    signed char a, b, res;

    if (mode == 0)
    {
        unsigned char regS = (IR >> 4) & 0b00000111;
        a = regs[reg];
        b = regs[regS];
        temp = regs[reg] - regs[regS];
    }
    else
    {
        unsigned char regS1 = (IR >> 4) & 0b00000111;
        unsigned char regS2 = IR & 0b00000111;
        a = regs[regS1];
        b = regs[regS2];
        temp = regs[regS1] - regs[regS2];
    }

    if (temp < 0)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = temp;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    res = (signed char)regs[reg];
    if ((a >= 0 && b < 0 && res < 0) || (a < 0 && b >= 0 && res >= 0))
        eflags |= OF_FLAG;
    else
        eflags &= ~OF_FLAG;

    PC += 2;
}

void LSR(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char regS = (IR >> 4) & 0b00000111;

    unsigned char value = regs[regS];

    if (value & 0b00000001)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = value >> 1;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void AND(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char mode = (IR >> 7) & 0b00000001;

    if (mode == 0)
    {
        unsigned char regS = (IR >> 4) & 0b00000111;
        regs[reg] = regs[reg] & regs[regS];
    }
    else
    {
        unsigned char regS1 = (IR >> 4) & 0b00000111;
        unsigned char regS2 = IR & 0b00000111;
        regs[reg] = regs[regS1] & regs[regS2];
    }

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void OR(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char mode = (IR >> 7) & 0b00000001;

    if (mode == 0)
    {
        unsigned char regS = (IR >> 4) & 0b00000111;
        regs[reg] = regs[reg] | regs[regS];
    }
    else
    {
        unsigned char regS1 = (IR >> 4) & 0b00000111;
        unsigned char regS2 = IR & 0b00000111;
        regs[reg] = regs[regS1] | regs[regS2];
    }

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void OUT(unsigned short &IR, unsigned char regs[], unsigned char &PC, ofstream &output_file)
{
    unsigned char regS = (IR >> 4) & 0b00000111;

    output_file << (char)regs[regS];
    PC += 2;
}

void JMP(unsigned short &IR, unsigned char &PC)
{
    signed char offset = IR & 0x00FF;
    PC += offset;
}

void NOP(unsigned char &PC)
{
    PC += 2;
}

void MOV(unsigned short &IR, unsigned char regs[], unsigned char &PC)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char regS = (IR >> 4) & 0b00000111;

    regs[reg] = regs[regS];
    PC += 2;
}

void INC(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;

    int temp = regs[reg] + 1;

    if (temp > 255)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = temp;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void DEC(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;

    int temp = regs[reg] - 1;

    if (temp < 0)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = temp;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    PC += 2;
}

void ADD(unsigned short &IR, unsigned char regs[], unsigned char &PC, unsigned char &eflags)
{
    unsigned char reg = (IR >> 8) & 0b00000111;
    unsigned char mode = (IR >> 7) & 0b00000001;

    int temp;
    signed char a, b, res;

    if (mode == 0)
    {
        unsigned char regS = (IR >> 4) & 0b00000111;
        a = regs[reg];
        b = regs[regS];
        temp = regs[reg] + regs[regS];
    }
    else
    {
        unsigned char regS1 = (IR >> 4) & 0b00000111;
        unsigned char regS2 = IR & 0b00000111;
        a = regs[regS1];
        b = regs[regS2];
        temp = regs[regS1] + regs[regS2];
    }

    if (temp > 255)
        eflags |= CF_FLAG;
    else
        eflags &= ~CF_FLAG;

    regs[reg] = temp;

    if (regs[reg] == 0)
        eflags |= ZF_FLAG;
    else
        eflags &= ~ZF_FLAG;

    res = (signed char)regs[reg];
    if ((a > 0 && b > 0 && res < 0) || (a < 0 && b < 0 && res >= 0))
        eflags |= OF_FLAG;
    else
        eflags &= ~OF_FLAG;

    PC += 2;
}

void JZ(unsigned short &IR, unsigned char &PC, unsigned char &eflags)
{
    signed char offset = IR & 0x00FF;

    if (eflags & ZF_FLAG)
        PC += offset;
    else
        PC += 2;
}

void JNZ(unsigned short &IR, unsigned char &PC, unsigned char &eflags)
{
    signed char offset = IR & 0x00FF;

    if (!(eflags & ZF_FLAG))
        PC += offset;
    else
        PC += 2;
}

void JC(unsigned short &IR, unsigned char &PC, unsigned char &eflags)
{
    signed char offset = IR & 0x00FF;

    if (eflags & CF_FLAG)
        PC += offset;
    else
        PC += 2;
}

void JO(unsigned short &IR, unsigned char &PC, unsigned char &eflags)
{
    signed char offset = IR & 0x00FF;

    if (eflags & OF_FLAG)
        PC += offset;
    else
        PC += 2;
}

int main()
{
    unsigned char regs[8] = {0};
    unsigned char RAM[256] = {0};

    unsigned char PC = 0;
    unsigned short IR = 0;
    unsigned char eflags = 0b00000010;

    ifstream bin_file;
    bin_file.open("decryptor2026v3.bin", ios::binary);

    ifstream input_file;
    input_file.open("encrypted_text.txt");

    ofstream output_file;
    output_file.open("output.txt");

    char symbol;
    while (bin_file.get(symbol))
    {
        RAM[PC] = (unsigned char)symbol;
        PC++;
    }
    bin_file.close();

    for (int i = 0; i < PC; i++)
    {
        cout << hex << (int)RAM[i] << " ";
    }
    cout << endl;
    PC = 0;

    while (true)
    {
        IR = RAM[PC];
        IR = IR << 8;
        IR = IR | RAM[PC + 1];

        unsigned char opcode = IR >> 11;

        if (opcode == 0x01) IN(IR, regs, PC, eflags, input_file);
        else if (opcode == 0x02) OUT(IR, regs, PC, output_file);
        else if (opcode == 0x03) MOV(IR, regs, PC);
        else if (opcode == 0x04) MOVC(IR, regs, PC);
        else if (opcode == 0x0A) JFE(IR, PC, eflags);
        else if (opcode == 0x10) XOR(IR, regs, PC, eflags);
        else if (opcode == 0x14) LSL(IR, regs, PC, eflags);
        else if (opcode == 0x0F) SUB(IR, regs, PC, eflags);
        else if (opcode == 0x13) LSR(IR, regs, PC, eflags);
        else if (opcode == 0x12) AND(IR, regs, PC, eflags);
        else if (opcode == 0x11) OR(IR, regs, PC, eflags);
        else if (opcode == 0x05) JMP(IR, PC);
        else if (opcode == 0x00) NOP(PC);
        else if (opcode == 0x0B)
        {
            cout << "\"Halt\", processor stopped." << endl;
            break;
        }
        else if (opcode == 0x0C) INC(IR, regs, PC, eflags);
        else if (opcode == 0x0D) DEC(IR, regs, PC, eflags);
        else if (opcode == 0x0E) ADD(IR, regs, PC, eflags);
        else if (opcode == 0x06) JZ(IR, PC, eflags);
        else if (opcode == 0x07) JNZ(IR, PC, eflags);
        else if (opcode == 0x08) JC(IR, PC, eflags);
        else if (opcode == 0x09) JO(IR, PC, eflags);
        else break;
    }

    cout << "RESULTS:" << endl;
    cout << "R[0] = " << (int)regs[0] << endl;
    cout << "R[1] = " << (int)regs[1] << endl;
    cout << "R[2] = " << (int)regs[2] << endl;
    cout << "R[3] = " << (int)regs[3] << endl;
    cout << "R[4] = " << (int)regs[4] << endl;
    cout << "R[5] = " << (int)regs[5] << endl;
    cout << "R[6] = " << (int)regs[6] << endl;
    cout << "R[7] = " << (int)regs[7] << endl;

    input_file.close();
    output_file.close();
    return 0;
}
