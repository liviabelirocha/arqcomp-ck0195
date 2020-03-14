#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

//Definição dos Tipos
typedef unsigned char byte; // 8 bits
typedef unsigned int palavra; // 32 bits
typedef unsigned long int microinstrucao; //64 bits, porém só utiliza 36 bits

/*Definição dos registradores*/
//Acesso à memória
palavra MAR=0, MDR=0, PC=0;
byte MBR=0;
//Operações ULA
palavra SP=0, LV=0, CPP=0, TOS=0, OPC=0, H=0;

//Barramentos
palavra Barramento_B, Barramento_C; //Não há Barramento A pois ele é o próprio H
//Próxima Microinstrução
palavra MPC;
//Armazenamento de Controle
microinstrucao ArmazenamentoDeControle[512]; //512 posições de 64 bits

//Flip-Flops
byte N, Z;

//Microinstrução
microinstrucao MIR;

/*
Auxílio para microinstrução
MIR_Prox_MicInstruc = 9 bits
MIR_Salto = 3 bits
MIR_Deslocador = 2 bits
MIR_Operação = 6 bits
MIR_Barramento_C = 9 bits
MIR_Memoria = 3 bits
MIR_Barramento_B = 4 bits
*/
palavra MIR_Prox_MicInstruc, MIR_Barramento_C;
byte MIR_Salto, MIR_Deslocador, MIR_Operacao, MIR_Memoria, MIR_Barramento_B;

/*Protótipo das funções*/
bool carregarArmazenamentoDeControle();
void atribuicaoMicroinstrucao();
void atribuicaoB();
void ALU();
void atribuicaoC();

/*Main*/
int main() {
    if ( carregarArmazenamentoDeControle() ) return 1;
    int i = 0;
    while(true) {
        cout << "Iteração " << i;

        //Microinstrução atual
        MIR = ArmazenamentoDeControle[MPC];

        //Operações a serem realizadas
        atribuicaoMicroinstrucao();
        atribuicaoB();
        ALU();
        atribuicaoC();

        //Só vai para próxima instrução quando o "enter" for pressionado
        i += 1;
        getchar();
    }
    return 0;
}

bool carregarArmazenamentoDeControle() {
    FILE * armazenamento; //Objeto que contém a informação para controlar uma stream
    armazenamento = fopen("microprog.rom", "rb"); //rb -> read binary
    if (armazenamento) { 
        /*
        fread ( void * ptr, size_t size, size_t count, FILE * stream );
        void * ptr -> bloco de memória com tamanho suficiente
        size_t size -> tamanho em bytes de cada elemento a ser lido, ou seja, cada microinstrução (sizeof(microinstrucao) = 8)
        size_t count -> número de elementos a ser lido
        FILE * stream -> objeto especificado anteriormente
        */
        fread(ArmazenamentoDeControle, 8, 512, armazenamento);
        fclose(armazenamento);
        return false;
    } else {
        cout << "(ERRO) Arquivo não existe!" << endl;
        return true;
    }
}

//"Quebra" da microinstrução em várias partes
void atribuicaoMicroinstrucao() {
    MIR_Barramento_B = (MIR) & 0b1111;
    MIR_Memoria = (MIR >> 4) & 0b111;
    MIR_Barramento_C = (MIR >> 7) & 0b111111111;
    MIR_Operacao = (MIR >> 16) & 0b111111;
    MIR_Deslocador = (MIR >> 24) & 0b11;
    MIR_Salto = (MIR >> 24) & 0b111;
    MIR_Prox_MicInstruc = (MIR >> 27) & 0b111111111;
}

//4 bits, variando de 0 a 8
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

/*
Binário -> Operação  -> Valor
001100  -> A AND B   -> 12
010000  -> 0         -> 16
010100  -> B         -> 20
011000  -> A         -> 24
011010  -> ~A        -> 26
011100  -> A OR B    -> 28
101100  -> ~B        -> 44
110001  -> 1         -> 49
110010  -> –1        -> 50
110101  -> B + 1     -> 53
110110  -> B – 1     -> 54
111001  -> A + 1     -> 57
111011  -> –A        -> 59
111100  -> A + B     -> 60
111101  -> A + B + 1 -> 61
111111  -> B – A     -> 63
*/
void ALU() {
    switch(MIR_Operacao) {
        case 12: Barramento_C = H & Barramento_B;     break;
        case 16: Barramento_C = 0;                    break;
        case 20: Barramento_C = Barramento_B;         break;
        case 24: Barramento_C = H;                    break;
        case 26: Barramento_C = ~H;                   break;
        case 28: Barramento_C = H | Barramento_B;     break;
        case 44: Barramento_C = ~Barramento_B;        break;
        case 49: Barramento_C = 1;                    break;
        case 50: Barramento_C = -1;                   break;
        case 53: Barramento_C = Barramento_B + 1;     break;
        case 54: Barramento_C = Barramento_B - 1;     break;
        case 57: Barramento_C = H + 1;                break;
        case 59: Barramento_C = -H;                   break;
        case 60: Barramento_C = H + Barramento_B;     break;
        case 61: Barramento_C = H + Barramento_B + 1; break;
        case 63: Barramento_C = Barramento_B - H;     break;
        default:                                      break;
    }

    if (Barramento_C) { N = 1; Z = 0; } //Se != 0
    else { N = 0; Z = 1; };             //Se == 0
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