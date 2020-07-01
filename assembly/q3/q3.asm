org 0x7c00
bits 16

mov ax, 0
mov ds, ax

cli

mov cx, 0        ; inicializa quantidade de letras como 0

read:
    mov ax, 0    ; leitura do teclado
    int 0x16

    cmp al, 13   ; checa se a tecla enter foi apertada
    je lineBreak

    mov ah, 0x0e ; printa na tela o que foi lido
    int 0x10

    push ax      ; coloca o valor lido na pilha
    inc cx       ; incrementa a quantidade de letras

    jmp read

lineBreak:       ; faz a quebra de linha
    mov ah, 0x02
    mov dh, 1
    int 0x10

invert:
    pop ax       ; retira um elemento da pilha
    int 0x10     ; printa o elemento
    dec cx       ; decrementa a quantidade de letras
    jz end       ; se dx for zero, acaba o programa
    jmp invert   ; senão faz outra iteração do laço

end:
    hlt

times 510 - ($-$$) db 0
dw 0XAA55