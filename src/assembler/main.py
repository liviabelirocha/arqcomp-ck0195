from assembler import Assembler


def main():
    assembler = Assembler()

    #abre o arquivo
    assembler.open_asm()

    for line in assembler.program:
        assembler.line_counter += 1

        assembler.is_label(line)
        assembler.op_handler(line)


if __name__ == '__main__':
    main()