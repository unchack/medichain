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

/* Fixed Data in ROM - Field and Curve parameters */


package com.ibm.zurich.amcl.BN254CX;

public class ROM
{
// Base Bits= 56
	public static final long[] Modulus= {0x6623EF5C1B55B3L,0xD6EE18093EE1BEL,0x647A6366D3243FL,0x8702A0DB0BDDFL,0x24000000L};
	public static final long[] R2modp= {0x466A0618A0800AL,0x2B3A22543056A3L,0x148515B09C6600L,0xEC9EA5606BDF50L,0x1C992E66L};
	public static final long MConst= 0x4E205BF9789E85L;

	public static final int CURVE_A= 0;
	public static final int CURVE_B_I= 2;
	public static final long[] CURVE_B= {0x2L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0x11C0A636EB1F6DL,0xD6EE0CC906CEBEL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L};
	public static final long[] CURVE_Gx= {0x6623EF5C1B55B2L,0xD6EE18093EE1BEL,0x647A6366D3243FL,0x8702A0DB0BDDFL,0x24000000L};
	public static final long[] CURVE_Gy= {0x1L,0x0L,0x0L,0x0L,0x0L};

	public static final long[] CURVE_Bnx= {0x3C012B1L,0x40L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Cru= {0xE0931794235C97L,0xDF6471EF875631L,0xCA83F1440BDL,0x480000L,0x0L};
	public static final long[] Fra= {0xD9083355C80EA3L,0x7326F173F8215BL,0x8AACA718986867L,0xA63A0164AFE18BL,0x1359082FL};
	public static final long[] Frb= {0x8D1BBC06534710L,0x63C7269546C062L,0xD9CDBC4E3ABBD8L,0x623628A900DC53L,0x10A6F7D0L};
	public static final long[] CURVE_Pxa= {0xC3E806CB5FAAF5L,0xB8E61B99362851L,0x73CB19C2F0944FL,0x7F9A9B4FA7121CL,0x1AC95A5BL};
	public static final long[] CURVE_Pxb= {0xE50060FC15F433L,0x33A1ABDA094DEDL,0xD1681F364C1BBAL,0x36566043F11C2L,0x53D3001L};
	public static final long[] CURVE_Pya= {0x33216A9772A299L,0xF0936EA3484E68L,0xFEF918406479DFL,0xB09E6FB2EE06BBL,0x15727970L};
	public static final long[] CURVE_Pyb= {0x1BC11F9C49E8CDL,0x56091A061ADC4L,0x7F56070166D320L,0x2AC189FD57BD7L,0x11BCB56CL};
	public static final long[][] CURVE_W= {{0x546349162FEB83L,0xB40381200L,0x6000L,0x0L,0x0L},{0x7802561L,0x80L,0x0L,0x0L,0x0L}};
	public static final long[][][] CURVE_SB= {{{0x5463491DB010E4L,0xB40381280L,0x6000L,0x0L,0x0L},{0x7802561L,0x80L,0x0L,0x0L,0x0L}},{{0x7802561L,0x80L,0x0L,0x0L,0x0L},{0xBD5D5D20BB33EAL,0xD6EE0188CEBCBDL,0x647A6366D2643FL,0x8702A0DB0BDDFL,0x24000000L}}};
	public static final long[][] CURVE_WB= {{0x1C2118567A84B0L,0x3C012B040L,0x2000L,0x0L,0x0L},{0xCDF995BE220475L,0x94EDA8CA7F9A36L,0x8702A0DC07EL,0x300000L,0x0L},{0x66FCCAE0F10B93L,0x4A76D4653FCD3BL,0x4381506E03FL,0x180000L,0x0L},{0x1C21185DFAAA11L,0x3C012B0C0L,0x2000L,0x0L,0x0L}};
	public static final long[][][] CURVE_BB= {{{0x11C0A6332B0CBDL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x11C0A6332B0CBCL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x11C0A6332B0CBCL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x7802562L,0x80L,0x0L,0x0L,0x0L}},{{0x7802561L,0x80L,0x0L,0x0L,0x0L},{0x11C0A6332B0CBCL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x11C0A6332B0CBDL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x11C0A6332B0CBCL,0xD6EE0CC906CE7EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L}},{{0x7802562L,0x80L,0x0L,0x0L,0x0L},{0x7802561L,0x80L,0x0L,0x0L,0x0L},{0x7802561L,0x80L,0x0L,0x0L,0x0L},{0x7802561L,0x80L,0x0L,0x0L,0x0L}},{{0x3C012B2L,0x40L,0x0L,0x0L,0x0L},{0xF004AC2L,0x100L,0x0L,0x0L,0x0L},{0x11C0A62F6AFA0AL,0xD6EE0CC906CE3EL,0x647A6366D2C43FL,0x8702A0DB0BDDFL,0x24000000L},{0x3C012B2L,0x40L,0x0L,0x0L,0x0L}}};

}
