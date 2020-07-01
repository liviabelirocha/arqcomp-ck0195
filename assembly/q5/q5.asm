org 0x7c00
bits 16

mov ax, 0
mov ds, ax

cli

mov al, 0x13       ; mudar modo de vídeo para modo gráfico (VGA 320x200)
int 0x10

mov ax, 0xa000     ; guarda o endereço da VRAM em es
mov es, ax

read:
    mov ax, 0      ; leitura do teclado
    int 0x16

mov di, 64000      ; utilizar di como índice para percorrer a tela

paint:
    mov[es:di], al ; atribui a tecla digitada a es no indice di
    dec di         ; incrementa di
    jz read        ; se di for zero aceita uma nova leitura do teclado
    jmp paint      ; senão retorna ao laço

times 510 - ($-$$) db 0
dw 0XAA55