import sys
import struct

from instructions import instructions


class Assembler:
    def __init__(self):
        self.program = []
        self.vars = []
        self.byte_array = []
        self.labels = {}
        self.byte_counter = 0
        self.line_counter = 0

    def throw_error(self, message, line):
        print(f"ERRO! {message} na linha {line}")
        sys.exit()

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
    def is_label(self, line):
        if len(line) >= 2 and line[0] not in instructions:
            self.labels[line[0]] = self.byte_counter + 1
            del line[0]

    #Lida com operações e operandos
    def op_handler(self, line):
        if line[0] not in instructions:
            self.throw_error("Instrução inválida", self.line_counter)

        instruction = line[0]

        #Adiciona instrução ao byte_array
        self.byte_array.append(instructions[instruction])
        self.byte_counter += 1

    def init(self):
        initialization = [
            0x7300,  #init
            0x0006,  #cpp
            0x1001,  #lv
            0x0400,  #pc
            0x1001 + len(self.vars)  #sp
        ]
