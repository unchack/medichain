/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ecdh.h"
#include "randapi.h"

int ecdh(csprng *RNG)
{
    int i,res;
    unsigned long ran;
    char *pp="M0ng00se";
    /* These octets are automatically protected against buffer overflow attacks */
    /* Note salt must be big enough to include an appended word */
    /* Note ECIES ciphertext C must be big enough to include at least 1 appended block */
    /* Recall EFS is field size in bytes. So EFS=32 for 256-bit curve */
    char s0[2*EGS],s1[EGS],w0[2*EFS+1],w1[2*EFS+1],z0[EFS],z1[EFS],key[EAS],salt[40],pw[40];
    octet S0= {0,sizeof(s0),s0};
    octet S1= {0,sizeof(s1),s1};
    octet W0= {0,sizeof(w0),w0};
    octet W1= {0,sizeof(w1),w1};
    octet Z0= {0,sizeof(z0),z0};
    octet Z1= {0,sizeof(z1),z1};
    octet KEY= {0,sizeof(key),key};
    octet SALT= {0,sizeof(salt),salt};
    octet PW= {0,sizeof(pw),pw};

    SALT.len=8;
    for (i=0; i<8; i++) SALT.val[i]=i+1; // set Salt

    printf("Alice's Passphrase= %s\n",pp);

    OCT_empty(&PW);
    OCT_jstring(&PW,pp);   // set Password from string

    /* private key S0 of size EGS bytes derived from Password and Salt */

    PBKDF2(HASH_TYPE_ECC,&PW,&SALT,1000,EGS,&S0);

    printf("Alices private key= 0x");
    OCT_output(&S0);

    /* Generate Key pair S/W */

    ECP_KEY_PAIR_GENERATE(NULL,&S0,&W0);
    printf("Alices public key= 0x");
    OCT_output(&W0);

    res=ECP_PUBLIC_KEY_VALIDATE(1,&W0);
    if (res!=0)
    {
        printf("ECP Public Key is invalid!\n");
        return 0;
    }

    /* Random private key for other party */
    ECP_KEY_PAIR_GENERATE(RNG,&S1,&W1);
    res=ECP_PUBLIC_KEY_VALIDATE(1,&W1);
    if (res!=0)
    {
        printf("ECP Public Key is invalid!\n");
        return 0;
    }
    printf("Servers private key= 0x");
    OCT_output(&S1);
    printf("Servers public key= 0x");
    OCT_output(&W1);

    /* Calculate common key using DH - IEEE 1363 method */

    ECPSVDP_DH(&S0,&W1,&Z0);
    ECPSVDP_DH(&S1,&W0,&Z1);

    if (!OCT_comp(&Z0,&Z1))
    {
        printf("*** ECPSVDP-DH Failed\n");
        return 0;
    }

    KDF2(HASH_TYPE_ECC,&Z0,NULL,EAS,&KEY);

    printf("Alice's DH Key=  0x");
    OCT_output(&KEY);
    printf("Servers DH Key=  0x");
    OCT_output(&KEY);

#if CURVETYPE != MONTGOMERY

    char ds[EGS],p1[30],p2[30],v[2*EFS+1],m[32],c[64],t[32],cs[EGS];
    octet DS= {0,sizeof(ds),ds};
    octet CS= {0,sizeof(cs),cs};
    octet P1= {0,sizeof(p1),p1};
    octet P2= {0,sizeof(p2),p2};
    octet V= {0,sizeof(v),v};
    octet M= {0,sizeof(m),m};
    octet C= {0,sizeof(c),c};
    octet T= {0,sizeof(t),t};

    printf("Testing ECIES\n");

    P1.len=3;
    P1.val[0]=0x0;
    P1.val[1]=0x1;
    P1.val[2]=0x2;
    P2.len=4;
    P2.val[0]=0x0;
    P2.val[1]=0x1;
    P2.val[2]=0x2;
    P2.val[3]=0x3;

    M.len=17;
    for (i=0; i<=16; i++) M.val[i]=i;

    ECP_ECIES_ENCRYPT(HASH_TYPE_ECC,&P1,&P2,RNG,&W1,&M,12,&V,&C,&T);

    printf("Ciphertext= \n");
    printf("V= 0x");
    OCT_output(&V);
    printf("C= 0x");
    OCT_output(&C);
    printf("T= 0x");
    OCT_output(&T);

    if (!ECP_ECIES_DECRYPT(HASH_TYPE_ECC,&P1,&P2,&V,&C,&T,&S1,&M))
    {
        printf("*** ECIES Decryption Failed\n");
        return 0;
    }
    else printf("Decryption succeeded\n");

    printf("Message is 0x");
    OCT_output(&M);


    printf("Testing ECDSA\n");

    if (ECPSP_DSA(HASH_TYPE_ECC,RNG,NULL,&S0,&M,&CS,&DS)!=0)
    {
        printf("***ECDSA Signature Failed\n");
        return 0;
    }

    printf("Signature C = 0x");
    OCT_output(&CS);
    printf("Signature D = 0x");
    OCT_output(&DS);

    if (ECPVP_DSA(HASH_TYPE_ECC,&W0,&M,&CS,&DS)!=0)
    {
        printf("***ECDSA Verification Failed\n");
        return 0;
    }
    else 
    {
      printf("ECDSA Signature/Verification succeeded\n");
    }

#endif

    return 0;
}

int main()
{
    int i,res;
    unsigned long ran;

	char raw[100];
    octet RAW= {0,sizeof(raw),raw};
    csprng RNG;                /* Crypto Strong RNG */

    time((time_t *)&ran);

    RAW.len=100;				/* fake random seed source */
    RAW.val[0]=ran;
    RAW.val[1]=ran>>8;
    RAW.val[2]=ran>>16;
    RAW.val[3]=ran>>24;
    for (i=0; i<100; i++) RAW.val[i]=i+1;

    CREATE_CSPRNG(&RNG,&RAW);   /* initialise strong RNG */

	ecdh(&RNG);

	KILL_CSPRNG(&RNG);
}

