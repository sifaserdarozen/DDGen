/**
 * @file
 * @brief encoder and corresponding factory's
 *
 * @author Sifa Serder Ozen sifa.serder.ozen@gmail.com
 */

#pragma once

#include "encoder.h"

#define G722_PACKET_SIZE 320    /**< G722 packet size is 320 sapmles, 20ms data at 16kHz sampling */
#define G722_RTP_PAYLOAD_TYPE 0x9    /**< G722 rtp payload type */

/* **** Coefficients for both transmission and reception QMF **** */
const short int coef_qmf[24] =
{
	3 * 2, -11 * 2, -11 * 2, 53 * 2, 12 * 2, -156 * 2,
	32 * 2, 362 * 2, -210 * 2, -805 * 2, 951 * 2, 3876 * 2,
	3876 * 2, 951 * 2, -805 * 2, -210 * 2, 362 * 2, 32 * 2,
	-156 * 2, 12 * 2, 53 * 2, -11 * 2, -11 * 2, 3 * 2
};

const short int misil[2][32] =
{
	{
		0x0000, 0x003F, 0x003E, 0x001F, 0x001E, 0x001D, 0x001C, 0x001B,
		0x001A, 0x0019, 0x0018, 0x0017, 0x0016, 0x0015, 0x0014, 0x0013,
		0x0012, 0x0011, 0x0010, 0x000F, 0x000E, 0x000D, 0x000C, 0x000B,
		0x000A, 0x0009, 0x0008, 0x0007, 0x0006, 0x0005, 0x0004, 0x0000
	},
	{
		0x0000, 0x003D, 0x003C, 0x003B, 0x003A, 0x0039, 0x0038, 0x0037,
		0x0036, 0x0035, 0x0034, 0x0033, 0x0032, 0x0031, 0x0030, 0x002F,
		0x002E, 0x002D, 0x002C, 0x002B, 0x002A, 0x0029, 0x0028, 0x0027,
		0x0026, 0x0025, 0x0024, 0x0023, 0x0022, 0x0021, 0x0020, 0x0000
	}
};

const short int q6[31] =
{
	0, 35, 72, 110, 150, 190, 233, 276,
	323, 370, 422, 473, 530, 587, 650, 714,
	786, 858, 940, 1023, 1121, 1219, 1339, 1458,
	1612, 1765, 1980, 2195, 2557, 2919, 3200
};

const short int ril4[16] = {0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 1, 0};
const short int risil[16] = {0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0};
const short int oq4[8] = {0, 150, 323, 530, 786, 1121, 1612, 2557};
const short int wl[8] = {-60, -30, 58, 172, 334, 538, 1198, 3042};

const short int ila[353] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 4, 4, 4, 4, 4,
	4, 4, 4, 5, 5, 5, 5, 5,
	5, 5, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 8, 8,
	8, 8, 8, 9, 9, 9, 9, 10,
	10, 10, 10, 11, 11, 11, 11, 12,
	12, 12, 13, 13, 13, 13, 14, 14,
	15, 15, 15, 16, 16, 16, 17, 17,
	18, 18, 18, 19, 19, 20, 20, 21,
	21, 22, 22, 23, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 28, 29, 30,
	31, 31, 32, 33, 33, 34, 35, 36,
	37, 37, 38, 39, 40, 41, 42, 43,
	44, 45, 46, 47, 48, 49, 50, 51,
	52, 54, 55, 56, 57, 58, 60, 61,
	63, 64, 65, 67, 68, 70, 71, 73,
	75, 76, 78, 80, 82, 83, 85, 87,
	89, 91, 93, 95, 97, 99, 102, 104,
	106, 109, 111, 113, 116, 118, 121, 124,
	127, 129, 132, 135, 138, 141, 144, 147,
	151, 154, 157, 161, 165, 168, 172, 176,
	180, 184, 188, 192, 196, 200, 205, 209,
	214, 219, 223, 228, 233, 238, 244, 249,
	255, 260, 266, 272, 278, 284, 290, 296,
	303, 310, 316, 323, 331, 338, 345, 353,
	361, 369, 377, 385, 393, 402, 411, 420,
	429, 439, 448, 458, 468, 478, 489, 500,
	511, 522, 533, 545, 557, 569, 582, 594,
	607, 621, 634, 648, 663, 677, 692, 707,
	723, 739, 755, 771, 788, 806, 823, 841,
	860, 879, 898, 918, 938, 958, 979, 1001,
	1023, 1045, 1068, 1092, 1115, 1140, 1165, 1190,
	1216, 1243, 1270, 1298, 1327, 1356, 1386, 1416,
	1447, 1479, 1511, 1544, 1578, 1613, 1648, 1684,
	1721, 1759, 1797, 1837, 1877, 1918, 1960, 2003,
	2047, 2092, 2138, 2185, 2232, 2281, 2331, 2382,
	2434, 2488, 2542, 2598, 2655, 2713, 2773, 2833,
	2895, 2959, 3024, 3090, 3157, 3227, 3297, 3370,
	3443, 3519, 3596, 3675, 3755, 3837, 3921, 4007,
	4095};

const short int misih[2][3] = {{0, 1, 0}, {0, 3, 2}};
const short ih2[4] = {2, 1, 2, 1};
const short sih[4] = {-1, -1, 0, 0};
const short oq2[3] = {0, 202, 926};
const short wh[3] = {0, -214, 798};

const short int full_g722_consts[] = {
	// const short int coef_qmf[24]
	3 * 2, -11 * 2, -11 * 2, 53 * 2, 12 * 2, -156 * 2,
	32 * 2, 362 * 2, -210 * 2, -805 * 2, 951 * 2, 3876 * 2,
	3876 * 2, 951 * 2, -805 * 2, -210 * 2, 362 * 2, 32 * 2,
	-156 * 2, 12 * 2, 53 * 2, -11 * 2, -11 * 2, 3 * 2,

	//const short int misil[2][32]
	0x0000, 0x003F, 0x003E, 0x001F, 0x001E, 0x001D, 0x001C, 0x001B,
	0x001A, 0x0019, 0x0018, 0x0017, 0x0016, 0x0015, 0x0014, 0x0013,
	0x0012, 0x0011, 0x0010, 0x000F, 0x000E, 0x000D, 0x000C, 0x000B,
	0x000A, 0x0009, 0x0008, 0x0007, 0x0006, 0x0005, 0x0004, 0x0000,
	0x0000, 0x003D, 0x003C, 0x003B, 0x003A, 0x0039, 0x0038, 0x0037,
	0x0036, 0x0035, 0x0034, 0x0033, 0x0032, 0x0031, 0x0030, 0x002F,
	0x002E, 0x002D, 0x002C, 0x002B, 0x002A, 0x0029, 0x0028, 0x0027,
	0x0026, 0x0025, 0x0024, 0x0023, 0x0022, 0x0021, 0x0020, 0x0000,

	// const short int q6[31]
	0, 35, 72, 110, 150, 190, 233, 276,
	323, 370, 422, 473, 530, 587, 650, 714,
	786, 858, 940, 1023, 1121, 1219, 1339, 1458,
	1612, 1765, 1980, 2195, 2557, 2919, 3200,

	// const short int ril4[16]
	0, 7, 6, 5, 4, 3, 2, 1, 7, 6, 5, 4, 3, 2, 1, 0,

	// const short int risil[16]
	0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0,
	// const short int oq4[8]
	0, 150, 323, 530, 786, 1121, 1612, 2557,
	// const short int wl[8]
	-60, -30, 58, 172, 334, 538, 1198, 3042,

	// const short int ila[353]
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 4, 4, 4, 4, 4,
	4, 4, 4, 5, 5, 5, 5, 5,
	5, 5, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 8, 8,
	8, 8, 8, 9, 9, 9, 9, 10,
	10, 10, 10, 11, 11, 11, 11, 12,
	12, 12, 13, 13, 13, 13, 14, 14,
	15, 15, 15, 16, 16, 16, 17, 17,
	18, 18, 18, 19, 19, 20, 20, 21,
	21, 22, 22, 23, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 28, 29, 30,
	31, 31, 32, 33, 33, 34, 35, 36,
	37, 37, 38, 39, 40, 41, 42, 43,
	44, 45, 46, 47, 48, 49, 50, 51,
	52, 54, 55, 56, 57, 58, 60, 61,
	63, 64, 65, 67, 68, 70, 71, 73,
	75, 76, 78, 80, 82, 83, 85, 87,
	89, 91, 93, 95, 97, 99, 102, 104,
	106, 109, 111, 113, 116, 118, 121, 124,
	127, 129, 132, 135, 138, 141, 144, 147,
	151, 154, 157, 161, 165, 168, 172, 176,
	180, 184, 188, 192, 196, 200, 205, 209,
	214, 219, 223, 228, 233, 238, 244, 249,
	255, 260, 266, 272, 278, 284, 290, 296,
	303, 310, 316, 323, 331, 338, 345, 353,
	361, 369, 377, 385, 393, 402, 411, 420,
	429, 439, 448, 458, 468, 478, 489, 500,
	511, 522, 533, 545, 557, 569, 582, 594,
	607, 621, 634, 648, 663, 677, 692, 707,
	723, 739, 755, 771, 788, 806, 823, 841,
	860, 879, 898, 918, 938, 958, 979, 1001,
	1023, 1045, 1068, 1092, 1115, 1140, 1165, 1190,
	1216, 1243, 1270, 1298, 1327, 1356, 1386, 1416,
	1447, 1479, 1511, 1544, 1578, 1613, 1648, 1684,
	1721, 1759, 1797, 1837, 1877, 1918, 1960, 2003,
	2047, 2092, 2138, 2185, 2232, 2281, 2331, 2382,
	2434, 2488, 2542, 2598, 2655, 2713, 2773, 2833,
	2895, 2959, 3024, 3090, 3157, 3227, 3297, 3370,
	3443, 3519, 3596, 3675, 3755, 3837, 3921, 4007,
	4095,

	// const short int misih[2][3]
	0, 1, 0,
	0, 3, 2,

	// const short ih2[4]
	2, 1, 2, 1,
	//const short sih[4]
	-1, -1, 0, 0,
	// const short oq2[3]
	0, 202, 926,
	// const short wh[3]
	0, -214, 798
};

#define MIN_32 (int)(0x80000000)
#define MAX_32 (int)(0x7fffffff)
#define MAX_16 (short int)(0x7fff)
#define MIN_16 (short int)(0x8000)

struct FullBandType
{
	short int detl;
	short int deth;
	short int nbl;
	short int sl;
	short int spl;
	short int szl;
	short int nbh;
	short int sh;
	short int sph;
	short int szh;
	short int al[3];
	short int plt[3]; /* plt[0]=plt */
	short int rlt[3];
	short int ah[3];
	short int ph[3]; /* ph[0]=ph */
	short int rh[3];
	short int bl[7];
	short int dlt[7]; /* dlt[0]=dlt */
	short int bh[7];
	short int dh[7]; /* dh[0]=dh */
	short int qmf_tx_delayx[24];
	short int qmf_rx_delayx[24];
};


/**
 * @brief G711aEncoder realization
 *
 * Encoder interface
 * @see EncoderType()
 * @see G711uEncoderType()
 */
class G722EncoderType : public EncoderType
{
private:
	FullBandType band;

	int SaturateAdd(int op1, int op2) const;
	int SaturateSubtract(int op1, int op2) const;
	short int SaturateSubtractShort(short int op1, short int op2) const;
	short int SaturateAddShort(short int op1, short int op2) const;
	int MultiplyAdd(int add_op, short int mul_op1, short int mul_op2) const;
	int ShiftRight(int op1, short int op2) const;
	int ShiftLeft(int op1, short int op2) const;
	short int ShiftLeftShort(short int op1, short int op2) const;
	short int ShiftRightShort(short int op1, short int op2) const;
	int Clamp15ToBits(int op) const;
	short int Saturate(int op) const;
	short int ScaledMult(short int op1, short int op2) const;

	short int Quantl(short int el, short int detl) const;
	short int Quanth(short int eh, short int deth) const;
	short int Invqal(short int il, short int detl) const;
	short int Invqah(short int ih, short int deth) const;
	short int Logscl(short int il, short int nbl) const;
	short int Logsch(short int ih, short int nbh) const;
	short int Scalel(short int nbpl) const;
	short int Scaleh(short int nbph) const;
	void Upzero(short int* dlt_ptr, short int* bl_ptr);
	void Uppol1(short int* al_ptr, short int* plt_ptr);
	void Uppol2(short int* al_ptr, short int* plt_ptr);
	short int Filtez(short int* dlt_ptr, short int* bl_ptr);
	short int Filtep(short int* rlt_ptr, short int* al_ptr);
	void QmfTx(short int xin0, short int xin1, short int& xl, short int& xh);
	short int LsbCod(short int xl);
	short int HsbCod(short int xh);

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G722EncoderType();

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G722EncoderType();

    /**
     * @brief g722 encoding implementation
     *
     * 16kHz g722 type A encoder implementation
     * @param pcm_data_ptr INPUT pointer to input pcm data that will be encoded, should contain GetPacaketSize() of data
     * @param encoded_data_ptr OUTPUT pointer to hold encoded output, should point to GetPacketSize() of data
     * @return indicates success of encoding
     * @see GetPacketSize()
     */
    virtual bool Encode(const short int* pcm_data_ptr, unsigned char* encoded_data_ptr);

    /**
     * @brief Reset band memory of encoder. Either used before first packer of after a silence period.
     */
    void ResetBand();

    /**
     * @brief implementation for getting rtp payload
     *
     * @return rtp payload type of encoder
     */
    virtual unsigned char GetRtpPayload() const { return G722_RTP_PAYLOAD_TYPE; }

    /**
     * @brief Implementation for obtaining packet size
     *
     * Default packet size of G722EncoderType is G722_PACKET_SIZE samples.
     * @return packet size of encoder
     */
    virtual unsigned short int GetPacketSize() const { return G722_PACKET_SIZE; }
};

class G722EncoderFactoryType : public EncoderFactoryType
{
private:

public:
    /**
     * @brief Default constructor, does not perform any specific operation
     */
    G722EncoderFactoryType() { }

    /**
     * @brief Default destructor, does not perform any specific operation
     */
    virtual ~G722EncoderFactoryType() {}

    /**
     * @brief Implementation of encoder generation
     *
     * @return G722EncoderType is created and returned to calling object
     * @see EncoderType()
     * @see G722EncoderType()
     */
    virtual EncoderType* CreateEncoder() const { return new G722EncoderType(); }
};
