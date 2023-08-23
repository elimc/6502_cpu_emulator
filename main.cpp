#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <time.h>

using byte = unsigned char;
using word = unsigned short;

using u32 = unsigned int;

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    byte Data[MAX_MEM];

    void initialize() {
        for ( u32 i = 0; i < MAX_MEM; i++ ) {
            Data[i] = 0;
        }
    }

    /* read 1 byte */
    byte operator[]( u32 Address ) const {
        // assert( Address < MAX_MEM );
        return Data[Address];
    }

    /* write 1 byte */
    byte &operator[]( u32 Address ) {
        // assert( Address < MAX_MEM );
        return Data[Address];
    }
};

struct CPU
{

    // https://en.wikibooks.org/wiki/6502_Assembly#Registers
    word PC;            // Program Counter
    word SP;            // Stack Pointer

    byte A, X, Y, S, P; // Registers
    byte C : 1, Z : 1, I : 1, D : 1, B : 1, V : 1, N : 1; // status Flags

    void reset( Mem &memory )
    {
        PC = 0xFFFC;
        SP = 0x0100;
        A = X = Y = S = P = 0;
        C = Z = I = D = B = V = N = 0;
        memory.initialize();
    }

    byte fetch_byte( u32 &Cycles, Mem &memory )
    {
        byte Data = memory.Data[PC];
        PC++;
        Cycles--;
        return Data;
    }

    // opcodes
    static constexpr byte
        INS_LDA_IM = 0xA9;

    static constexpr byte
        INS_LDA_ZP = 0xA5;

    void execute( u32 Cycles, Mem &memory)
    {
        while ( Cycles > 0 )
        {
            byte Ins = fetch_byte( Cycles, memory );
            switch ( Ins )
            {
                case INS_LDA_IM:
                {
                    byte Value = fetch_byte( Cycles, memory );
                    A = Value;
                    Z = ( A == 0 );
                    N = ( A & 0b10000000 ) > 0;
                } break;
                case INS_LDA_ZP:
                {
                    byte ZeroPageAddress = fetch_byte( Cycles, memory );
                    A = memory[ZeroPageAddress];
                    Z = ( A == 0 );
                    N = ( A & 0b10000000 ) > 0;
                } break;
                default:
                {
                    printf( "Instruction not handled %d", Ins );
                } break;
            }
        }
        
    }

};

int main () 
{
    Mem mem;
    CPU cpu;

    cpu.reset( mem );
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x42;
    mem[0x0042] = 0x84;
    cpu.execute( 2, mem );

    return 0;
}