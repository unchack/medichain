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

/* AMCL BN Curve pairing functions */

//#define HAS_MAIN

#include "pair_ZZZ.h"

/* Line function */
static void PAIR_ZZZ_line(FP12_YYY *v,ECP2_ZZZ *A,ECP2_ZZZ *B,BIG_XXX Qx,BIG_XXX Qy)
{
    ECP2_ZZZ P;
    FP2_YYY Z3,X,Y,ZZ,T,NY;
    FP4_YYY a,b,c;
    int D;
    ECP2_ZZZ_copy(&P,A);
    if (A==B)
        D=ECP2_ZZZ_dbl(A);  // check these return numbers...
    else
        D=ECP2_ZZZ_add(A,B);
    if (D<0)
    {
        /* Infinity */
        FP12_YYY_one(v);
        return;
    }

    FP2_YYY_copy(&Z3,&(A->z));
    FP4_YYY_zero(&c);
    FP2_YYY_sqr(&ZZ,&(P.z));    /* ZZ=Z^2 */
    if (D==0)
    {
        /* addition */
        ECP2_ZZZ_get(&X,&Y,B);
        FP2_YYY_mul(&T,&(P.z),&Y);  /* T=Z*Y2 */

        FP2_YYY_mul(&ZZ,&ZZ,&T);

        FP2_YYY_neg(&NY,&(P.y));
        FP2_YYY_add(&ZZ,&ZZ,&NY); /* ZZ=Z^3*Y2-Y (slope numerator) */
        FP2_YYY_pmul(&Z3,&Z3,Qy);    /* Z3*Qy */
        FP2_YYY_mul(&T,&T,&(P.x));
        FP2_YYY_mul(&X,&X,&NY);
        FP2_YYY_add(&T,&T,&X);       /* Z*Y2*X-X2*Y */
        FP4_YYY_from_FP2s(&a,&Z3,&T); /* a=[Z3*Qy,Z*Y2*X-X2*Y] */
        FP2_YYY_neg(&ZZ,&ZZ);
        FP2_YYY_pmul(&ZZ,&ZZ,Qx);
        FP4_YYY_from_FP2(&b,&ZZ);    /* b=-slope*Qx */
    }
    else
    {
        /* doubling */
        FP2_YYY_sqr(&T,&(P.x));
        FP2_YYY_imul(&T,&T,3);   /* T=3X^2 (slope numerator) */
        FP2_YYY_sqr(&Y,&(P.y));

        FP2_YYY_add(&Y,&Y,&Y);   /* Y=2Y^2 */
        FP2_YYY_mul(&Z3,&Z3,&ZZ);   /* Z3=Z3*ZZ */
        FP2_YYY_pmul(&Z3,&Z3,Qy);   /* Z3=Z3*ZZ*Qy */

        FP2_YYY_mul(&X,&(P.x),&T);
        FP2_YYY_sub(&X,&X,&Y);      /* X=X*slope-2Y^2 */
        FP4_YYY_from_FP2s(&a,&Z3,&X); /* a=[Z3*ZZ*Qy , X*slope-2Y^2] */
        FP2_YYY_neg(&T,&T);
        FP2_YYY_mul(&ZZ,&ZZ,&T);
        FP2_YYY_pmul(&ZZ,&ZZ,Qx);
        FP4_YYY_from_FP2(&b,&ZZ);    /* b=-slope*ZZ*Qx */
    }

    FP12_YYY_from_FP4s(v,&a,&b,&c);
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_ZZZ_ate(FP12_YYY *r,ECP2_ZZZ *P,ECP_ZZZ *Q)
{
    FP2_YYY X;
    BIG_XXX x,n,Qx,Qy;
    int i,nb;
    ECP2_ZZZ A;
    FP12_YYY lv;
#if PAIRING_FRIENDLY_ZZZ==BN
    ECP2_ZZZ KA;
#endif

    BIG_XXX_rcopy(Qx,CURVE_Fra_ZZZ);
    BIG_XXX_rcopy(Qy,CURVE_Frb_ZZZ);
    FP2_YYY_from_BIGs(&X,Qx,Qy);

    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);

#if PAIRING_FRIENDLY_ZZZ==BN
    BIG_XXX_pmul(n,x,6);
    BIG_XXX_dec(n,2);
#else
    BIG_XXX_copy(n,x);
#endif

    BIG_XXX_norm(n);

    ECP2_ZZZ_affine(P);
    ECP_ZZZ_affine(Q);

    BIG_XXX_copy(Qx,Q->x);
    BIG_XXX_copy(Qy,Q->y);

    ECP2_ZZZ_copy(&A,P);
    FP12_YYY_one(r);
    nb=BIG_XXX_nbits(n);

    /* Main Miller Loop */
    for (i=nb-2; i>=1; i--)
    {
        PAIR_ZZZ_line(&lv,&A,&A,Qx,Qy);
        FP12_YYY_smul(r,&lv);
        if (BIG_XXX_bit(n,i))
        {

            PAIR_ZZZ_line(&lv,&A,P,Qx,Qy);
            FP12_YYY_smul(r,&lv);
        }
        FP12_YYY_sqr(r,r);
    }

    PAIR_ZZZ_line(&lv,&A,&A,Qx,Qy);
    FP12_YYY_smul(r,&lv);

    if (BIG_XXX_parity(n))
    {
        PAIR_ZZZ_line(&lv,&A,P,Qx,Qy);
        FP12_YYY_smul(r,&lv);
    }

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_ZZZ==BN
    ECP2_ZZZ_copy(&KA,P);
    ECP2_ZZZ_frob(&KA,&X);

    ECP2_ZZZ_neg(&A);
    FP12_YYY_conj(r,r);

    PAIR_ZZZ_line(&lv,&A,&KA,Qx,Qy);
    FP12_YYY_smul(r,&lv);
    ECP2_ZZZ_frob(&KA,&X);
    ECP2_ZZZ_neg(&KA);
    PAIR_ZZZ_line(&lv,&A,&KA,Qx,Qy);
    FP12_YYY_smul(r,&lv);
#endif
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_ZZZ_double_ate(FP12_YYY *r,ECP2_ZZZ *P,ECP_ZZZ *Q,ECP2_ZZZ *R,ECP_ZZZ *S)
{
    FP2_YYY X;
    BIG_XXX x,n,Qx,Qy,Sx,Sy;
    int i,nb;
    ECP2_ZZZ A,B;
    FP12_YYY lv;
#if PAIRING_FRIENDLY_ZZZ==BN
    ECP2_ZZZ K;
#endif
    BIG_XXX_rcopy(Qx,CURVE_Fra_ZZZ);
    BIG_XXX_rcopy(Qy,CURVE_Frb_ZZZ);
    FP2_YYY_from_BIGs(&X,Qx,Qy);

    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);

#if PAIRING_FRIENDLY_ZZZ==BN
    BIG_XXX_pmul(n,x,6);
    BIG_XXX_dec(n,2);
#else
    BIG_XXX_copy(n,x);
#endif

    BIG_XXX_norm(n);

    ECP2_ZZZ_affine(P);
    ECP_ZZZ_affine(Q);

    ECP2_ZZZ_affine(R);
    ECP_ZZZ_affine(S);

    BIG_XXX_copy(Qx,Q->x);
    BIG_XXX_copy(Qy,Q->y);

    BIG_XXX_copy(Sx,S->x);
    BIG_XXX_copy(Sy,S->y);

    ECP2_ZZZ_copy(&A,P);
    ECP2_ZZZ_copy(&B,R);
    FP12_YYY_one(r);
    nb=BIG_XXX_nbits(n);

    /* Main Miller Loop */
    for (i=nb-2; i>=1; i--)
    {
        PAIR_ZZZ_line(&lv,&A,&A,Qx,Qy);
        FP12_YYY_smul(r,&lv);
        PAIR_ZZZ_line(&lv,&B,&B,Sx,Sy);
        FP12_YYY_smul(r,&lv);

        if (BIG_XXX_bit(n,i))
        {
            PAIR_ZZZ_line(&lv,&A,P,Qx,Qy);
            FP12_YYY_smul(r,&lv);

            PAIR_ZZZ_line(&lv,&B,R,Sx,Sy);
            FP12_YYY_smul(r,&lv);
        }
        FP12_YYY_sqr(r,r);
    }

    PAIR_ZZZ_line(&lv,&A,&A,Qx,Qy);
    FP12_YYY_smul(r,&lv);

    PAIR_ZZZ_line(&lv,&B,&B,Sx,Sy);
    FP12_YYY_smul(r,&lv);

    if (BIG_XXX_parity(n))
    {
        PAIR_ZZZ_line(&lv,&A,P,Qx,Qy);
        FP12_YYY_smul(r,&lv);

        PAIR_ZZZ_line(&lv,&B,R,Sx,Sy);
        FP12_YYY_smul(r,&lv);
    }

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_ZZZ==BN
    FP12_YYY_conj(r,r);

    ECP2_ZZZ_copy(&K,P);
    ECP2_ZZZ_frob(&K,&X);
    ECP2_ZZZ_neg(&A);
    PAIR_ZZZ_line(&lv,&A,&K,Qx,Qy);
    FP12_YYY_smul(r,&lv);
    ECP2_ZZZ_frob(&K,&X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_line(&lv,&A,&K,Qx,Qy);
    FP12_YYY_smul(r,&lv);

    ECP2_ZZZ_copy(&K,R);
    ECP2_ZZZ_frob(&K,&X);
    ECP2_ZZZ_neg(&B);
    PAIR_ZZZ_line(&lv,&B,&K,Sx,Sy);
    FP12_YYY_smul(r,&lv);
    ECP2_ZZZ_frob(&K,&X);
    ECP2_ZZZ_neg(&K);
    PAIR_ZZZ_line(&lv,&B,&K,Sx,Sy);
    FP12_YYY_smul(r,&lv);
#endif
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
void PAIR_ZZZ_fexp(FP12_YYY *r)
{
    FP2_YYY X;
    BIG_XXX x,a,b;
    FP12_YYY t0,y0,y1,y2,y3;

    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    BIG_XXX_rcopy(a,CURVE_Fra_ZZZ);
    BIG_XXX_rcopy(b,CURVE_Frb_ZZZ);
    FP2_YYY_from_BIGs(&X,a,b);

    /* Easy part of final exp */

    FP12_YYY_inv(&t0,r);
    FP12_YYY_conj(r,r);

    FP12_YYY_mul(r,&t0);
    FP12_YYY_copy(&t0,r);

    FP12_YYY_frob(r,&X);
    FP12_YYY_frob(r,&X);
    FP12_YYY_mul(r,&t0);

    /* Hard part of final exp - see Duquesne & Ghamman eprint 2015/192.pdf */
#if PAIRING_FRIENDLY_ZZZ==BN
    FP12_YYY_pow(&t0,r,x); // t0=f^-u
    FP12_YYY_usqr(&y3,&t0); // y3=t0^2
    FP12_YYY_copy(&y0,&t0);
    FP12_YYY_mul(&y0,&y3); // y0=t0*y3
    FP12_YYY_copy(&y2,&y3);
    FP12_YYY_frob(&y2,&X); // y2=y3^p
    FP12_YYY_mul(&y2,&y3); //y2=y2*y3
    FP12_YYY_usqr(&y2,&y2); //y2=y2^2
    FP12_YYY_mul(&y2,&y3); // y2=y2*y3

    FP12_YYY_pow(&t0,&y0,x);  //t0=y0^-u
    FP12_YYY_conj(&y0,r);     //y0=~r
    FP12_YYY_copy(&y1,&t0);
    FP12_YYY_frob(&y1,&X);
    FP12_YYY_frob(&y1,&X); //y1=t0^p^2
    FP12_YYY_mul(&y1,&y0); // y1=y0*y1
    FP12_YYY_conj(&t0,&t0); // t0=~t0
    FP12_YYY_copy(&y3,&t0);
    FP12_YYY_frob(&y3,&X); //y3=t0^p
    FP12_YYY_mul(&y3,&t0); // y3=t0*y3
    FP12_YYY_usqr(&t0,&t0); // t0=t0^2
    FP12_YYY_mul(&y1,&t0); // y1=t0*y1

    FP12_YYY_pow(&t0,&y3,x); // t0=y3^-u
    FP12_YYY_usqr(&t0,&t0); //t0=t0^2
    FP12_YYY_conj(&t0,&t0); //t0=~t0
    FP12_YYY_mul(&y3,&t0); // y3=t0*y3

    FP12_YYY_frob(r,&X);
    FP12_YYY_copy(&y0,r);
    FP12_YYY_frob(r,&X);
    FP12_YYY_mul(&y0,r);
    FP12_YYY_frob(r,&X);
    FP12_YYY_mul(&y0,r);

    FP12_YYY_usqr(r,&y3);  //r=y3^2
    FP12_YYY_mul(r,&y2);   //r=y2*r
    FP12_YYY_copy(&y3,r);
    FP12_YYY_mul(&y3,&y0); // y3=r*y0
    FP12_YYY_mul(r,&y1); // r=r*y1
    FP12_YYY_usqr(r,r); // r=r^2
    FP12_YYY_mul(r,&y3); // r=r*y3
    FP12_YYY_reduce(r);
#else
// Ghamman & Fouotsa Method

    FP12_YYY_usqr(&y0,r);
    FP12_YYY_pow(&y1,&y0,x);
    BIG_XXX_fshr(x,1);
    FP12_YYY_pow(&y2,&y1,x);
    BIG_XXX_fshl(x,1); // x must be even
    FP12_YYY_conj(&y3,r);
    FP12_YYY_mul(&y1,&y3);

    FP12_YYY_conj(&y1,&y1);
    FP12_YYY_mul(&y1,&y2);

    FP12_YYY_pow(&y2,&y1,x);

    FP12_YYY_pow(&y3,&y2,x);
    FP12_YYY_conj(&y1,&y1);
    FP12_YYY_mul(&y3,&y1);

    FP12_YYY_conj(&y1,&y1);
    FP12_YYY_frob(&y1,&X);
    FP12_YYY_frob(&y1,&X);
    FP12_YYY_frob(&y1,&X);
    FP12_YYY_frob(&y2,&X);
    FP12_YYY_frob(&y2,&X);
    FP12_YYY_mul(&y1,&y2);

    FP12_YYY_pow(&y2,&y3,x);
    FP12_YYY_mul(&y2,&y0);
    FP12_YYY_mul(&y2,r);

    FP12_YYY_mul(&y1,&y2);
    FP12_YYY_copy(&y2,&y3);
    FP12_YYY_frob(&y2,&X);
    FP12_YYY_mul(&y1,&y2);
    FP12_YYY_copy(r,&y1);
    FP12_YYY_reduce(r);

// Aranha et al method as described by Ghamman & Fouotsa
    /*
    	FP12_YYY_usqr(&y0,r);  // t0=f^2
    	FP12_YYY_conj(&y3,&y0); // t0=f^-2
    	FP12_YYY_pow(&t0,r,x); // t5=f^u
    	FP12_YYY_usqr(&y1,&t0); // t1=t5^2
    	FP12_YYY_mul(&y3,&t0); // t3=t0*t5

    	FP12_YYY_pow(&y0,&y3,x);

    	FP12_YYY_pow(&y2,&y0,x);

    	FP12_YYY_pow(&y4,&y2,x);

    	FP12_YYY_mul(&y4,&y1);
    	FP12_YYY_pow(&y1,&y4,x);
    	FP12_YYY_conj(&y3,&y3);
    	FP12_YYY_mul(&y1,&y3);
    	FP12_YYY_mul(&y1,r);

    	FP12_YYY_conj(&y3,r);
    	FP12_YYY_mul(&y0,r);
    	FP12_YYY_frob(&y0,&X); FP12_YYY_frob(&y0,&X); FP12_YYY_frob(&y0,&X);

    	FP12_YYY_mul(&y4,&y3);
    	FP12_YYY_frob(&y4,&X);

    	FP12_YYY_mul(&t0,&y2);
    	FP12_YYY_frob(&t0,&X); FP12_YYY_frob(&t0,&X);

    	FP12_YYY_mul(&t0,&y0);
    	FP12_YYY_mul(&t0,&y4);
    	FP12_YYY_mul(&t0,&y1);
    	FP12_YYY_copy(r,&t0);
    	FP12_YYY_reduce(r);*/

//-----------------------------------
    /*
    	FP12_YYY_copy(&y0,r);						// y0=r;
    	FP12_YYY_copy(&y1,r);						// y1=r;
    	FP12_YYY_copy(&t0,r); FP12_YYY_frob(&t0,&X);	// t0=Frobenius(r,X,1);
    	FP12_YYY_conj(&y3,&t0); FP12_YYY_mul(&y1,&y3);	// y1*=inverse(t0);
    	FP12_YYY_frob(&t0,&X); FP12_YYY_frob(&t0,&X);	// t0=Frobenius(t0,X,2);
    	FP12_YYY_mul(&y1,&t0);						// y1*=t0;

    	FP12_YYY_pow(r,r,x);						// r=pow(r,x);
    	FP12_YYY_conj(&y3,r); FP12_YYY_mul(&y1,&y3);	// y1*=inverse(r);
    	FP12_YYY_copy(&t0,r); FP12_YYY_frob(&t0,&X);	// t0=Frobenius(r,X,1);
    	FP12_YYY_mul(&y0,&t0);						// y0*=t0;
    	FP12_YYY_frob(&t0,&X);						// t0=Frobenius(t0,X,1);
    	FP12_YYY_mul(&y1,&t0);						// y1*=t0;
    	FP12_YYY_frob(&t0,&X);						// t0=Frobenius(t0,X,1);
    	FP12_YYY_conj(&y3,&t0); FP12_YYY_mul(&y0,&y3);	// y0*=inverse(t0);

    	FP12_YYY_pow(r,r,x);						// r=pow(r,x);
    	FP12_YYY_mul(&y0,r);						// y0*=r;
    	FP12_YYY_copy(&t0,r); FP12_YYY_frob(&t0,&X); FP12_YYY_frob(&t0,&X); // t0=Frobenius(r,X,2);
    	FP12_YYY_conj(&y3,&t0); FP12_YYY_mul(&y0,&y3);	// y0*=inverse(t0);
    	FP12_YYY_frob(&t0,&X);						// t0=Frobenius(t0,X,1);
    	FP12_YYY_mul(&y1,&t0);						// y1*=t0;

    	FP12_YYY_pow(r,r,x);						// r=pow(r,x);			// r^x3
    	FP12_YYY_copy(&t0,r); FP12_YYY_frob(&t0,&X);	// t0=Frobenius(r,X,1);
    	FP12_YYY_conj(&y3,&t0); FP12_YYY_mul(&y0,&y3);	// y0*=inverse(t0);
    	FP12_YYY_frob(&t0,&X);						// t0=Frobenius(t0,X,1);
    	FP12_YYY_mul(&y1,&t0);						// y1*=t0;

    	FP12_YYY_pow(r,r,x);						// r=pow(r,x);			// r^x4
    	FP12_YYY_conj(&y3,r); FP12_YYY_mul(&y0,&y3);	// y0*=inverse(r);
    	FP12_YYY_copy(&t0,r); FP12_YYY_frob(&t0,&X);	// t0=Frobenius(r,X,1);
    	FP12_YYY_mul(&y1,&t0);						//y1*=t0;

    	FP12_YYY_pow(r,r,x);						// r=pow(r,x);			// r^x5
    	FP12_YYY_mul(&y1,r);						// y1*=r;

    	FP12_YYY_usqr(&y0,&y0);						// r=y0*y0*y1;
    	FP12_YYY_mul(&y0,&y1);
    	FP12_YYY_copy(r,&y0);
    	FP12_YYY_reduce(r); */
#endif
}

#ifdef USE_GLV_ZZZ
/* GLV method */
static void glv(BIG_XXX u[2],BIG_XXX e)
{
#if PAIRING_FRIENDLY_ZZZ==BN
    int i,j;
    BIG_XXX v[2],t,q;
    DBIG_XXX d;
    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    for (i=0; i<2; i++)
    {
        BIG_XXX_rcopy(t,CURVE_W_ZZZ[i]);
//BIG_XXX_norm(t); BIG_XXX_norm(e);
        BIG_XXX_mul(d,t,e);
        BIG_XXX_ddiv(v[i],d,q);
        BIG_XXX_zero(u[i]);
    }
    BIG_XXX_copy(u[0],e);
    for (i=0; i<2; i++)
        for (j=0; j<2; j++)
        {
            BIG_XXX_rcopy(t,CURVE_SB_ZZZ[j][i]);
            BIG_XXX_modmul(t,v[j],t,q);
            BIG_XXX_add(u[i],u[i],q);
            BIG_XXX_sub(u[i],u[i],t);
            BIG_XXX_mod(u[i],q);
        }

#else
// -(x^2).P = (Beta.x,y)

    BIG_XXX x,x2,q;
    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    BIG_XXX_smul(x2,x,x);
    BIG_XXX_copy(u[0],e);
    BIG_XXX_mod(u[0],x2);
    BIG_XXX_copy(u[1],e);
    BIG_XXX_sdiv(u[1],x2);

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    BIG_XXX_sub(u[1],q,u[1]);

#endif

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_XXX u[4],BIG_XXX e)
{
    int i;
#if PAIRING_FRIENDLY_ZZZ==BN
    int j;
    BIG_XXX v[4],t,q;
    DBIG_XXX d;
    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    for (i=0; i<4; i++)
    {
        BIG_XXX_rcopy(t,CURVE_WB_ZZZ[i]);
//BIG_XXX_norm(t); BIG_XXX_norm(e);
        BIG_XXX_mul(d,t,e);
        BIG_XXX_ddiv(v[i],d,q);
        BIG_XXX_zero(u[i]);
    }

    BIG_XXX_copy(u[0],e);
    for (i=0; i<4; i++)
        for (j=0; j<4; j++)
        {
            BIG_XXX_rcopy(t,CURVE_BB_ZZZ[j][i]);
            BIG_XXX_modmul(t,v[j],t,q);
            BIG_XXX_add(u[i],u[i],q);
            BIG_XXX_sub(u[i],u[i],t);
            BIG_XXX_mod(u[i],q);
        }

#else

    BIG_XXX x,w;
    BIG_XXX_rcopy(x,CURVE_Bnx_ZZZ);
    BIG_XXX_copy(w,e);

    for (i=0; i<4; i++)
    {
        BIG_XXX_copy(u[i],w);
        BIG_XXX_mod(u[i],x);
        BIG_XXX_sdiv(w,x);
    }

#endif
    return;
}

/* Multiply P by e in group G1 */
void PAIR_ZZZ_G1mul(ECP_ZZZ *P,BIG_XXX e)
{
#ifdef USE_GLV_ZZZ   /* Note this method is patented */
    int np,nn;
    ECP_ZZZ Q;
    BIG_XXX cru,t,q;
    BIG_XXX u[2];

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ);
    glv(u,e);

    ECP_ZZZ_affine(P);
    ECP_ZZZ_copy(&Q,P);
    BIG_XXX_rcopy(cru,CURVE_Cru_ZZZ);
    FP_YYY_nres(cru);
    FP_YYY_mul(Q.x,Q.x,cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np=BIG_XXX_nbits(u[0]);
    BIG_XXX_modneg(t,u[0],q);
    nn=BIG_XXX_nbits(t);
    if (nn<np)
    {
        BIG_XXX_copy(u[0],t);
        ECP_ZZZ_neg(P);
    }

    np=BIG_XXX_nbits(u[1]);
    BIG_XXX_modneg(t,u[1],q);
    nn=BIG_XXX_nbits(t);
    if (nn<np)
    {
        BIG_XXX_copy(u[1],t);
        ECP_ZZZ_neg(&Q);
    }

    ECP_ZZZ_mul2(P,&Q,u[0],u[1]);

#else
    ECP_ZZZ_mul(P,e);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_ZZZ_G2mul(ECP2_ZZZ *P,BIG_XXX e)
{
#ifdef USE_GS_G2_ZZZ   /* Well I didn't patent it :) */
    int i,np,nn;
    ECP2_ZZZ Q[4];
    FP2_YYY X;
    BIG_XXX x,y;
    BIG_XXX u[4];

    BIG_XXX_rcopy(x,CURVE_Fra_ZZZ);
    BIG_XXX_rcopy(y,CURVE_Frb_ZZZ);
    FP2_YYY_from_BIGs(&X,x,y);

    BIG_XXX_rcopy(y,CURVE_Order_ZZZ);
    gs(u,e);


    ECP2_ZZZ_affine(P);

    ECP2_ZZZ_copy(&Q[0],P);
    for (i=1; i<4; i++)
    {
        ECP2_ZZZ_copy(&Q[i],&Q[i-1]);
        ECP2_ZZZ_frob(&Q[i],&X);
    }

    for (i=0; i<4; i++)
    {
        np=BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(x,u[i],y);
        nn=BIG_XXX_nbits(x);
        if (nn<np)
        {
            BIG_XXX_copy(u[i],x);
            ECP2_ZZZ_neg(&Q[i]);
        }
    }

    ECP2_ZZZ_mul4(P,Q,u);

#else
    ECP2_ZZZ_mul(P,e);
#endif
}

/* f=f^e */
void PAIR_ZZZ_GTpow(FP12_YYY *f,BIG_XXX e)
{
#ifdef USE_GS_GT_ZZZ   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see fp4.c */
    int i,np,nn;
    FP12_YYY g[4];
    FP2_YYY X;
    BIG_XXX t,q,x,y;
    BIG_XXX u[4];

    BIG_XXX_rcopy(x,CURVE_Fra_ZZZ);
    BIG_XXX_rcopy(y,CURVE_Frb_ZZZ);
    FP2_YYY_from_BIGs(&X,x,y);

    BIG_XXX_rcopy(q,CURVE_Order_ZZZ
		);
    gs(u,e);

    FP12_YYY_copy(&g[0],f);
    for (i=1; i<4; i++)
    {
        FP12_YYY_copy(&g[i],&g[i-1]);
        FP12_YYY_frob(&g[i],&X);
    }

    for (i=0; i<4; i++)
    {
        np=BIG_XXX_nbits(u[i]);
        BIG_XXX_modneg(t,u[i],q);
        nn=BIG_XXX_nbits(t);
        if (nn<np)
        {
            BIG_XXX_copy(u[i],t);
            FP12_YYY_conj(&g[i],&g[i]);
        }
    }
    FP12_YYY_pow4(f,g,u);

#else
    FP12_YYY_pow(f,f,e);
#endif
}

/* test group membership test - no longer needed */
/* with GT-Strong curve, now only check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */

/*
int PAIR_ZZZ_GTmember(FP12_YYY *m)
{
	BIG_XXX a,b;
	FP2_YYY X;
	FP12_YYY r,w;
	if (FP12_YYY_isunity(m)) return 0;
	FP12_YYY_conj(&r,m);
	FP12_YYY_mul(&r,m);
	if (!FP12_YYY_isunity(&r)) return 0;

	BIG_XXX_rcopy(a,CURVE_Fra);
	BIG_XXX_rcopy(b,CURVE_Frb);
	FP2_YYY_from_BIGs(&X,a,b);


	FP12_YYY_copy(&r,m); FP12_YYY_frob(&r,&X); FP12_YYY_frob(&r,&X);
	FP12_YYY_copy(&w,&r); FP12_YYY_frob(&w,&X); FP12_YYY_frob(&w,&X);
	FP12_YYY_mul(&w,m);


#ifndef GT_STRONG
	if (!FP12_YYY_equals(&w,&r)) return 0;

	BIG_XXX_rcopy(a,CURVE_Bnx);

	FP12_YYY_copy(&r,m); FP12_YYY_pow(&w,&r,a); FP12_YYY_pow(&w,&w,a);
	FP12_YYY_sqr(&r,&w); FP12_YYY_mul(&r,&w); FP12_YYY_sqr(&r,&r);

	FP12_YYY_copy(&w,m); FP12_YYY_frob(&w,&X);
 #endif

	return FP12_YYY_equals(&w,&r);
}

*/


#ifdef HAS_MAIN
/*
#if CHOICE==BN254_T

const BIG_XXX TEST_Gx={0x18AFF11A,0xF2EF406,0xAF68220,0x171F2E27,0x6BA0959,0x124C50E0,0x450BE27,0x7003EA8,0x8A914};
const BIG_XXX TEST_Gy={0x6E010F4,0xA71D07E,0x7ECADA8,0x8260E8E,0x1F79C328,0x17A09412,0xBFAE690,0x1C57CBD1,0x17DF54};

const BIG_XXX TEST_Pxa={0x1047D566,0xD83CD71,0x10322E9D,0x991FA93,0xA282C48,0x18AEBEC8,0xCB05850,0x13B4F669,0x21794A};
const BIG_XXX TEST_Pxb={0x1E305936,0x16885BF1,0x327060,0xE26F794,0x1547D870,0x1963E5B2,0x1BEBB96C,0x988A33C,0x1A9B47};
const BIG_XXX TEST_Pya={0x20FF876,0x4427E67,0x18732211,0xE88E45E,0x174D1A7E,0x17D877ED,0x343AB37,0x97EB453,0xB00D5};
const BIG_XXX TEST_Pyb={0x1D746B7B,0x732F4C2,0x122A49B0,0x16267985,0x235DF56,0x10B1E4D,0x14D8F210,0x17A05C3E,0x5ECF8};

#endif

#if CHOICE==BN254_T2

const BIG_XXX TEST_Gx={0x15488765,0x46790D7,0xD9900A,0x1DFB43F,0x9F2D307,0xC4724E8,0x5678E51,0x15C3E3A7,0x1BEC8E};
const BIG_XXX TEST_Gy={0x3D3273C,0x1AFA5FF,0x1880A139,0xACD34DF,0x17493067,0x10FA4103,0x1D4C9766,0x1A73F3DB,0x2D148};

const BIG_XXX TEST_Pxa={0xF8DC275,0xAC27FA,0x11815151,0x152691C8,0x5CDEBF1,0x7D5A965,0x1BF70CE3,0x679A1C8,0xD62CF};
const BIG_XXX TEST_Pxb={0x1D17D7A8,0x6B28DF4,0x174A0389,0xFE67E5F,0x1FA97A3C,0x7F5F473,0xFFB5146,0x4BC19A5,0x227010};
const BIG_XXX TEST_Pya={0x16CC1F90,0x5284627,0x171B91AB,0x11F843B9,0x1D468755,0x67E279C,0x19FE0EF8,0x1A0CAA6B,0x1CC6CB};
const BIG_XXX TEST_Pyb={0x1FF0CF2A,0xBC83255,0x6DD6EE8,0xB8B752F,0x13E484EC,0x1809BE81,0x1A648AA1,0x8CEF3F3,0x86EE};


#endif

#if CHOICE==BN254

const BIG_XXX TEST_Gx={0x14BEC4670E4EB7,0xEA2973860F6861,0x35C14B2FC3C28F,0x4402A0B63B9473,0x2074A81D};
const BIG_XXX TEST_Gy={0xC284846631CBEB,0x34A6E8D871B3B,0x89FB94A82B2006,0x87B20038771FC,0x6A41108};

const BIG_XXX TEST_Pxa={0xE4A00F52183C77,0x554E02DF4F8354,0xB65EB5CF1C2F89,0x8B71A87BFCFC9,0x49EEDB1};
const BIG_XXX TEST_Pxb={0xCFB8FA9AA8845D,0x8A9CC76D966697,0x185BA05BF5EC08,0x76140E87D97226,0x1FB93AB6};
const BIG_XXX TEST_Pya={0x3644CC1EDF208A,0xA637FB3FF8E257,0x4453DA2BB9E686,0xD14AD3CDF6A1FE,0xCD04A1E};
const BIG_XXX TEST_Pyb={0x71BD7630A43C14,0x1CAA9F14EA264E,0x3C3C2DFC765DEF,0xCF59D1A1A7D6EE,0x11FF7795};


#endif
*/
int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG_XXX xa,xb,ya,yb,w,a,b,t1,q,u[2],v[4],m,r;
    ECP2_ZZZ P,G;
    ECP_ZZZ Q,R;
    FP12_YYY g,gp;
    FP4_YYY t,c,cp,cpm1,cpm2;
    FP2_YYY x,y,X;


    BIG_XXX_rcopy(a,CURVE_Fra);
    BIG_XXX_rcopy(b,CURVE_Frb);
    FP2_YYY_from_BIGs(&X,a,b);

    BIG_XXX_rcopy(xa,CURVE_Gx);
    BIG_XXX_rcopy(ya,CURVE_Gy);

    ECP_ZZZ_set(&Q,xa,ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_ZZZ_output(&Q);
    printf("\n");

//	BIG_XXX_rcopy(r,CURVE_Order); BIG_XXX_dec(r,7); BIG_XXX_norm(r);
    BIG_XXX_rcopy(xa,CURVE_Pxa);
    BIG_XXX_rcopy(xb,CURVE_Pxb);
    BIG_XXX_rcopy(ya,CURVE_Pya);
    BIG_XXX_rcopy(yb,CURVE_Pyb);

    FP2_YYY_from_BIGs(&x,xa,xb);
    FP2_YYY_from_BIGs(&y,ya,yb);

    ECP2_ZZZ_set(&P,&x,&y);
    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    printf("P= ");
    ECP2_ZZZ_output(&P);
    printf("\n");

    for (i=0; i<1000; i++ )
    {

        PAIR_ZZZ_ate(&g,&P,&Q);
        PAIR_ZZZ_fexp(&g);

//	PAIR_ZZZ_GTpow(&g,xa);

    }
    printf("g= ");
    FP12_YYY_output(&g);
    printf("\n");

}

#endif
