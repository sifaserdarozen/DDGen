#include <iostream>

#include "g722encoder.h"

int G722EncoderType::SaturateAdd(int op1, int op2) const
{
	int out = op1 + op2;
	if ((((op1 ^ op2) & MIN_32) == 0) && ((out ^ op1) & MIN_32))
		out = (op1 < 0) ? MIN_32 : MAX_32;
	return out;
}

int G722EncoderType::SaturateSubtract(int op1, int op2) const
{
	int out = op1 - op2;
	if ((((op1 ^ op2) & MIN_32) != 0) && ((out ^ op1) & MIN_32))
		out = (op1 < 0L) ? MIN_32 : MAX_32;
	return out;
}

int G722EncoderType::ShiftRight(int op1, short int op2) const
{
		if (op2 >= 31)
			return (op1 < 0) ? -1 : 0;
		else
			return op1 >> op2;
}

int G722EncoderType::ShiftLeft(int op1, short int op2) const
{

		for (; op2 > 0; op2--)
		{
			if (op1 > 0X3fffffff)
				return MAX_32;
			else if (op1 < (int)0xc0000000)
				return MIN_32;
			op1 *= 2;
		}
	return op1;
}


short int G722EncoderType::ShiftLeftShort(short int op1, short int op2) const
{
		int result = ((int)op1) * ((int) 1 << op2);

		if ((op2 > 15 && op1 != 0) || (result != (int) ((short int) result)))
			return (op1 > 0) ? MAX_16 : MIN_16;
		else
			return (short int)result;
}


short int G722EncoderType::ShiftRightShort(short int op1, short int op2) const
{
		if (op2 >= 15)
			return (op1 < 0) ? -1 : 0;
		else
			if (op1 < 0)
				return ~((~op1) >> op2);
			else
				return op1 >> op2;
}

int G722EncoderType::Clamp15ToBits(int op) const
{
	if (op > 16383)
		return 16383;
	else if (op < -16384)
		return -16384;
	return op;
}

int G722EncoderType::MultiplyAdd(int add_op, short int mul_op1, short int mul_op2) const
{
	return SaturateAdd(add_op, ((int)mul_op1 * (int)mul_op2));
}

short int G722EncoderType::Saturate(int op) const
{
	if (op > MAX_16)
		return MAX_16;
	else if (op < MIN_16)
		return MIN_16;
	return op;
}

short int G722EncoderType::SaturateSubtractShort(short int op1, short int op2) const
{
	return Saturate (((int)op1 - op2));
}

short int G722EncoderType::SaturateAddShort(short int op1, short int op2) const
{
	return Saturate (((int)op1 + op2));
}

short int G722EncoderType::ScaledMult(short int op1, short int op2) const
{
	int product = (((int)op1 * (int)op2) & (int)(0xffff8000)) >> 15;

	if (product & (int)0x00010000)
		product |= (int)0xffff0000;

	return (short int)product;
}

short int G722EncoderType::Quantl(short int el, short int detl) const
{
	short int sil = ShiftRightShort(el, 15);
	short int wd = SaturateSubtractShort(MAX_16,(el & MAX_16));
	short int mil = 0;

	if (sil == 0)
		wd = el;

	short int val = ScaledMult(ShiftLeftShort(q6[mil], 3), detl);
	while (SaturateSubtractShort(val,wd) <= 0)
	{
		if (SaturateSubtractShort(mil, 30) == 0)
			break;
		else
		{
			mil = SaturateAddShort(mil, 1);
                        // protect array index frombeing negative
                        if (mil < 0)
                                mil = 0;
			val = ScaledMult(ShiftLeftShort(q6[mil], 3), detl);
		}
	}

	sil = SaturateAddShort(sil, 1);
        // protect array index from being negative
        if (sil < 0)
                sil = 0;
	return misil[sil][mil];
}

short int G722EncoderType::Quanth(short int eh, short int deth) const
{
	short int sih = ShiftRightShort(eh, 15);
	short int wd = SaturateSubtractShort(MAX_16, (eh & MAX_16));

	if (sih == 0)
		wd = eh;

	short int mih = 1;

	if (SaturateSubtractShort(wd, ScaledMult(ShiftLeftShort(564, 3), deth)) >= 0)
		mih = 2;

	sih = SaturateAddShort(sih, 1);
        // protect array index from being negative
        if (sih < 0)
                sih = 0;

	return misih[sih][mih];
}

short int G722EncoderType::Invqal(short int il, short int detl) const
{
	short int ril = ShiftRightShort(il, 2);
	short int wd1 = ShiftLeftShort(oq4[ril4[ril]], 3);
	short int wd2 = -wd1;

	if (risil[ril] == 0)
		wd2 = wd1;

	return ScaledMult(detl, wd2);
}

short int G722EncoderType::Invqah(short int ih, short int deth) const
{
	short int wd1 = ShiftLeftShort(oq2[ih2[ih]], 3);
	short int wd2 = -wd1;

	if (sih[ih] == 0)
		wd2 = wd1;

	return ScaledMult(wd2, deth);
}

short int G722EncoderType::Logscl(short int il, short int nbl) const
{
	short int ril = ShiftRightShort(il, 2);
	short int wd = ScaledMult(nbl, 32512);
	short int il4 = ril4[ril];
	short int nbpl = SaturateAddShort (wd, wl[il4]);

	if (nbpl < 0)
		nbpl = 0;

	if (SaturateSubtractShort(nbpl, 18432) > 0)
		nbpl = 18432;

	return nbpl;
}

short int G722EncoderType::Logsch(short int ih, short int nbh) const
{
	short int wd = ScaledMult(nbh, 32512);
	short int nbph = SaturateAddShort(wd, wh[ih2[ih]]);

	if(nbph < 0)
		nbph = 0;

	if(SaturateSubtractShort(nbph, 22528) > 0)
		nbph = 22528;

	return nbph;
}

short int G722EncoderType::Scalel(short int nbpl) const
{
	short int wd1 = ShiftRightShort(nbpl, 6) & 511;
	short int wd2 = SaturateAddShort(wd1, 64);
        // protect array index from being negative
        if (wd2 < 0)
                wd2 = 0;
	return (ShiftLeftShort(SaturateAddShort(ila[wd2], 1), 2));
}

short int G722EncoderType::Scaleh(short int nbph) const
{
	short int wd = ShiftRightShort(nbph, 6) & 511;
	return ShiftLeftShort(SaturateAddShort(ila[wd], 1), 2);
}


void G722EncoderType::Upzero(short int* dlt_ptr, short int* bl_ptr)
{
	short int wd1 = 128;

	if (dlt_ptr[0] == 0)
		 wd1 = 0;

	short int sg0 = ShiftRightShort(dlt_ptr[0], 15);

	for (short int i = 6; i > 0; i--)
	{
		short int wd2 = SaturateSubtractShort (0, wd1);
		if(sg0 == ShiftRightShort(dlt_ptr[i], 15))
			wd2 = SaturateAddShort (0, wd1);

		bl_ptr[i] = SaturateAddShort(wd2, ScaledMult(bl_ptr[i], 32640));
		dlt_ptr[i] = dlt_ptr[i - 1];
	}
}

void G722EncoderType::Uppol1(short int* al_ptr, short int* plt_ptr)
{
	short int sg0 = ShiftRightShort(plt_ptr[0], 15);
	short int sg1 = ShiftRightShort(plt_ptr[1], 15);
	short int wd1 = -192;

	if (SaturateSubtractShort(sg0, sg1) == 0)
		wd1 = 192;

	short int wd2 = ScaledMult (al_ptr[1], 32640);
	short int apl1 = SaturateAddShort(wd1, wd2);
	short int wd3 = SaturateSubtractShort(15360, al_ptr[2]);

	if (SaturateSubtractShort(apl1, wd3) > 0)
		apl1 = wd3;
	else if (SaturateAddShort(apl1, wd3) < 0)
		apl1 = -wd3;

	/* Shift of the plt signals */
	plt_ptr[2] = plt_ptr[1];
	plt_ptr[1] = plt_ptr[0];
	al_ptr[1] = apl1;
}

void G722EncoderType::Uppol2(short int* al_ptr, short int* plt_ptr)
{
	short int sg0 = ShiftRightShort(plt_ptr[0], 15);
	short int sg1 = ShiftRightShort(plt_ptr[1], 15);
	short int sg2 = ShiftRightShort(plt_ptr[2], 15);
	short int wd1 = ShiftLeftShort(al_ptr[1], 2);
	short int wd2 = SaturateAddShort(0, wd1);

	if (SaturateSubtractShort(sg0, sg1) == 0)
		wd2 = SaturateSubtractShort(0, wd1);

	wd2 = ShiftRightShort(wd2, 7);
	short int wd3 = -128;

	if (SaturateSubtractShort(sg0, sg2) == 0)
		wd3 = 128;

	short int wd4 = SaturateAddShort (wd2, wd3);
	short int wd5 = ScaledMult(al_ptr[2], 32512);
	short int apl2 = SaturateAddShort(wd4, wd5);

	if (SaturateSubtractShort(apl2, 12288) > 0)
		apl2 = 12288;

	if (SaturateSubtractShort(apl2, -12288) < 0)
		apl2 = -12288;

	al_ptr[2] = apl2;
}

short int G722EncoderType::Filtez(short int* dlt_ptr, short int* bl_ptr)
{
	short int szl = 0;

	for (short int i = 6; i > 0; i--)
	{
		short int wd = SaturateAddShort(dlt_ptr[i], dlt_ptr[i]);
		wd = ScaledMult(wd, bl_ptr[i]);
		szl = SaturateAddShort(szl, wd);
	}
	return szl;
}

short int G722EncoderType::Filtep(short int* rlt_ptr, short int* al_ptr)
{
	// shift of rlt
	rlt_ptr[2] = rlt_ptr[1];		
	rlt_ptr[1] = rlt_ptr[0];		

	short int wd1 = SaturateAddShort(rlt_ptr[1], rlt_ptr[1]);
	wd1 = ScaledMult(al_ptr[1], wd1);
	short int wd2 = SaturateAddShort(rlt_ptr[2], rlt_ptr[2]);
	wd2 = ScaledMult(al_ptr[2], wd2);
	return SaturateAddShort(wd1, wd2);
}

void G722EncoderType::QmfTx(short int xin0, short int xin1, short int& xl, short int& xh)
{
	int accuma;
	int accumb;
	int comp_low;
	int comp_high;

	const short int* pcoef = coef_qmf;
	short int* pdelayx = band.qmf_tx_delayx;

	/* Saving past samples in delay line */
	band.qmf_tx_delayx[1] = xin1;
	band.qmf_tx_delayx[0] = xin0;

	accuma = ((int)*pcoef++)*((int)*pdelayx++);
	accumb = ((int)*pcoef++)*((int)*pdelayx++);

	for(short int i = 1; i < 12; i++)
	{
		accuma = MultiplyAdd(accuma, *pcoef++, *pdelayx++);
		accumb = MultiplyAdd(accumb, *pcoef++, *pdelayx++);
	}

	/* Descaling and shift of the delay line */
	for (short int i = 0; i < 22; i++)
		band.qmf_tx_delayx[23 - i] = band.qmf_tx_delayx[21 - i];

	comp_low = SaturateAdd (accuma, accumb);
	comp_low = SaturateAdd (comp_low, comp_low);
	comp_high = SaturateSubtract (accuma, accumb);
	comp_high = SaturateAdd (comp_high, comp_high);
	xl = Clamp15ToBits (ShiftRight(comp_low, 16));
	xh = Clamp15ToBits (ShiftRight(comp_high, 16));
}

short int G722EncoderType::LsbCod(short int xl)
{
	short int il = Quantl (SaturateSubtractShort (xl, band.sl), band.detl);
	band.dlt[0] = Invqal (il, band.detl);
	short int nbpl = Logscl (il, band.nbl);
	band.nbl = nbpl;
	band.detl = Scalel (nbpl);
	band.plt[0] = SaturateAddShort (band.dlt[0], band.szl);   /* parrec */
	band.rlt[0] = SaturateAddShort (band.sl, band.dlt[0]);    /* recons */
	Upzero (band.dlt, band.bl);
	Uppol2 (band.al, band.plt);
	Uppol1 (band.al, band.plt);
	band.szl = Filtez(band.dlt, band.bl);
	band.spl = Filtep(band.rlt, band.al);
	band.sl = SaturateAddShort (band.spl, band.szl);          /* predic */

	/* Return encoded sample */
	return il;
}

short int G722EncoderType::HsbCod(short int xh)
{
	short int ih = Quanth (SaturateSubtractShort(xh, band.sh), band.deth);
	band.dh[0] = Invqah (ih, band.deth);
	short int nbph = Logsch (ih, band.nbh);
	band.nbh = nbph;
	band.deth = Scaleh (nbph);
	band.ph[0] = SaturateAddShort(band.dh[0], band.szh);   /* parrec */
	band.rh[0] = SaturateAddShort(band.sh, band.dh[0]);    /* recons */
	Upzero (band.dh, band.bh);
	Uppol2 (band.ah, band.ph);
	Uppol1 (band.ah, band.ph);
	band.szh = Filtez (band.dh, band.bh);
	band.sph = Filtep (band.rh, band.ah);
	band.sh = SaturateAddShort(band.sph, band.szh);        /* predic */

	return ih;
}

void G722EncoderType::ResetBand()
{
	// initialization due to lower band
	band.detl = 32;
	band.deth = 8;
	band.sl = 0;
	band.sh = 0;
	band.spl = 0;
	band.sph = 0;
	band.szl = 0;
	band.szh = 0;
	band.nbl = 0;
	band.nbh = 0;

	for (int i = 0; i < 2; i++)
	{
		band.al[i] = 0;
		band.ah[i] = 0;
		band.plt[i] = 0;
		band.rlt[i] = 0;
		band.ph[i] = 0;
		band.rh[i] = 0;
	}

	for (int i = 0; i < 7; i++)
	{
		band.bl[i] = 0;
		band.dlt[i] = 0;
		band.bh[i] = 0;
		band.dh[i] = 0;
	}

	for (int i = 0; i < 24; i++)
	{
		band.qmf_tx_delayx[i] = 0;
		band.qmf_rx_delayx[i] = 0;
	}
}

G722EncoderType::G722EncoderType()
{
    ResetBand(); 
}

G722EncoderType::~G722EncoderType()
{

}

bool G722EncoderType :: Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr)
{
	if (!encoded_data_ptr)
	{
		std::cerr << __FILE__ << " " << __LINE__ << "encoded_data_ptr is null" << std::endl;
		return false;
	}

	if (!pcm_data_ptr)
	{
		std::cerr << __FILE__ << " " << __LINE__ << "pcm_data_ptr is null" << std::endl;
		return false;
	}
    
	unsigned char g722_data;
	short int xin1;
	short int xin0;
	short int xl;
	short int il;
	short int xh;
	short int ih;

	for (unsigned int index = 0; index < G722_PACKET_SIZE; index += 2)
	{
		xin1 = *pcm_data_ptr++;
		xin0 = *pcm_data_ptr++;

		// Calculation of the synthesis QMF samples 
		// qmf_tx (xin0, xin1, &xl, &xh, encoder);
		QmfTx(xin0, xin1, xl, xh);

		// Call the upper and lower band ADPCM encoders
		// il = lsbcod (xl, 0, encoder);
		il = LsbCod(xl);
		// ih = hsbcod (xh, 0, encoder);
		ih = HsbCod(xh);

		// Mount the output G722 codeword: bits 0 to 5 are the lower-band
		// portion of the encoding, and bits 6 and 7 are the upper-band
		// portion of the encoding
		// code[i] = s_and(add(shl(ih, 6), il), 0xFF);
		g722_data = (unsigned char) SaturateAddShort(ShiftLeftShort(ih, 6), il);

		*encoded_data_ptr++ = g722_data;
	}

	return true;
}
