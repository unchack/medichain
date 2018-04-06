#include "arch.h"
#include "fp_254BNCX.h"

/* Curve BN254CX - Pairing friendly BN curve */

/* CertiVox BN curve/field  */

#if CHUNK==16
const chunk MConst_254BNCX=0x1E85;
const BIG_256 Modulus_254BNCX= {0x15B3,0xDA,0x1BD7,0xC47,0x1BE6,0x1F70,0x24,0x1DC3,0x1FD6,0x1921,0x19B4,0x14C6,0x1647,0x1EEF,0x16C2,0x541,0x870,0x0,0x0,0x48};
#endif

#if CHUNK==32
const chunk MConst_254BNCX=0x19789E85;
const BIG_256 Modulus_254BNCX= {0x1C1B55B3,0x13311F7A,0x24FB86F,0x1FADDC30,0x166D3243,0xFB23D31,0x836C2F7,0x10E05,0x240000};
#endif

#if CHUNK==64
const chunk MConst_254BNCX=0x4E205BF9789E85;
const BIG_256 Modulus_254BNCX= {0x6623EF5C1B55B3,0xD6EE18093EE1BE,0x647A6366D3243F,0x8702A0DB0BDDF,0x24000000};
#endif

