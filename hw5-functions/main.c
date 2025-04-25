#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int IsPassOk(void);
int main(void) {
  int PwStatus;
  puts("Enter password:");
  PwStatus = IsPassOk();
  if (PwStatus == 0) {
    printf("Bad password!\n");
    exit(1);
  } else {
    printf("Access granted!\n");
  }
  return 0;
}
int IsPassOk(void) {
  char Pass[12];
  gets(Pass);
  return 0 == strcmp(Pass, "test");
}

// gcc -Wall -fno-stack-protector -no-pie main.c

// Dump of assembler code for function IsPassOk:
//    0x00000000004011ee <+0>:     endbr64
//    0x00000000004011f2 <+4>:     push   %rbp
//    0x00000000004011f3 <+5>:     mov    %rsp,%rbp
//    0x00000000004011f6 <+8>:     sub    $0x10,%rsp
//    0x00000000004011fa <+12>:    lea    -0xc(%rbp),%rax
//    0x00000000004011fe <+16>:    mov    %rax,%rdi
//    0x0000000000401201 <+19>:    mov    $0x0,%eax
//    0x0000000000401206 <+24>:    call   0x401090 <gets@plt>
//    0x000000000040120b <+29>:    lea    -0xc(%rbp),%rax
//    0x000000000040120f <+33>:    lea    0xe1c(%rip),%rdx        # 0x402032
//    0x0000000000401216 <+40>:    mov    %rdx,%rsi
//    0x0000000000401219 <+43>:    mov    %rax,%rdi
//    0x000000000040121c <+46>:    call   0x401080 <strcmp@plt>
//    0x0000000000401221 <+51>:    test   %eax,%eax
//    0x0000000000401223 <+53>:    sete   %al
//    0x0000000000401226 <+56>:    movzbl %al,%eax
//    0x0000000000401229 <+59>:    leave
//    0x000000000040122a <+60>:    ret