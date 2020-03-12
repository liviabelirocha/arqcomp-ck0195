#include <iostream>

//Definição dos Tipos
typedef unsigned char byte; // 8 bits
typedef unsigned int palavra; // 32 bits
typedef unsigned long int microinstrucao; //64 bits, porém só utiliza 36 bits

/*Definição dos registradores*/
//Acesso à memória
palavra MAR=0, MDR=0;
byte PC=0, MBR=0;

//Operações ULA
palavra SP=0, LV=0, CPP=0, TOS=0, OPC=0, H=0;

//Barramentos
palavra Barramento_B, Barramento_C; //Não há Barramento A pois ele é o próprio H

//Microinstrução
microinstrucao MIR;

//Auxílio para microinstrução
/*
MIR_Barramento_C = 9 bytes
MIR_Barramento_B = 4 bytes
*/
palavra MIR_Barramento_C;
byte MIR_Barramento_B;

/*Protótipo das funções*/ 
void atribuicaoB();
void atribuicaoC();

/*Main*/
int main() {
    return 0;
}

//4 bytes, variando de 0 a 8
void atribuicaoB() {
    switch(MIR_Barramento_B) {
        case 0: Barramento_B  = MDR; break;
        case 1: Barramento_B  = PC;  break;
        case 2: Barramento_B  = MBR; break;
        case 3: Barramento_B  = MBR; break;
        case 4: Barramento_B  = SP;  break;
        case 5: Barramento_B  = LV;  break;
        case 6: Barramento_B  = CPP; break;
        case 7: Barramento_B  = TOS; break;
        case 8: Barramento_B  = OPC; break;
        default: Barramento_B = -1;  break;
    }
}

void atribuicaoC() {
    if(MIR_Barramento_C & 0b000000001) MAR = Barramento_C;
	if(MIR_Barramento_C & 0b000000010) MDR = Barramento_C;
	if(MIR_Barramento_C & 0b000000100) PC  = Barramento_C;
	if(MIR_Barramento_C & 0b000001000) SP  = Barramento_C;
	if(MIR_Barramento_C & 0b000010000) LV  = Barramento_C;
	if(MIR_Barramento_C & 0b000100000) CPP = Barramento_C;
	if(MIR_Barramento_C & 0b001000000) TOS = Barramento_C;
	if(MIR_Barramento_C & 0b010000000) OPC = Barramento_C;
	if(MIR_Barramento_C & 0b100000000) H   = Barramento_C;
}