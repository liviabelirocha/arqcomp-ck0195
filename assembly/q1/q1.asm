org 0x7c00
bits 16

mov ax, 0
mov ds, ax

cli

mov al, 0x13        ; mudar modo de vídeo para modo gráfico (VGA 320x200)
int 0x10

int 0x13            ; interrupção de disco
    mov ah, 0x02    ; fazer leitura dos setores
    mov al, 32      ; ler 32 setores
    mov cl, 2       ; a partir do setor 2
    mov ch, 0       ; cilindro 0
    mov dh, 0       ; cabeçote 0
    mov bx, 0x7e00  ; gravar dados lidos
int 0x13

mov ax, 0xa000      ; guarda o endereço da VRAM em es
mov es, ax

mov si, 0x7e00      ; endereço que a imagem está salva
mov di, 0           ; utilizar di como índice

looping:
    mov al, [ds:si] ; copia o setor 0 no índice si para es no índice di
    mov [es:di], al
    cmp di, 16000   ; checa se di chegou em 16000 (numero de bytes da imagem)
    je end          ; se sim, o programa acaba
    inc si          ; incrementa si
    inc di          ; incrementa di
    jmp looping     ; outra iteração do laço

end:
    jmp end

times 510 - ($-$$) db 0
dw 0XAA55