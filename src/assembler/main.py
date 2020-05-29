from assembler import Assembler

def main():
    assembler = Assembler()

    #abre o arquivo
    assembler.open_asm()

    for line in assembler.program:
        assembler.line_counter += 1

        if len(line) >= 2 and assembler.is_label(line[0]):
            assembler.add_label(line[0])
            del line[0]

        assembler.op_handler(line)
    
    assembler.assemble()

if __name__ == '__main__':
    main()