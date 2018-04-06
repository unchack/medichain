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

/* AMCL BN Curve Pairing functions */

package com.ibm.zurich.amcl.BN254CX;

public final class PAIR {

	public static final boolean USE_GLV =true;
	public static final boolean USE_GS_G2 =true;
	public static final boolean USE_GS_GT =true;	
	public static final boolean GT_STRONG=false;


/* Line function */
	public static FP12 line(ECP2 A,ECP2 B,FP Qx,FP Qy)
	{
//System.out.println("Into line");
		FP4 a,b,c;                            // Edits here
		c=new FP4(0);
		if (A==B)
		{ // Doubling
			FP2 XX=new FP2(A.getx());  //X
			FP2 YY=new FP2(A.gety());  //Y
			FP2 ZZ=new FP2(A.getz());  //Z
			FP2 YZ=new FP2(YY);        //Y 
			YZ.mul(ZZ);                //YZ
			XX.sqr();	               //X^2
			YY.sqr();	               //Y^2
			ZZ.sqr();			       //Z^2
			
			YZ.imul(4);
			YZ.neg(); YZ.norm();       //-2YZ
			YZ.pmul(Qy);               //-2YZ.Ys

			XX.imul(6);                //3X^2
			XX.pmul(Qx);               //3X^2.Xs

			int sb=3*ROM.CURVE_B_I;
			ZZ.imul(sb); 	
			
			ZZ.div_ip2();  ZZ.norm(); // 3b.Z^2 

			YY.add(YY);
			ZZ.sub(YY); ZZ.norm();     // 3b.Z^2-Y^2

			a=new FP4(YZ,ZZ);          // -2YZ.Ys | 3b.Z^2-Y^2 | 3X^2.Xs 
			b=new FP4(XX);             // L(0,1) | L(0,0) | L(1,0)

			A.dbl();
		}
		else
		{ // Addition - assume B is affine

			FP2 X1=new FP2(A.getx());    // X1
			FP2 Y1=new FP2(A.gety());    // Y1
			FP2 T1=new FP2(A.getz());    // Z1
			FP2 T2=new FP2(A.getz());    // Z1
			
			T1.mul(B.gety());    // T1=Z1.Y2 
			T2.mul(B.getx());    // T2=Z1.X2

			X1.sub(T2); X1.norm();  // X1=X1-Z1.X2
			Y1.sub(T1); Y1.norm();  // Y1=Y1-Z1.Y2

			T1.copy(X1);            // T1=X1-Z1.X2
			X1.pmul(Qy);            // X1=(X1-Z1.X2).Ys
			T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

			T2.copy(Y1);            // T2=Y1-Z1.Y2
			T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
			T2.sub(T1); T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2
			Y1.pmul(Qx);  Y1.neg(); Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs

			a=new FP4(X1,T2);       // (X1-Z1.X2).Ys  |  (Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2  | - (Y1-Z1.Y2).Xs
			b=new FP4(Y1);

			A.add(B);
		}
//System.out.println("Out of line");
		return new FP12(a,b,c);
	}

/* Optimal R-ate pairing */
	public static FP12 ate(ECP2 P,ECP Q)
	{
		FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
		BIG x=new BIG(ROM.CURVE_Bnx);
		BIG n=new BIG(x);
		ECP2 K=new ECP2();
		FP12 lv;

		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			n.pmul(6); n.dec(2);
		}
		else
			n.copy(x);
		n.norm();
		
		FP Qx=new FP(Q.getx());
		FP Qy=new FP(Q.gety());

		ECP2 A=new ECP2();
		FP12 r=new FP12(1);

		A.copy(P);
		int nb=n.nbits();

		for (int i=nb-2;i>=1;i--)
		{
			lv=line(A,A,Qx,Qy);
			r.smul(lv);

			if (n.bit(i)==1)
			{
				lv=line(A,P,Qx,Qy);
				r.smul(lv);
			}
			r.sqr();
		}

		lv=line(A,A,Qx,Qy);
		r.smul(lv);
		if (n.parity()==1)
		{
			lv=line(A,P,Qx,Qy);
			r.smul(lv);
		}

/* R-ate fixup required for BN curves */
		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			r.conj();
			K.copy(P);
			K.frob(f);
			A.neg();
			lv=line(A,K,Qx,Qy);
			r.smul(lv);
			K.frob(f);
			K.neg();
			lv=line(A,K,Qx,Qy);
			r.smul(lv);
		}
		return r;
	}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
	public static FP12 ate2(ECP2 P,ECP Q,ECP2 R,ECP S)
	{
		FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
		BIG x=new BIG(ROM.CURVE_Bnx);
		BIG n=new BIG(x);
		ECP2 K=new ECP2();
		FP12 lv;

		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			n.pmul(6); n.dec(2);
		}
		else
			n.copy(x);
		n.norm();

		FP Qx=new FP(Q.getx());
		FP Qy=new FP(Q.gety());
		FP Sx=new FP(S.getx());
		FP Sy=new FP(S.gety());

		ECP2 A=new ECP2();
		ECP2 B=new ECP2();
		FP12 r=new FP12(1);

		A.copy(P);
		B.copy(R);
		int nb=n.nbits();

		for (int i=nb-2;i>=1;i--)
		{
			lv=line(A,A,Qx,Qy);
			r.smul(lv);

			lv=line(B,B,Sx,Sy);
			r.smul(lv);

			if (n.bit(i)==1)
			{
				lv=line(A,P,Qx,Qy);
				r.smul(lv);
				lv=line(B,R,Sx,Sy);
				r.smul(lv);
			}
			r.sqr();

		}

		lv=line(A,A,Qx,Qy);
		r.smul(lv);
		lv=line(B,B,Sx,Sy);
		r.smul(lv);
		if (n.parity()==1)
		{
			lv=line(A,P,Qx,Qy);
			r.smul(lv);
			lv=line(B,R,Sx,Sy);
			r.smul(lv);
		}

/* R-ate fixup required for BN curves */
		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			r.conj();
			K.copy(P);
			K.frob(f);
			A.neg();
			lv=line(A,K,Qx,Qy);
			r.smul(lv);
			K.frob(f);
			K.neg();
			lv=line(A,K,Qx,Qy);
			r.smul(lv);

			K.copy(R);
			K.frob(f);
			B.neg();
			lv=line(B,K,Sx,Sy);
			r.smul(lv);
			K.frob(f);
			K.neg();
			lv=line(B,K,Sx,Sy);
			r.smul(lv);
		}
		return r;
	}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
	public static FP12 fexp(FP12 m)
	{
		FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
		BIG x=new BIG(ROM.CURVE_Bnx);
		FP12 r=new FP12(m);

/* Easy part of final exp */
		FP12 lv=new FP12(r);
		lv.inverse();
		r.conj();

		r.mul(lv);
		lv.copy(r);
		r.frob(f);
		r.frob(f);
		r.mul(lv);
/* Hard part of final exp */
		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			FP12 x0,x1,x2,x3,x4,x5;			
			lv.copy(r);
			lv.frob(f);
			x0=new FP12(lv);
			x0.frob(f);
			lv.mul(r);
			x0.mul(lv);
			x0.frob(f);
			x1=new FP12(r);
			x1.conj();
			x4=r.pow(x);

			x3=new FP12(x4);
			x3.frob(f);

			x2=x4.pow(x);

			x5=new FP12(x2); x5.conj();
			lv=x2.pow(x);

			x2.frob(f);
			r.copy(x2); r.conj();

			x4.mul(r);
			x2.frob(f);

			r.copy(lv);
			r.frob(f);
			lv.mul(r);

			lv.usqr();
			lv.mul(x4);
			lv.mul(x5);
			r.copy(x3);
			r.mul(x5);
			r.mul(lv);
			lv.mul(x2);
			r.usqr();
			r.mul(lv);
			r.usqr();
			lv.copy(r);
			lv.mul(x1);
			r.mul(x0);
			lv.usqr();
			r.mul(lv);
			r.reduce();
		}
		else
		{

			FP12 y0,y1,y2,y3;
// Ghamman & Fouotsa Method
			y0=new FP12(r); y0.usqr();
			y1=y0.pow(x);
			x.fshr(1); y2=y1.pow(x); x.fshl(1);
			y3=new FP12(r); y3.conj();
			y1.mul(y3);

			y1.conj();
			y1.mul(y2);

			y2=y1.pow(x);

			y3=y2.pow(x);
			y1.conj();
			y3.mul(y1);

			y1.conj();
			y1.frob(f); y1.frob(f); y1.frob(f);
			y2.frob(f); y2.frob(f);
			y1.mul(y2);

			y2=y3.pow(x);
			y2.mul(y0);
			y2.mul(r);

			y1.mul(y2);
			y2.copy(y3); y2.frob(f);
			y1.mul(y2);
			r.copy(y1);
			r.reduce();
		}
		
		return r;
	}

/* GLV method */
	public static BIG[] glv(BIG e)
	{
		BIG[] u=new BIG[2];
		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			int i,j;
			BIG t=new BIG(0);
			BIG q=new BIG(ROM.CURVE_Order);

			BIG[] v=new BIG[2];
			for (i=0;i<2;i++)
			{
				t.copy(new BIG(ROM.CURVE_W[i]));  // why not just t=new BIG(ROM.CURVE_W[i]); 
				DBIG d=BIG.mul(t,e);
				v[i]=new BIG(d.div(q));
				u[i]=new BIG(0);
			}
			u[0].copy(e);
			for (i=0;i<2;i++)
				for (j=0;j<2;j++)
				{
					t.copy(new BIG(ROM.CURVE_SB[j][i]));
					t.copy(BIG.modmul(v[j],t,q));
					u[i].add(q);
					u[i].sub(t);
					u[i].mod(q);
				}
		}
		else
		{ // -(x^2).P = (Beta.x,y)
			BIG q=new BIG(ROM.CURVE_Order);
			BIG x=new BIG(ROM.CURVE_Bnx);
			BIG x2=BIG.smul(x,x);
			u[0]=new BIG(e);
			u[0].mod(x2);
			u[1]=new BIG(e);
			u[1].div(x2);
			u[1].rsub(q);
		}
		return u;
	}

/* Galbraith & Scott Method */
	public static BIG[] gs(BIG e)
	{
		BIG[] u=new BIG[4];
		if (ECP.CURVE_PAIRING_TYPE==ECP.BN)
		{
			int i,j;
			BIG t=new BIG(0);
			BIG q=new BIG(ROM.CURVE_Order);
			BIG[] v=new BIG[4];
			for (i=0;i<4;i++)
			{
				t.copy(new BIG(ROM.CURVE_WB[i]));
				DBIG d=BIG.mul(t,e);
				v[i]=new BIG(d.div(q));
				u[i]=new BIG(0);
			}
			u[0].copy(e);
			for (i=0;i<4;i++)
				for (j=0;j<4;j++)
				{
					t.copy(new BIG(ROM.CURVE_BB[j][i]));
					t.copy(BIG.modmul(v[j],t,q));
					u[i].add(q);
					u[i].sub(t);
					u[i].mod(q);
				}
		}
		else
		{
			BIG x=new BIG(ROM.CURVE_Bnx);
			BIG w=new BIG(e);
			for (int i=0;i<3;i++)
			{
				u[i]=new BIG(w);
				u[i].mod(x);
				w.div(x);
			}
			u[3]=new BIG(w);
		}
		return u;
	}	

/* Multiply P by e in group G1 */
	public static ECP G1mul(ECP P,BIG e)
	{
		ECP R;
		if (USE_GLV)
		{
			P.affine();
			R=new ECP();
			R.copy(P);
			int i,np,nn;
			ECP Q=new ECP();
			Q.copy(P);
			BIG q=new BIG(ROM.CURVE_Order);
			FP cru=new FP(new BIG(ROM.CURVE_Cru));
			BIG t=new BIG(0);
			BIG[] u=glv(e);
			Q.getx().mul(cru);

			np=u[0].nbits();
			t.copy(BIG.modneg(u[0],q));
			nn=t.nbits();
			if (nn<np)
			{
				u[0].copy(t);
				R.neg();
			}

			np=u[1].nbits();
			t.copy(BIG.modneg(u[1],q));
			nn=t.nbits();
			if (nn<np)
			{
				u[1].copy(t);
				Q.neg();
			}

			R=R.mul2(u[0],Q,u[1]);
			
		}
		else
		{
			R=P.mul(e);
		}
		return R;
	}

/* Multiply P by e in group G2 */
	public static ECP2 G2mul(ECP2 P,BIG e)
	{
		ECP2 R;
		if (USE_GS_G2)
		{
			ECP2[] Q=new ECP2[4];
			FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
			BIG q=new BIG(ROM.CURVE_Order);
			BIG[] u=gs(e);

			BIG t=new BIG(0);
			int i,np,nn;
			P.affine();

			Q[0]=new ECP2(); Q[0].copy(P);
			for (i=1;i<4;i++)
			{
				Q[i]=new ECP2(); Q[i].copy(Q[i-1]);
				Q[i].frob(f);
			}
			for (i=0;i<4;i++)
			{
				np=u[i].nbits();
				t.copy(BIG.modneg(u[i],q));
				nn=t.nbits();
				if (nn<np)
				{
					u[i].copy(t);
					Q[i].neg();
				}
			}

			R=ECP2.mul4(Q,u);
		}
		else
		{
			R=P.mul(e);
		}
		return R;
	}

/* f=f^e */
/* Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP4.java */
	public static FP12 GTpow(FP12 d,BIG e)
	{
		FP12 r;
		if (USE_GS_GT)
		{
			FP12[] g=new FP12[4];
			FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
			BIG q=new BIG(ROM.CURVE_Order);
			BIG t=new BIG(0);
			int i,np,nn;
			BIG[] u=gs(e);

			g[0]=new FP12(d);
			for (i=1;i<4;i++)
			{
				g[i]=new FP12(0); g[i].copy(g[i-1]);
				g[i].frob(f);
			}
			for (i=0;i<4;i++)
			{
				np=u[i].nbits();
				t.copy(BIG.modneg(u[i],q));
				nn=t.nbits();
				if (nn<np)
				{
					u[i].copy(t);
					g[i].conj();
				}
			}
			r=FP12.pow4(g,u);
		}
		else
		{
			r=d.pow(e);
		}
		return r;
	}

/* test group membership - no longer needed */
/* with GT-Strong curve, now only check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */
/*
	public static boolean GTmember(FP12 m)
	{
		if (m.isunity()) return false;
		FP12 r=new FP12(m);
		r.conj();
		r.mul(m);
		if (!r.isunity()) return false;

		FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));

		r.copy(m); r.frob(f); r.frob(f);
		FP12 w=new FP12(r); w.frob(f); w.frob(f);
		w.mul(m);
		if (!ROM.GT_STRONG)
		{
			if (!w.equals(r)) return false;
			BIG x=new BIG(ROM.CURVE_Bnx);
			r.copy(m); w=r.pow(x); w=w.pow(x);
			r.copy(w); r.sqr(); r.mul(w); r.sqr();
			w.copy(m); w.frob(f);
		}
		return w.equals(r);
	}
*/
/*
	public static void main(String[] args) {
		ECP Q=new ECP(new BIG(ROM.CURVE_Gx),new BIG(ROM.CURVE_Gy));
		ECP2 P=new ECP2(new FP2(new BIG(ROM.CURVE_Pxa),new BIG(ROM.CURVE_Pxb)),new FP2(new BIG(ROM.CURVE_Pya),new BIG(ROM.CURVE_Pyb)));

		BIG r=new BIG(ROM.CURVE_Order);
		BIG xa=new BIG(ROM.CURVE_Pxa);

		System.out.println("P= "+P.toString());
		System.out.println("Q= "+Q.toString());

		BIG m=new BIG(17);

		FP12 e=ate(P,Q);
		System.out.println("\ne= "+e.toString());

		e=fexp(e);

		for (int i=1;i<1000;i++)
		{
			e=ate(P,Q);
			e=fexp(e);
		}
	//	e=GTpow(e,m);

		System.out.println("\ne= "+e.toString());

		BIG [] GLV=glv(r);

		System.out.println("GLV[0]= "+GLV[0].toString());
		System.out.println("GLV[0]= "+GLV[1].toString());

		ECP G=new ECP(); G.copy(Q);
		ECP2 R=new ECP2(); R.copy(P);


		e=ate(R,Q);
		e=fexp(e);

		e=GTpow(e,xa);
		System.out.println("\ne= "+e.toString()); 


		R=G2mul(R,xa);
		e=ate(R,G);
		e=fexp(e);

		System.out.println("\ne= "+e.toString());

		G=G1mul(G,xa);
		e=ate(P,G);
		e=fexp(e);
		System.out.println("\ne= "+e.toString()); 
	} */
}

