.text

/*
 * struct context *swap_context(struct context *current, struct context *next);
 * This routine switches current context from <current> to <next>.
 */
.globl swap_context
swap_context:
    # Save callee-saved registers.
    mov %rbx, (%rdi)
    mov %r12, 8(%rdi)
    mov %r13, 16(%rdi)
    mov %r14, 24(%rdi)
    mov %r15, 32(%rdi)
    mov %rsp, 40(%rdi)
    mov %rbp, 48(%rdi)
    
    # Restore callee-saved registers.
    mov (%rsi), %rbx
    mov 8(%rsi), %r12
    mov 16(%rsi), %r13
    mov 24(%rsi), %r14
    mov 32(%rsi), %r15
    mov 40(%rsi), %rsp
    mov 48(%rsi), %rbp

    # Return to the next context.
    mov %rdi, %rax
    ret
