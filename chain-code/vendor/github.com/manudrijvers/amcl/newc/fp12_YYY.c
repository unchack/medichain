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

/* AMCL Fp^12 functions */
/* SU=m, m is Stack Usage (no lazy )*/
/* FP12 elements are of the form a+i.b+i^2.c */

#include "fp12_YYY.h"

/* test x==0 ? */
/* SU= 8 */
int FP12_YYY_iszilch(FP12_YYY *x)
{
    if (FP4_YYY_iszilch(&(x->a)) && FP4_YYY_iszilch(&(x->b)) && FP4_YYY_iszilch(&(x->c))) return 1;
    return 0;
}

/* test x==1 ? */
/* SU= 8 */
int FP12_YYY_isunity(FP12_YYY *x)
{
    if (FP4_YYY_isunity(&(x->a)) && FP4_YYY_iszilch(&(x->b)) && FP4_YYY_iszilch(&(x->c))) return 1;
    return 0;
}

/* FP12 copy w=x */
/* SU= 16 */
void FP12_YYY_copy(FP12_YYY *w,FP12_YYY *x)
{
    if (x==w) return;
    FP4_YYY_copy(&(w->a),&(x->a));
    FP4_YYY_copy(&(w->b),&(x->b));
    FP4_YYY_copy(&(w->c),&(x->c));
}

/* FP12 w=1 */
/* SU= 8 */
void FP12_YYY_one(FP12_YYY *w)
{
    FP4_YYY_one(&(w->a));
    FP4_YYY_zero(&(w->b));
    FP4_YYY_zero(&(w->c));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP12_YYY_equals(FP12_YYY *x,FP12_YYY *y)
{
    if (FP4_YYY_equals(&(x->a),&(y->a)) && FP4_YYY_equals(&(x->b),&(y->b)) && FP4_YYY_equals(&(x->b),&(y->b)))
        return 1;
    return 0;
}

/* Set w=conj(x) */
/* SU= 8 */
void FP12_YYY_conj(FP12_YYY *w,FP12_YYY *x)
{
    FP12_YYY_copy(w,x);
    FP4_YYY_conj(&(w->a),&(w->a));
    FP4_YYY_nconj(&(w->b),&(w->b));
    FP4_YYY_conj(&(w->c),&(w->c));
}

/* Create FP12 from FP4 */
/* SU= 8 */
void FP12_YYY_from_FP4(FP12_YYY *w,FP4_YYY *a)
{
    FP4_YYY_copy(&(w->a),a);
    FP4_YYY_zero(&(w->b));
    FP4_YYY_zero(&(w->c));
}

/* Create FP12 from 3 FP4's */
/* SU= 16 */
void FP12_YYY_from_FP4s(FP12_YYY *w,FP4_YYY *a,FP4_YYY *b,FP4_YYY *c)
{
    FP4_YYY_copy(&(w->a),a);
    FP4_YYY_copy(&(w->b),b);
    FP4_YYY_copy(&(w->c),c);
}

/* Granger-Scott Unitary Squaring. This does not benefit from lazy reduction */
/* SU= 600 */
void FP12_YYY_usqr(FP12_YYY *w,FP12_YYY *x)
{
    FP4_YYY A,B,C,D;

    FP4_YYY_copy(&A,&(x->a));

    FP4_YYY_sqr(&(w->a),&(x->a));
    FP4_YYY_add(&D,&(w->a),&(w->a));
    FP4_YYY_add(&(w->a),&D,&(w->a));

    FP4_YYY_norm(&(w->a));
    FP4_YYY_nconj(&A,&A);

    FP4_YYY_add(&A,&A,&A);
    FP4_YYY_add(&(w->a),&(w->a),&A);
    FP4_YYY_sqr(&B,&(x->c));
    FP4_YYY_times_i(&B);

    FP4_YYY_add(&D,&B,&B);
    FP4_YYY_add(&B,&B,&D);
    FP4_YYY_norm(&B);

    FP4_YYY_sqr(&C,&(x->b));

    FP4_YYY_add(&D,&C,&C);
    FP4_YYY_add(&C,&C,&D);

    FP4_YYY_norm(&C);
    FP4_YYY_conj(&(w->b),&(x->b));
    FP4_YYY_add(&(w->b),&(w->b),&(w->b));
    FP4_YYY_nconj(&(w->c),&(x->c));

    FP4_YYY_add(&(w->c),&(w->c),&(w->c));
    FP4_YYY_add(&(w->b),&B,&(w->b));
    FP4_YYY_add(&(w->c),&C,&(w->c));
    FP12_YYY_reduce(w);	    /* reduce here as in pow function repeated squarings would trigger multiple reductions */

}

/* FP12 squaring w=x^2 */
/* SU= 600 */
void FP12_YYY_sqr(FP12_YYY *w,FP12_YYY *x)
{
    /* Use Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */

    FP4_YYY A,B,C,D;

    FP4_YYY_sqr(&A,&(x->a));
    FP4_YYY_mul(&B,&(x->b),&(x->c));
    FP4_YYY_add(&B,&B,&B);
    FP4_YYY_sqr(&C,&(x->c));
    FP4_YYY_mul(&D,&(x->a),&(x->b));
    FP4_YYY_add(&D,&D,&D);
    FP4_YYY_add(&(w->c),&(x->a),&(x->c));
    FP4_YYY_add(&(w->c),&(x->b),&(w->c));

    FP4_YYY_sqr(&(w->c),&(w->c));

    FP4_YYY_copy(&(w->a),&A);

    FP4_YYY_add(&A,&A,&B);

    FP4_YYY_norm(&A);

    FP4_YYY_add(&A,&A,&C);
    FP4_YYY_add(&A,&A,&D);

    FP4_YYY_norm(&A);

    FP4_YYY_neg(&A,&A);
    FP4_YYY_times_i(&B);
    FP4_YYY_times_i(&C);

    FP4_YYY_add(&(w->a),&(w->a),&B);
    FP4_YYY_add(&(w->b),&C,&D);
    FP4_YYY_add(&(w->c),&(w->c),&A);

    FP12_YYY_norm(w);
}

/* FP12 full multiplication w=w*y */


/* SU= 896 */
/* FP12 full multiplication w=w*y */
void FP12_YYY_mul(FP12_YYY *w,FP12_YYY *y)
{
    FP4_YYY z0,z1,z2,z3,t0,t1;

    FP4_YYY_mul(&z0,&(w->a),&(y->a));
    FP4_YYY_mul(&z2,&(w->b),&(y->b));  //

    FP4_YYY_add(&t0,&(w->a),&(w->b));
    FP4_YYY_add(&t1,&(y->a),&(y->b));  //
    FP4_YYY_mul(&z1,&t0,&t1);
    FP4_YYY_add(&t0,&(w->b),&(w->c));

    FP4_YYY_add(&t1,&(y->b),&(y->c));  //
    FP4_YYY_mul(&z3,&t0,&t1);

    FP4_YYY_neg(&t0,&z0);
    FP4_YYY_neg(&t1,&z2);

    FP4_YYY_add(&z1,&z1,&t0);   // z1=z1-z0

    FP4_YYY_norm(&z1);

    FP4_YYY_add(&(w->b),&z1,&t1);
// z1=z1-z2
    FP4_YYY_add(&z3,&z3,&t1);        // z3=z3-z2
    FP4_YYY_add(&z2,&z2,&t0);        // z2=z2-z0

    FP4_YYY_add(&t0,&(w->a),&(w->c));

    FP4_YYY_add(&t1,&(y->a),&(y->c));
    FP4_YYY_mul(&t0,&t1,&t0);
    FP4_YYY_add(&z2,&z2,&t0);

    FP4_YYY_mul(&t0,&(w->c),&(y->c));
    FP4_YYY_neg(&t1,&t0);

    FP4_YYY_norm(&z2);
    FP4_YYY_norm(&z3);
    FP4_YYY_norm(&(w->b));

    FP4_YYY_add(&(w->c),&z2,&t1);
    FP4_YYY_add(&z3,&z3,&t1);
    FP4_YYY_times_i(&t0);
    FP4_YYY_add(&(w->b),&(w->b),&t0);

    FP4_YYY_times_i(&z3);
    FP4_YYY_add(&(w->a),&z0,&z3);

    FP12_YYY_norm(w);
}

/* FP12 multiplication w=w*y */
/* SU= 744 */
/* catering for special case that arises from special form of ATE pairing line function */
void FP12_YYY_smul(FP12_YYY *w,FP12_YYY *y)
{
    FP4_YYY z0,z2,z3,t0,t1;

    FP4_YYY_copy(&z3,&(w->b));
    FP4_YYY_mul(&z0,&(w->a),&(y->a));
    FP4_YYY_pmul(&z2,&(w->b),&(y->b).a);
    FP4_YYY_add(&(w->b),&(w->a),&(w->b));
    FP4_YYY_copy(&t1,&(y->a));
    FP2_YYY_add(&t1.a,&t1.a,&(y->b).a);

    FP4_YYY_mul(&(w->b),&(w->b),&t1);
    FP4_YYY_add(&z3,&z3,&(w->c));
    FP4_YYY_pmul(&z3,&z3,&(y->b).a);
    FP4_YYY_neg(&t0,&z0);
    FP4_YYY_neg(&t1,&z2);

    FP4_YYY_add(&(w->b),&(w->b),&t0);   // z1=z1-z0

    FP4_YYY_norm(&(w->b));

    FP4_YYY_add(&(w->b),&(w->b),&t1);   // z1=z1-z2

    FP4_YYY_add(&z3,&z3,&t1);        // z3=z3-z2
    FP4_YYY_add(&z2,&z2,&t0);        // z2=z2-z0

    FP4_YYY_add(&t0,&(w->a),&(w->c));

    FP4_YYY_mul(&t0,&(y->a),&t0);
    FP4_YYY_add(&(w->c),&z2,&t0);

    FP4_YYY_times_i(&z3);
    FP4_YYY_add(&(w->a),&z0,&z3);

    FP12_YYY_norm(w);
}

/* Set w=1/x */
/* SU= 600 */
void FP12_YYY_inv(FP12_YYY *w,FP12_YYY *x)
{
    FP4_YYY f0,f1,f2,f3;
    FP12_YYY_norm(x);

    FP4_YYY_sqr(&f0,&(x->a));
    FP4_YYY_mul(&f1,&(x->b),&(x->c));
    FP4_YYY_times_i(&f1);
    FP4_YYY_sub(&f0,&f0,&f1);  /* y.a */

    FP4_YYY_sqr(&f1,&(x->c));
    FP4_YYY_times_i(&f1);
    FP4_YYY_mul(&f2,&(x->a),&(x->b));
    FP4_YYY_sub(&f1,&f1,&f2);  /* y.b */

    FP4_YYY_sqr(&f2,&(x->b));
    FP4_YYY_mul(&f3,&(x->a),&(x->c));
    FP4_YYY_sub(&f2,&f2,&f3);  /* y.c */

    FP4_YYY_mul(&f3,&(x->b),&f2);
    FP4_YYY_times_i(&f3);
    FP4_YYY_mul(&(w->a),&f0,&(x->a));
    FP4_YYY_add(&f3,&(w->a),&f3);
    FP4_YYY_mul(&(w->c),&f1,&(x->c));
    FP4_YYY_times_i(&(w->c));

    FP4_YYY_add(&f3,&(w->c),&f3);
    FP4_YYY_inv(&f3,&f3);

    FP4_YYY_mul(&(w->a),&f0,&f3);
    FP4_YYY_mul(&(w->b),&f1,&f3);
    FP4_YYY_mul(&(w->c),&f2,&f3);

}

/* constant time powering by small integer of max length bts */

void FP12_YYY_pinpow(FP12_YYY *r,int e,int bts)
{
    int i,b;
    FP12_YYY R[2];

    FP12_YYY_one(&R[0]);
    FP12_YYY_copy(&R[1],r);

    for (i=bts-1; i>=0; i--)
    {
        b=(e>>i)&1;
        FP12_YYY_mul(&R[1-b],&R[b]);
        FP12_YYY_usqr(&R[b],&R[b]);
    }
    FP12_YYY_copy(r,&R[0]);
}

/* SU= 528 */
/* set r=a^b */
/* Note this is simple square and multiply, so not side-channel safe */

void FP12_YYY_pow(FP12_YYY *r,FP12_YYY *a,BIG_XXX b)
{
    FP12_YYY w;
    BIG_XXX z,zilch;
    int bt;
    BIG_XXX_zero(zilch);
    BIG_XXX_norm(b);
    BIG_XXX_copy(z,b);
    FP12_YYY_copy(&w,a);
    FP12_YYY_one(r);

    while(1)
    {
        bt=BIG_XXX_parity(z);
        BIG_XXX_shr(z,1);
        if (bt)
            FP12_YYY_mul(r,&w);
        if (BIG_XXX_comp(z,zilch)==0) break;
        FP12_YYY_usqr(&w,&w);
    }

    FP12_YYY_reduce(r);
}

/* p=q0^u0.q1^u1.q2^u2.q3^u3 */
/* Timing attack secure, but not cache attack secure */

void FP12_YYY_pow4(FP12_YYY *p,FP12_YYY *q,BIG_XXX u[4])
{
    int i,j,a[4],nb,m;
    FP12_YYY g[8],c,s[2];
    BIG_XXX t[4],mt;
    sign8 w[NLEN_XXX*BASEBITS_XXX+1];

    for (i=0; i<4; i++)
        BIG_XXX_copy(t[i],u[i]);

    FP12_YYY_copy(&g[0],&q[0]);
    FP12_YYY_conj(&s[0],&q[1]);
    FP12_YYY_mul(&g[0],&s[0]);  /* P/Q */
    FP12_YYY_copy(&g[1],&g[0]);
    FP12_YYY_copy(&g[2],&g[0]);
    FP12_YYY_copy(&g[3],&g[0]);
    FP12_YYY_copy(&g[4],&q[0]);
    FP12_YYY_mul(&g[4],&q[1]);  /* P*Q */
    FP12_YYY_copy(&g[5],&g[4]);
    FP12_YYY_copy(&g[6],&g[4]);
    FP12_YYY_copy(&g[7],&g[4]);

    FP12_YYY_copy(&s[1],&q[2]);
    FP12_YYY_conj(&s[0],&q[3]);
    FP12_YYY_mul(&s[1],&s[0]);       /* R/S */
    FP12_YYY_conj(&s[0],&s[1]);
    FP12_YYY_mul(&g[1],&s[0]);
    FP12_YYY_mul(&g[2],&s[1]);
    FP12_YYY_mul(&g[5],&s[0]);
    FP12_YYY_mul(&g[6],&s[1]);
    FP12_YYY_copy(&s[1],&q[2]);
    FP12_YYY_mul(&s[1],&q[3]);      /* R*S */
    FP12_YYY_conj(&s[0],&s[1]);
    FP12_YYY_mul(&g[0],&s[0]);
    FP12_YYY_mul(&g[3],&s[1]);
    FP12_YYY_mul(&g[4],&s[0]);
    FP12_YYY_mul(&g[7],&s[1]);

    /* if power is even add 1 to power, and add q to correction */
    FP12_YYY_one(&c);

    BIG_XXX_zero(mt);
    for (i=0; i<4; i++)
    {
        if (BIG_XXX_parity(t[i])==0)
        {
            BIG_XXX_inc(t[i],1);
            BIG_XXX_norm(t[i]);
            FP12_YYY_mul(&c,&q[i]);
        }
        BIG_XXX_add(mt,mt,t[i]);
        BIG_XXX_norm(mt);
    }

    FP12_YYY_conj(&c,&c);
    nb=1+BIG_XXX_nbits(mt);

    /* convert exponent to signed 1-bit window */
    for (j=0; j<nb; j++)
    {
        for (i=0; i<4; i++)
        {
            a[i]=BIG_XXX_lastbits(t[i],2)-2;
            BIG_XXX_dec(t[i],a[i]);
            BIG_XXX_norm(t[i]);
            BIG_XXX_fshr(t[i],1);
        }
        w[j]=8*a[0]+4*a[1]+2*a[2]+a[3];
    }
    w[nb]=8*BIG_XXX_lastbits(t[0],2)+4*BIG_XXX_lastbits(t[1],2)+2*BIG_XXX_lastbits(t[2],2)+BIG_XXX_lastbits(t[3],2);
    FP12_YYY_copy(p,&g[(w[nb]-1)/2]);

    for (i=nb-1; i>=0; i--)
    {
        m=w[i]>>7;
        j=(w[i]^m)-m;  /* j=abs(w[i]) */
        j=(j-1)/2;
        FP12_YYY_copy(&s[0],&g[j]);
        FP12_YYY_conj(&s[1],&g[j]);
        FP12_YYY_usqr(p,p);
        FP12_YYY_mul(p,&s[m&1]);
    }
    FP12_YYY_mul(p,&c); /* apply correction */
    FP12_YYY_reduce(p);
}

/* Set w=w^p using Frobenius */
/* SU= 160 */
void FP12_YYY_frob(FP12_YYY *w,FP2_YYY *f)
{
    FP2_YYY f2,f3;
    FP2_YYY_sqr(&f2,f);     /* f2=f^2 */
    FP2_YYY_mul(&f3,&f2,f); /* f3=f^3 */

    FP4_YYY_frob(&(w->a),&f3);
    FP4_YYY_frob(&(w->b),&f3);
    FP4_YYY_frob(&(w->c),&f3);

    FP4_YYY_pmul(&(w->b),&(w->b),f);
    FP4_YYY_pmul(&(w->c),&(w->c),&f2);
}

/* SU= 8 */
/* normalise all components of w */
void FP12_YYY_norm(FP12_YYY *w)
{
    FP4_YYY_norm(&(w->a));
    FP4_YYY_norm(&(w->b));
    FP4_YYY_norm(&(w->c));
}

/* SU= 8 */
/* reduce all components of w */
void FP12_YYY_reduce(FP12_YYY *w)
{
    FP4_YYY_reduce(&(w->a));
    FP4_YYY_reduce(&(w->b));
    FP4_YYY_reduce(&(w->c));
}

/* trace function w=trace(x) */
/* SU= 8 */
void FP12_YYY_trace(FP4_YYY *w,FP12_YYY *x)
{
    FP4_YYY_imul(w,&(x->a),3);
    FP4_YYY_reduce(w);
}

/* SU= 8 */
/* Output w in hex */
void FP12_YYY_output(FP12_YYY *w)
{
    printf("[");
    FP4_YYY_output(&(w->a));
    printf(",");
    FP4_YYY_output(&(w->b));
    printf(",");
    FP4_YYY_output(&(w->c));
    printf("]");
}

/* SU= 64 */
/* Convert g to octet string w */
void FP12_YYY_toOctet(octet *W,FP12_YYY *g)
{
    BIG_XXX a;
    W->len=12*MODBYTES_XXX;

    BIG_XXX_copy(a,(*g).a.a.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[0]),a);
    BIG_XXX_copy(a,(*g).a.a.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).a.b.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[2*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).a.b.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[3*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).b.a.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[4*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).b.a.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[5*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).b.b.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[6*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).b.b.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[7*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).c.a.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[8*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).c.a.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[9*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).c.b.a);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[10*MODBYTES_XXX]),a);
    BIG_XXX_copy(a,(*g).c.b.b);
    FP_YYY_redc(a);
    BIG_XXX_toBytes(&(W->val[11*MODBYTES_XXX]),a);
}

/* SU= 24 */
/* Restore g from octet string w */
void FP12_YYY_fromOctet(FP12_YYY *g,octet *W)
{
    BIG_XXX_fromBytes((*g).a.a.a,&W->val[0]);
    FP_YYY_nres((*g).a.a.a);
    BIG_XXX_fromBytes((*g).a.a.b,&W->val[MODBYTES_XXX]);
    FP_YYY_nres((*g).a.a.b);
    BIG_XXX_fromBytes((*g).a.b.a,&W->val[2*MODBYTES_XXX]);
    FP_YYY_nres((*g).a.b.a);
    BIG_XXX_fromBytes((*g).a.b.b,&W->val[3*MODBYTES_XXX]);
    FP_YYY_nres((*g).a.b.b);
    BIG_XXX_fromBytes((*g).b.a.a,&W->val[4*MODBYTES_XXX]);
    FP_YYY_nres((*g).b.a.a);
    BIG_XXX_fromBytes((*g).b.a.b,&W->val[5*MODBYTES_XXX]);
    FP_YYY_nres((*g).b.a.b);
    BIG_XXX_fromBytes((*g).b.b.a,&W->val[6*MODBYTES_XXX]);
    FP_YYY_nres((*g).b.b.a);
    BIG_XXX_fromBytes((*g).b.b.b,&W->val[7*MODBYTES_XXX]);
    FP_YYY_nres((*g).b.b.b);
    BIG_XXX_fromBytes((*g).c.a.a,&W->val[8*MODBYTES_XXX]);
    FP_YYY_nres((*g).c.a.a);
    BIG_XXX_fromBytes((*g).c.a.b,&W->val[9*MODBYTES_XXX]);
    FP_YYY_nres((*g).c.a.b);
    BIG_XXX_fromBytes((*g).c.b.a,&W->val[10*MODBYTES_XXX]);
    FP_YYY_nres((*g).c.b.a);
    BIG_XXX_fromBytes((*g).c.b.b,&W->val[11*MODBYTES_XXX]);
    FP_YYY_nres((*g).c.b.b);
}

/*
int main(){
		FP2_YYY f,w0,w1;
		FP4_YYY t0,t1,t2;
		FP12_YYY w,t,lv;
		BIG_XXX a,b;
		BIG_XXX p;

		//Test w^(P^4) = w mod p^2
//		BIG_XXX_randomnum(a);
//		BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);
	BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,1); BIG_XXX_inc(b,2); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w0,a,b);

//		BIG_XXX_randomnum(a); BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);
	BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,3); BIG_XXX_inc(b,4); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w1,a,b);

		FP4_YYY_from_FP2s(&t0,&w0,&w1);
		FP4_YYY_reduce(&t0);

//		BIG_XXX_randomnum(a);
//		BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);
		BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,5); BIG_XXX_inc(b,6); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w0,a,b);

//		BIG_XXX_randomnum(a); BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);

		BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,7); BIG_XXX_inc(b,8); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w1,a,b);

		FP4_YYY_from_FP2s(&t1,&w0,&w1);
		FP4_YYY_reduce(&t1);

//		BIG_XXX_randomnum(a);
//		BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);
		BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,9); BIG_XXX_inc(b,10); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w0,a,b);

//		BIG_XXX_randomnum(a); BIG_XXX_randomnum(b);
//		BIG_XXX_mod(a,Modulus); BIG_XXX_mod(b,Modulus);
		BIG_XXX_zero(a); BIG_XXX_zero(b); BIG_XXX_inc(a,11); BIG_XXX_inc(b,12); FP_YYY_nres(a); FP_YYY_nres(b);
		FP2_YYY_from_zps(&w1,a,b);

		FP4_YYY_from_FP2s(&t2,&w0,&w1);
		FP4_YYY_reduce(&t2);

		FP12_YYY_from_FP4s(&w,&t0,&t1,&t2);

		FP12_YYY_copy(&t,&w);

		printf("w= ");
		FP12_YYY_output(&w);
		printf("\n");

		BIG_XXX_rcopy(p,Modulus);
		//BIG_XXX_zero(p); BIG_XXX_inc(p,7);

		FP12_YYY_pow(&w,&w,p);

		printf("w^p= ");
		FP12_YYY_output(&w);
		printf("\n");

		FP2_YYY_gfc(&f,12);
		FP12_YYY_frob(&t,&f);
		printf("w^p= ");
		FP12_YYY_output(&t);
		printf("\n");

//exit(0);

		FP12_YYY_pow(&w,&w,p);
		//printf("w^p^2= ");
		//FP12_YYY_output(&w);
		//printf("\n");
		FP12_YYY_pow(&w,&w,p);
		//printf("w^p^3= ");
		//FP12_YYY_output(&w);
		//printf("\n");
		FP12_YYY_pow(&w,&w,p);
		FP12_YYY_pow(&w,&w,p);
		FP12_YYY_pow(&w,&w,p);
		printf("w^p^6= ");
		FP12_YYY_output(&w);
		printf("\n");
		FP12_YYY_pow(&w,&w,p);
		FP12_YYY_pow(&w,&w,p);
		printf("w^p^8= ");
		FP12_YYY_output(&w);
		printf("\n");
		FP12_YYY_pow(&w,&w,p);
		FP12_YYY_pow(&w,&w,p);
		FP12_YYY_pow(&w,&w,p);
		printf("w^p^11= ");
		FP12_YYY_output(&w);
		printf("\n");

	//	BIG_XXX_zero(p); BIG_XXX_inc(p,7); BIG_XXX_norm(p);
		FP12_YYY_pow(&w,&w,p);

		printf("w^p12= ");
		FP12_YYY_output(&w);
		printf("\n");
//exit(0);

		FP12_YYY_inv(&t,&w);
		printf("1/w mod p^4 = ");
		FP12_YYY_output(&t);
		printf("\n");

		FP12_YYY_inv(&w,&t);
		printf("1/(1/w) mod p^4 = ");
		FP12_YYY_output(&w);
		printf("\n");



	FP12_YYY_inv(&lv,&w);
//printf("w= "); FP12_YYY_output(&w); printf("\n");
	FP12_YYY_conj(&w,&w);
//printf("w= "); FP12_YYY_output(&w); printf("\n");
//exit(0);
	FP12_YYY_mul(&w,&w,&lv);
//printf("w= "); FP12_YYY_output(&w); printf("\n");
	FP12_YYY_copy(&lv,&w);
	FP12_YYY_frob(&w,&f);
	FP12_YYY_frob(&w,&f);
	FP12_YYY_mul(&w,&w,&lv);

//printf("w= "); FP12_YYY_output(&w); printf("\n");
//exit(0);

w.unitary=0;
FP12_YYY_conj(&lv,&w);
	printf("rx= "); FP12_YYY_output(&lv); printf("\n");
FP12_YYY_inv(&lv,&w);
	printf("ry= "); FP12_YYY_output(&lv); printf("\n");


		return 0;
}

*/
