// Copyright (c) 2023 Mario "VEX" Sieg <mario.sieg.64@gmail.com>. All Rights Reserved.
// MIT licensed.

#ifndef X86_64_H
#define X86_64_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t X64MScar;
#define X64_VLA_MAX 15

typedef enum {
    RID_RAX, RID_RCX, RID_RDX, RID_RBX, RID_RSP, RID_RBP, RID_RSI, RID_RDI,
    RID_R8,  RID_R9,  RID_R10, RID_R11, RID_R12, RID_R13, RID_R14, RID_R15,
    RID_$MAX
} X64GprReg;

typedef enum {
    RID_XMM0, RID_XMM1, RID_XMM2,  RID_XMM3,  RID_XMM4,  RID_XMM5,  RIX_XMM6,  RID_XMM7,
    RID_XMM8, RID_XMM9, RID_XMM10, RID_XMM11, RID_XMM12, RID_XMM13, RIX_XMM14, RID_XMM15
} X64FprReg;

typedef enum {
    MRM_INDIR, MRM_SIG_DISP8, MRM_SIG_DISP32, MRM_DIR
} X64Mrm;

typedef enum {
    GAU_DOUBLEWORD = 4, GAU_QUADWORD = 8
} X64Gauge;
 
/* construct VLA SSE opcodes | little endian  */
#define SSE_PS(o) ((uint32_t)(0xaa00000fu|((0x##o##u&0xffu)<<8)))  /*    0f = packed single prec */ 
#define SSE_PD(o) ((uint32_t)(0x00000f66u|((0x##o##u&0xffu)<<16))) /* 66 0f = packed double prec */
#define SSE_SS(o) ((uint32_t)(0x00000ff3u|((0x##o##u&0xffu)<<16))) /* f3 0f = scalar single prec */
#define SSE_SD(o) ((uint32_t)(0x00000ff2u|((0x##o##u&0xffu)<<16))) /* f2 0f = scalar double prec */

#define SSE_QUADRUPLE(mnemonic, xo)\
    XO_##mnemonic##PS = SSE_PS(xo),\
    XO_##mnemonic##PD = SSE_PD(xo),\
    XO_##mnemonic##SS = SSE_SS(xo),\
    XO_##mnemonic##SD = SSE_SD(xo)

#define SSE_AU_QUADRUPLE(mnemonic, xo_a, xo_u)\
    XO_##mnemonic##APS = SSE_PS(xo_a),\
    XO_##mnemonic##APD = SSE_PD(xo_a),\
    XO_##mnemonic##UPS = SSE_PS(xo_u),\
    XO_##mnemonic##UPD = SSE_PD(xo_u)

#define SSE_DUPLET(mnemonic, xo)\
    XO_##mnemonic##SS = SSE_SS(xo),\
    XO_##mnemonic##SD = SSE_SD(xo)

typedef enum {
    XI_INT3 = 0xCC,
    SSE_DUPLET(MOV, 10),            /* mov[ss/sd]           */
    SSE_AU_QUADRUPLE(MOV, 28, 10),  /* mov[aps/apd/ups/upd] */
    SSE_QUADRUPLE(ADD, 58),         /* opc[ps/pd/ss/sd]  */
    SSE_QUADRUPLE(SUB, 5c),         /* opc[ps/pd/ss/sd]  */
    SSE_QUADRUPLE(MUL, 59),         /* opc[ps/pd/ss/sd]  */
    SSE_QUADRUPLE(DIV, 5e),         /* opc[ps/pd/ss/sd]  */
    SSE_QUADRUPLE(MIN, 5d),         /* opc[ps/pd/ss/sd]  */
    SSE_QUADRUPLE(MAX, 5f)
} X64Opc;

#define X64_ModRm(mod, ro, rx) ((X64MScar)(((mod)&3)<<6)|(((ro)&7)<<3)|((rx)&7))
#define X64_REX64 0x40

static inline void X64_rex(X64MScar **restrict p, X64MScar mod, X64MScar idx, X64MScar rmo, X64Gauge gau) {
    enum { B = 1<<0, X = 1<<1, R = 1<<2, W = 1<<3 };
    X64MScar rex = 0;
    rex |= gau == GAU_QUADWORD ? W : 0;
    rex |= mod > 7 ? R : 0;
    rex |= idx > 7 ? X : 0;
    rex |= rmo > 7 ? B : 0;
    if (rex) { *(*p)++ = X64_REX64 | rex; }
}

static inline void X64_sse_rr(X64MScar **restrict p, X64Opc xo, X64FprReg dst, X64FprReg src) {
    bool rex = src > RID_XMM7 || dst > RID_XMM7;
    if (((xo >> 24) & 0xff) == 0xaa) {
        if (rex) X64_rex(p, dst, 0, src, GAU_DOUBLEWORD);
        *(uint16_t *)*p = (uint16_t)xo;
        *p += 2;
    }
    else {
        X64MScar op;
        if (rex) {
            op = (xo >> 16) & 0xff;
            X64_rex(p, dst, 0, src, GAU_DOUBLEWORD);
            --*p;
            xo = (xo & 0xff) | (**p << 8);
        }
        *(uint16_t *)*p = (uint16_t)xo;
        *p += 2;
        if (rex) *(*p)++ = 0x0f;
        *(*p)++ = rex ? op : (X64MScar)(xo >> 16);
    }
    *(*p)++ = X64_ModRm(MRM_DIR, dst, src);
}

static inline void X64_machine_buf_dump_c(const X64MScar *buf, const X64MScar *p) {
    ptrdiff_t n = p - buf;
    assert(n >= 0);
    printf("const char *machine_code = { /* N = %zu */\n\t\"", (size_t)n);
    for (ptrdiff_t i = 0; i < n; ++i) {
        if (i && i % 8 == 0) {
            printf("\"\n\t\"");
        }
        printf("\\x%02x", buf[i]);
    }
    printf("\"\n};\n");
}

#ifdef __cplusplus
}
#endif

#endif
