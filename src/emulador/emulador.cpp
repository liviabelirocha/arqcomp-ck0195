#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

//Definição dos Tipos
typedef unsigned char byte;               // 8 bits
typedef unsigned int palavra;             // 32 bits
typedef unsigned long int microinstrucao; //64 bits, porém só utiliza 36 bits

/*Definição dos registradores*/
//Acesso à memória
palavra MAR = 0, MDR = 0, PC = 0;
byte MBR = 0;
//Operações ULA
palavra SP = 0, LV = 0, CPP = 0, TOS = 0, OPC = 0, H = 0;

//Barramentos
palavra Barramento_B, Barramento_C; //Não há Barramento A pois ele é o próprio H
//Armazenamento de Controle
microinstrucao ArmazenamentoDeControle[512]; //512 posições de 64 bits
//Memória
#define MEM_TAM 0xFFFF + 1
byte memoria[MEM_TAM];

//Flip-Flops
byte N, Z;

//Microinstrução
microinstrucao MIR;

struct microinstrucao_decodificada
{
    /*
    Prox_MicInstruc = 9 bits
    Salto = 3 bits
    Deslocador = 2 bits
    Operação = 6 bits
    Barramento_C = 9 bits
    Memoria = 3 bits
    Barramento_B = 4 bits
    */
    palavra Prox_MicInstruc = 0, //MPC
        Barramento_C;
    byte Salto, Deslocador, Operacao, Memoria, Barramento_B;
};
microinstrucao_decodificada md;

/*Protótipo das funções*/
bool carregarArmazenamentoDeControle();
bool carregarPrograma(char *prog);

void atribuicaoMicroinstrucao();
void atribuicaoB();
void ALU();
void flipFlop();
void deslocamento();
void atribuicaoC();
void oprMemoria();
void saltar();

void exibicao();
void print(void *valor, int tipo);

int main(int argc, char *argv[])
{
    (void)argc;
    if (carregarArmazenamentoDeControle() || carregarPrograma(argv[1]))
        return 1;

    while (true)
    {
        exibicao();
        //Microinstrução atual
        MIR = ArmazenamentoDeControle[md.Prox_MicInstruc];

        //Operações a serem realizadas
        atribuicaoMicroinstrucao();
        atribuicaoB();

        ALU();
        flipFlop();
        deslocamento();

        atribuicaoC();
        oprMemoria();
        saltar();

        //Só vai para próxima instrução quando o "enter" for pressionado
        getchar();
    }
    return 0;
}

bool carregarArmazenamentoDeControle()
{
    FILE *armazenamento;                          //Objeto que contém a informação para controlar uma stream
    armazenamento = fopen("microprog.rom", "rb"); //rb -> read binary
    if (armazenamento)
    {
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
    }
    cout << "(ERRO) Microprograma não existe!" << endl;
    return true;
}

bool carregarPrograma(char *prog)
{
    FILE *programa;
    byte tamanho_temp[4];
    palavra tamanho;
    programa = fopen(prog, "rb");

    if (programa)
    {
        fread(tamanho_temp, 1, 4, programa);
        memcpy(&tamanho, tamanho_temp, 4);
        fread(memoria, 1, 20, programa);                    // Lendo os 20 bytes de Inicialização
        fread(&memoria[0x0401], 1, tamanho - 20, programa); // Lendo o Programa
        fclose(programa);
        return false;
    }
    cout << "(ERRO) Programa não existe!" << endl;
    return true;
}

//"Quebra" da microinstrução em várias partes
void atribuicaoMicroinstrucao()
{
    md.Barramento_B = (MIR)&0b1111;
    md.Memoria = (MIR >> 4) & 0b111;
    md.Barramento_C = (MIR >> 7) & 0b111111111;
    md.Operacao = (MIR >> 16) & 0b111111;
    md.Deslocador = (MIR >> 22) & 0b11;
    md.Salto = (MIR >> 24) & 0b111;
    md.Prox_MicInstruc = (MIR >> 27) & 0b111111111;
}

//4 bits, variando de 0 a 8
void atribuicaoB()
{
    switch (md.Barramento_B)
    {
    case 0:
        Barramento_B = MDR;
        break;
    case 1:
        Barramento_B = PC;
        break;
    case 2:
        Barramento_B = MBR;
        break;
    case 3:
        Barramento_B = MBR;
        if (MBR & 0b10000000)
            Barramento_B = Barramento_B | (0b111111111111111111111111 << 8);
        break;
    case 4:
        Barramento_B = SP;
        break;
    case 5:
        Barramento_B = LV;
        break;
    case 6:
        Barramento_B = CPP;
        break;
    case 7:
        Barramento_B = TOS;
        break;
    case 8:
        Barramento_B = OPC;
        break;
    default:
        Barramento_B = -1;
        break;
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
void ALU()
{
    switch (md.Operacao)
    {
    case 12:
        Barramento_C = H & Barramento_B;
        break;
    case 16:
        Barramento_C = 0;
        break;
    case 20:
        Barramento_C = Barramento_B;
        break;
    case 24:
        Barramento_C = H;
        break;
    case 26:
        Barramento_C = ~H;
        break;
    case 28:
        Barramento_C = H | Barramento_B;
        break;
    case 44:
        Barramento_C = ~Barramento_B;
        break;
    case 49:
        Barramento_C = 1;
        break;
    case 50:
        Barramento_C = -1;
        break;
    case 53:
        Barramento_C = Barramento_B + 1;
        break;
    case 54:
        Barramento_C = Barramento_B - 1;
        break;
    case 57:
        Barramento_C = H + 1;
        break;
    case 59:
        Barramento_C = -H;
        break;
    case 60:
        Barramento_C = H + Barramento_B;
        break;
    case 61:
        Barramento_C = H + Barramento_B + 1;
        break;
    case 63:
        Barramento_C = Barramento_B - H;
        break;
    default:
        break;
    }
}

void flipFlop()
{
    if (Barramento_C)
    {
        N = 1;
        Z = 0;
    } //Se != 0
    else
    {
        N = 0;
        Z = 1;
    }; //Se == 0
}

void deslocamento()
{
    switch (md.Deslocador)
    {
    case 1:
        Barramento_C = Barramento_C << 8;
        break;
    case 2:
        Barramento_C = Barramento_C >> 1;
        break;
    }
}

void atribuicaoC()
{
    if (md.Barramento_C & 0b000000001)
        MAR = Barramento_C;
    if (md.Barramento_C & 0b000000010)
        MDR = Barramento_C;
    if (md.Barramento_C & 0b000000100)
        PC = Barramento_C;
    if (md.Barramento_C & 0b000001000)
        SP = Barramento_C;
    if (md.Barramento_C & 0b000010000)
        LV = Barramento_C;
    if (md.Barramento_C & 0b000100000)
        CPP = Barramento_C;
    if (md.Barramento_C & 0b001000000)
        TOS = Barramento_C;
    if (md.Barramento_C & 0b010000000)
        OPC = Barramento_C;
    if (md.Barramento_C & 0b100000000)
        H = Barramento_C;
}

void oprMemoria()
{
    if (md.Memoria & 0b001)
        MBR = memoria[PC];
    if (md.Memoria & 0b010)
        memcpy(&MDR, &memoria[MAR * 4], 4);
    if (md.Memoria & 0b100)
        memcpy(&memoria[MAR * 4], &MDR, 4);
}

void saltar()
{
    if (md.Salto & 0b001)
        md.Prox_MicInstruc = md.Prox_MicInstruc | (Z << 8);
    if (md.Salto & 0b010)
        md.Prox_MicInstruc = md.Prox_MicInstruc | (N << 8);
    if (md.Salto & 0b100)
        md.Prox_MicInstruc = md.Prox_MicInstruc | (MBR);
}

//Funções de exibição e print foram feitas de forma diferente da passada pelo professor, mas não alteram em nada a leitura dos dados
void exibicao()
{
    system("clear");

    if (LV && SP)
    {
        cout << "\nPILHA DE OPERANDOS\n";

        cout << "\n   BINÁRIO\t\t\t\tENDEREÇO    INT\n";

        for (unsigned int i = SP; i >= LV; i--)
        {
            palavra valor;
            memcpy(&valor, &memoria[i * 4], 4);

            print(&valor, 1);
            cout << "\t " << i << "\t     " << valor << endl;
        }
    }

    if (PC >= 0x0401)
    {
        cout << "\nÁREA DO PROGRAMA\n";
        cout << "\n\t\t BINÁRIO        HEX   ENDEREÇO\n";

        for (unsigned int i = PC - 2; i <= PC + 3; i++)
        {
            if (i == PC)
                cout << "  Em execução >";
            else
                cout << "\t       ";
            print(&memoria[i], 2);
            printf("\t0x%02X", memoria[i]);
            cout << "\t " << i << endl;
        }
    }

    cout << "\nREGISTRADORES";

    ;
    cout << "\n\n\t\t\t  BINÁRIO\t       INT\n"
         << "\nMAR :  ";
    print(&MAR, 3);
    cout << "      " << MAR
         << "\nMDR :  ";
    print(&MDR, 3);
    cout << "      " << MDR
         << "\nPC  :  ";
    print(&PC, 3);
    cout << "      " << PC
         << "\nMBR :\t\t\t       ";
    print(&MBR, 2);
    cout << "      " << (palavra)MBR
         << "\nSP  :  ";
    print(&SP, 3);
    cout << "      " << SP
         << "\nLV  :  ";
    print(&LV, 3);
    cout << "      " << LV
         << "\nCPP :  ";
    print(&CPP, 3);
    cout << "      " << CPP
         << "\nTOS :  ";
    print(&TOS, 3);
    cout << "      " << TOS
         << "\nOPC :  ";
    print(&OPC, 3);
    cout << "      " << OPC
         << "\nH   :  ";
    print(&H, 3);
    cout << "      " << H
         << "\nMPC :\t\t\t  ";
    print(&md.Prox_MicInstruc, 5);
    cout << "          " << md.Prox_MicInstruc << endl;

    cout << "\nMIR :  ";
    print(&MIR, 4);
}

/*
1: Binário de 4 bytes seguidos
2: Binário de 1 byte
3: Binario de uma palavra
4: Binário de uma microinstrução
5: Binário dos 9 bits do MPC
*/
void print(void *valor, int tipo)
{
    printf("  ");
    switch (tipo)
    {
    case 1:
    {
        printf(" ");
        byte aux;
        byte *valorAux = (byte *)valor;

        for (int i = 3; i >= 0; i--)
        {
            aux = *(valorAux + i);
            for (int j = 0; j < 8; j++)
            {
                printf("%d", (aux >> 7) & 0b1);
                aux = aux << 1;
            }
            printf(" ");
        }
    }
    break;

    case 2:
    {
        byte aux;

        aux = *((byte *)(valor));
        for (int j = 0; j < 8; j++)
        {
            printf("%d", (aux >> 7) & 0b1);
            aux = aux << 1;
        }
    }
    break;

    case 3:
    {
        palavra aux;

        aux = *((palavra *)(valor));
        for (int j = 0; j < 32; j++)
        {
            printf("%d", (aux >> 31) & 0b1);
            aux = aux << 1;
        }
    }
    break;

    case 4:
    {
        microinstrucao aux;

        aux = *((microinstrucao *)(valor));
        for (int j = 0; j < 36; j++)
        {
            if (j == 9 || j == 12 || j == 20 || j == 29 || j == 32)
                cout << "  ";

            printf("%ld", (aux >> 35) & 0b1);
            aux = aux << 1;
        }
    }
    break;

    case 5:
    {
        palavra aux;

        aux = *((palavra *)(valor)) << 23;
        for (int j = 0; j < 9; j++)
        {
            printf("%d", (aux >> 31) & 0b1);
            aux = aux << 1;
        }
    }
    break;
    }
}