# MachineKit [WIP]
<h3>

* Encode x86-64 machine language at runtime.
* Single header x86-64 code generator in under 300 lines of C99!
* Includes most common x86-64 instructions aswell as SSE and SSE2.
* AVX support is planned.

</h3>

<h2>SSE instruction encoding example</h2>

<h3>1. Encode some instructions from C:</h3>
```c
#include "X86_64.h"

int main() {
    X64MScar buf[8192];                 // create a buffer on the stack
    memset(buf, XI_INT3, sizeof(buf));  // fill the buffer with the INT3 (breakpoint) instruction
    X64MScar *p = buf;                  // create pointer as needle

    // encode some SSE register-to-register instructions
    X64_sse_rr(&p, XO_ADDPS,  RID_XMM8,  RID_XMM7);
    X64_sse_rr(&p, XO_SUBSS,  RID_XMM15, RID_XMM7);
    X64_sse_rr(&p, XO_MOVSS,  RID_XMM1,  RID_XMM7);
    X64_sse_rr(&p, XO_SUBSD,  RID_XMM8,  RID_XMM11);
    X64_sse_rr(&p, XO_MOVAPS, RID_XMM8,  RID_XMM7);
    X64_sse_rr(&p, XO_MOVUPD, RID_XMM15, RID_XMM7);
    X64_sse_rr(&p, XO_DIVPS,  RID_XMM1,  RID_XMM7);
    X64_sse_rr(&p, XO_MINPD,  RID_XMM8,  RID_XMM11);

    X64_machine_buf_dump_c(buf, p);   // dump the machine code in our buffer into a C like string literal

    return 0;
}
```

<h3>2. Machine code is generated:</h3>
<h4>And can be then executed (requires mmap etc..) or printed as C string literal (X64_machine_buf_dump_c):</h4>
```c
const char *machine_code = { /* N = 35 */
  "\x44\x0f\x58\xc7\xf3\x44\x0f\x5c"
  "\xff\xf3\x0f\x10\xcf\xf2\x45\x0f"
  "\x5c\xc3\x44\x0f\x28\xc7\x66\x44"
  "\x0f\x10\xff\x0f\x5e\xcf\x66\x45"
  "\x0f\x5d\xc3"
};
```
<h3> Disassembled machine code from above: </h3>

```asm
    Address        | Machine Language |     Assembly
------------------------------------------------------------
0x0000000000000000:  44 0F 58 C7       addps  %xmm7, %xmm8 
0x0000000000000004:  F3 44 0F 5C FF    subss  %xmm7, %xmm15
0x0000000000000009:  F3 0F 10 CF       movss  %xmm7, %xmm1 
0x000000000000000d:  F2 45 0F 5C C3    subsd  %xmm11, %xmm8 
0x0000000000000012:  44 0F 28 C7       movaps %xmm7, %xmm8 
0x0000000000000016:  66 44 0F 10 FF    movupd %xmm7, %xmm15
0x000000000000001b:  0F 5E CF          divps  %xmm7, %xmm1 
0x000000000000001e:  66 45 0F 5D C3    minpd  %xmm11, %xmm8 
```
