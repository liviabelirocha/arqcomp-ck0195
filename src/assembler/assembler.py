import sys
import struct

from instructions import instructions

class Assembler:
    #Inicialização do Assembler
    def __init__(self):
        self.program = []
        self.byte_array = []

        self.vars = {}
        self.labels = {}

        self.byte_counter = 0
        self.line_counter = 0
        self.next = 0

    #Função para lidar com erros e encerrar o programa
    def throw_error(self, function, message):
        print(f"ERRO! Na função {function}. {message} na linha {self.line_counter}")
        sys.exit()

    #Abre o arquivo e o coloca dentro de program, separando por linhas
    def open_asm(self):
        try:
            with open(sys.argv[1], 'r') as prog:
                self.program = [line.lower().split() for line in prog]
        except IndexError:
            print("Deve ser passado um arquivo para leitura")
            raise IndexError
        except IOError:
            print("O arquivo passado não foi encontrado")
            raise IOError

    #Encontra todas as labels
    def is_label(self, label):
        if label in self.vars:
            self.throw_error("is_label", "Label com mesmo nome de variável")
        return label not in instructions
    
    #Adiciona label ao dicionario de labels
    def add_label(self, label):
        self.labels[label] = self.byte_counter + 1
        
    #Lida com operações e operandos
    def op_handler(self, line):
        instruction = line[0]

        if instruction not in instructions:
            self.throw_error("op_handler", "Instrução inválida")

        operand_types = instructions[instruction][1] #Pega o tipo do operando
        number_of_operands = len(operand_types)      #Numero de operandos
        operand = line[1 : number_of_operands + 1]   #Pega o operando

        if self.check_operand(instruction, operand_types, number_of_operands, operand):
            self.add_instruction(instruction)
            self.add_operand(instruction, operand_types, number_of_operands, operand)
        else:
            self.throw_error("op_handler", "Operando inválido")

    #Checa se operando é valido
    def check_operand(self, instruction, operand_types, number_of_operands, operand):
        if number_of_operands == len(operand):
            if number_of_operands == 0:
                return True
            
            check = True

            for i in range (0, number_of_operands):
                if operand_types[i] == 'varnum':
                    #Precisa ser seguido de uma variável
                    if operand[i] not in self.vars:
                        check = False
    
                elif (operand_types[i] == 'byte' ) or (operand_types[i] == 'const') or (operand_types[i] == 'disp' ) or (operand_types[i] == 'index'): 
                    #Precisam ser seguidos de número
                    if not operand[i].isnumeric():
                        check = False

                else: #operand_types[i] == 'offset'
                    if not self.is_label(operand[i]):
                        check = False
            
            return check

        return False

    def add_instruction(self, instruction):
        self.byte_counter += 1
        self.byte_array.append(instructions[instruction][0])

    def add_operand(self, instruction, operand_types, number_of_operands, operand):
        for i in range (0, number_of_operands):
            if operand_types[i] == 'varnum':
                self.byte_array.append(self.vars[operand[i]])
                self.byte_counter += 1
            elif operand_types[i] == 'new_varnum':
                if operand[i] not in self.vars:
                    self.vars[operand[i]] = self.next
                    self.next += 1
                    self.byte_array.append(self.vars[operand[i]])
                    self.byte_counter += 1
            elif (operand_types[i] == 'byte') or (operand_types[i] == 'const'):
                self.byte_array.append(int(operand[i]))
                self.byte_counter += 1
            elif (operand_types[i] == 'disp') or (operand_types[i] == 'index'):
                self.byte_array.append(int(operand[i]) & 0xff)
                self.byte_array.append(int(operand[i]) >> 8)
                self.byte_counter += 2
            else:  #operand_types[i] == 'offset'
                self.byte_array.append([operand[i], self.byte_counter])
                self.byte_counter += 2

    def assemble(self):
        initialization = [
            0x7300,                 #init
            0x0006,                 #cpp
            0x1001,                 #lv
            0x0400,                 #pc
            0x1001 + len(self.vars) #sp
        ]

        array_of_bytes = bytearray()
        tam_prog = (self.byte_counter + 20).to_bytes(4, byteorder='little', signed=True)
        array_of_bytes += tam_prog

        for byte in initialization:
            array_of_bytes += byte.to_bytes(4, byteorder='little', signed=True)
        
        for byte in self.byte_array:
            if type(byte) == list:
                if byte[0] not in self.labels:
                    self.throw_error("assemble", "Label não encontrada")
                label_dist = self.labels[byte[0]] - byte[1]
                array_of_bytes += label_dist.to_bytes(2, 'big', signed=True)
            else:
                array_of_bytes.append(byte)
        
        binary = open(sys.argv[1][:-4] + '.exe', 'wb')
        binary.write(array_of_bytes)
        binary.close
        