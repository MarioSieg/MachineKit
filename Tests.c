// Copyright (c) 2023 Mario "VEX" Sieg <mario.sieg.64@gmail.com>. All Rights Reserved.
// MIT licensed.

#include "X86_64.h"

int main() {
    X64MScar buf[8192];
    memset(buf, XI_INT3, sizeof(buf));
    X64MScar *p = buf;

    X64_sse_rr(&p, XO_ADDPS, RID_XMM8, RID_XMM7);
    X64_sse_rr(&p, XO_SUBSS, RID_XMM15, RID_XMM7);
    X64_sse_rr(&p, XO_MOVSS, RID_XMM1, RID_XMM7);
    X64_sse_rr(&p, XO_SUBSD, RID_XMM8, RID_XMM11);
    X64_sse_rr(&p, XO_MOVAPS, RID_XMM8, RID_XMM7);
    X64_sse_rr(&p, XO_MOVUPD, RID_XMM15, RID_XMM7);
    X64_sse_rr(&p, XO_DIVPS, RID_XMM1, RID_XMM7);
    X64_sse_rr(&p, XO_MINPD, RID_XMM8, RID_XMM11);

    X64_machine_buf_dump_c(buf, p);

    return 0;
}