.intel_syntax noprefix
.global main
main:
    push 5
    push 2
    pop rdi
    pop rax
    imul rax, rdi
    push rax
    push 20
    push 2
    pop rdi
    pop rax
    cqo
    idiv rdi
    push rax
    pop rdi
    pop rax
    cmp rax, rdi
    setle al
    movzb rax, al
    push rax
    pop rax
    ret
