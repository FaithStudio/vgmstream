#include <math.h>
#include "coding.h"
#include "../util.h"

/* for some algos, maybe closer to the real thing */
#define VAG_USE_INTEGER_TABLE  0


/* PS ADPCM table (precalculated divs) */
static const double VAG_f[5][2] = {
        {   0.0        ,   0.0        },
        {  60.0 / 64.0 ,   0.0        },
        { 115.0 / 64.0 , -52.0 / 64.0 },
        {  98.0 / 64.0 , -55.0 / 64.0 },
        { 122.0 / 64.0 , -60.0 / 64.0 }
};
#if VAG_USE_INTEGER_TABLE
/* PS ADPCM table */
static const int8_t VAG_coefs[5][2] = {
        {   0 ,   0 },
        {  60 ,   0 },
        { 115 , -52 },
        {  98 , -55 },
        { 122 , -60 }
};
#endif


/* PSVita ADPCM table */
static const int16_t HEVAG_coefs[128][4] = {
        {      0,     0,     0,     0 },
        {   7680,     0,     0,     0 },
        {  14720, -6656,     0,     0 },
        {  12544, -7040,     0,     0 },
        {  15616, -7680,     0,     0 },
        {  14731, -7059,     0,     0 },
        {  14507, -7366,     0,     0 },
        {  13920, -7522,     0,     0 },
        {  13133, -7680,     0,     0 },
        {  12028, -7680,     0,     0 },
        {  10764, -7680,     0,     0 },
        {   9359, -7680,     0,     0 },
        {   7832, -7680,     0,     0 },
        {   6201, -7680,     0,     0 },
        {   4488, -7680,     0,     0 },
        {   2717, -7680,     0,     0 },
        {    910, -7680,     0,     0 },
        {   -910, -7680,     0,     0 },
        {  -2717, -7680,     0,     0 },
        {  -4488, -7680,     0,     0 },
        {  -6201, -7680,     0,     0 },
        {  -7832, -7680,     0,     0 },
        {  -9359, -7680,     0,     0 },
        { -10764, -7680,     0,     0 },
        { -12028, -7680,     0,     0 },
        { -13133, -7680,     0,     0 },
        { -13920, -7522,     0,     0 },
        { -14507, -7366,     0,     0 },
        { -14731, -7059,     0,     0 },
        {   5376, -9216,  3328, -3072 },
        {  -6400, -7168, -3328, -2304 },
        { -10496, -7424, -3584, -1024 },
        {   -167, -2722,  -494,  -541 },
        {  -7430, -2221, -2298,   424 },
        {  -8001, -3166, -2814,   289 },
        {   6018, -4750,  2649, -1298 },
        {   3798, -6946,  3875, -1216 },
        {  -8237, -2596, -2071,   227 },
        {   9199,  1982, -1382, -2316 },
        {  13021, -3044, -3792,  1267 },
        {  13112, -4487, -2250,  1665 },
        {  -1668, -3744, -6456,   840 },
        {   7819, -4328,  2111,  -506 },
        {   9571, -1336,  -757,   487 },
        {  10032, -2562,   300,   199 },
        {  -4745, -4122, -5486, -1493 },
        {  -5896,  2378, -4787, -6947 },
        {  -1193, -9117, -1237, -3114 },
        {   2783, -7108, -1575, -1447 },
        {  -7334, -2062, -2212,   446 },
        {   6127, -2577,  -315,   -18 },
        {   9457, -1858,   102,   258 },
        {   7876, -4483,  2126,  -538 },
        {  -7172, -1795, -2069,   482 },
        {  -7358, -2102, -2233,   440 },
        {  -9170, -3509, -2674,  -391 },
        {  -2638, -2647, -1929, -1637 },
        {   1873,  9183,  1860, -5746 },
        {   9214,  1859, -1124, -2427 },
        {  13204, -3012, -4139,  1370 },
        {  12437, -4792,  -256,   622 },
        {  -2653, -1144, -3182, -6878 },
        {   9331, -1048,  -828,   507 },
        {   1642,  -620,  -946, -4229 },
        {   4246, -7585,  -533, -2259 },
        {  -8988, -3891, -2807,    44 },
        {  -2562, -2735, -1730, -1899 },
        {   3182,  -483,  -714, -1421 },
        {   7937, -3844,  2821, -1019 },
        {  10069, -2609,   314,   195 },
        {   8400, -3297,  1551,  -155 },
        {  -8529, -2775, -2432,  -336 },
        {   9477, -1882,   108,   256 },
        {     75, -2241,  -298, -6937 },
        {  -9143, -4160, -2963,     5 },
        {  -7270, -1958, -2156,   460 },
        {  -2740,  3745,  5936, -1089 },
        {   8993,  1948,  -683, -2704 },
        {  13101, -2835, -3854,  1055 },
        {   9543, -1961,   130,   250 },
        {   5272, -4270,  3124, -3157 },
        {  -7696, -3383, -2907,  -456 },
        {   7309,  2523,   434, -2461 },
        {  10275, -2867,   391,   172 },
        {  10940, -3721,   665,    97 },
        {     24,  -310, -1262,   320 },
        {  -8122, -2411, -2311,  -271 },
        {  -8511, -3067, -2337,   163 },
        {    326, -3846,   419,  -933 },
        {   8895,  2194,  -541, -2880 },
        {  12073, -1876, -2017,  -601 },
        {   8729, -3423,  1674,  -169 },
        {  12950, -3847, -3007,  1946 },
        {  10038, -2570,   302,   198 },
        {   9385, -2757,  1008,    41 },
        {  -4720, -5006, -2852, -1161 },
        {   7869, -4326,  2135,  -501 },
        {   2450, -8597,  1299, -2780 },
        {  10192, -2763,   360,   181 },
        {  11313, -4213,   833,    53 },
        {  10154, -2716,   345,   185 },
        {   9638, -1417,  -737,   482 },
        {   3854, -4554,  2843, -3397 },
        {   6699, -5659,  2249, -1074 },
        {  11082, -3908,   728,    80 },
        {  -1026, -9810,  -805, -3462 },
        {  10396, -3746,  1367,   -96 },
        {  10287,   988, -1915, -1437 },
        {   7953,  3878,  -764, -3263 },
        {  12689, -3375, -3354,  2079 },
        {   6641,  3166,   231, -2089 },
        {  -2348, -7354, -1944, -4122 },
        {   9290, -4039,  1885,  -246 },
        {   4633, -6403,  1748, -1619 },
        {  11247, -4125,   802,    61 },
        {   9807, -2284,   219,   222 },
        {   9736, -1536,  -706,   473 },
        {   8440, -3436,  1562,  -176 },
        {   9307, -1021,  -835,   509 },
        {   1698, -9025,   688, -3037 },
        {  10214, -2791,   368,   179 },
        {   8390,  3248,  -758, -2989 },
        {   7201,  3316,    46, -2614 },
        {    -88, -7809,  -538, -4571 },
        {   6193, -5189,  2760, -1245 },
        {  12325, -1290, -3284,   253 },
        {  13064, -4075, -2824,  1877 },
        {   5333,  2999,   775, -1132 }
};


/**
 * Sony's VAG ADPCM, decodes 16 bytes into 28 samples.
 * The first 2 bytes are a header (shift, predictor, optional flag)
 *
 * Flags:
 *  0x0: Nothing
 *  0x1: End marker + decode
 *  0x2: Loop region
 *  0x3: Loop end
 *  0x4: Start marker
 *  0x5: ?
 *  0x6: Loop start
 *  0x7: End marker + don't decode
 *  0x8+ Not valid
 */
void decode_psx(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

	int predict_nr, shift_factor, sample;
	int32_t hist1=stream->adpcm_history1_32;
	int32_t hist2=stream->adpcm_history2_32;

	short scale;
	int i;
	int32_t sample_count;
	uint8_t flag;

	int framesin = first_sample/28;

	predict_nr = read_8bit(stream->offset+framesin*16,stream->streamfile) >> 4;
	shift_factor = read_8bit(stream->offset+framesin*16,stream->streamfile) & 0xf;
	flag = read_8bit(stream->offset+framesin*16+1,stream->streamfile); /* only lower nibble needed */

	first_sample = first_sample % 28;
	
	for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {

		sample=0;

		if(flag<0x07) {
		
			short sample_byte = (short)read_8bit(stream->offset+(framesin*16)+2+i/2,stream->streamfile);

			scale = ((i&1 ? /* odd/even byte */
				     sample_byte >> 4 :
					 sample_byte & 0x0f)<<12);

			sample=(int)((scale >> shift_factor)+hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]);
		}

		outbuf[sample_count] = clamp16(sample);
		hist2=hist1;
		hist1=sample;
	}
	stream->adpcm_history1_32=hist1;
	stream->adpcm_history2_32=hist2;
}

void decode_invert_psx(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

	int predict_nr, shift_factor, sample;
	int32_t hist1=stream->adpcm_history1_32;
	int32_t hist2=stream->adpcm_history2_32;

	short scale;
	int i;
	int32_t sample_count;
	uint8_t flag;

	int framesin = first_sample/28;
    int head = read_8bit(stream->offset+framesin*16,stream->streamfile) ^ stream->bmdx_xor;

	predict_nr = ((head >> 4) & 0xf);
	shift_factor = (head & 0xf);
	flag = read_8bit(stream->offset+framesin*16+1,stream->streamfile);

	first_sample = first_sample % 28;
	
	for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {

		sample=0;

		if(flag<0x07) {
		
			short sample_byte = (short)read_8bit(stream->offset+(framesin*16)+2+i/2,stream->streamfile);
            if (i/2 == 0)
                sample_byte = (short)(int8_t)(sample_byte+stream->bmdx_add);

			scale = ((i&1 ?
				     sample_byte >> 4 :
					 sample_byte & 0x0f)<<12);

			sample=(int)((scale >> shift_factor)+hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]);
		}

		outbuf[sample_count] = clamp16(sample);
		hist2=hist1;
		hist1=sample;
	}
	stream->adpcm_history1_32=hist1;
	stream->adpcm_history2_32=hist2;
}

/* some TAITO games have garbage (?) in their flags, this decoder
 * just ignores that byte */
void decode_psx_badflags(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

	int predict_nr, shift_factor, sample;
	int32_t hist1=stream->adpcm_history1_32;
	int32_t hist2=stream->adpcm_history2_32;

	short scale;
	int i;
	int32_t sample_count;

	int framesin = first_sample/28;

	predict_nr = read_8bit(stream->offset+framesin*16,stream->streamfile) >> 4;
	shift_factor = read_8bit(stream->offset+framesin*16,stream->streamfile) & 0xf;
	first_sample = first_sample % 28;
	
	for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {
        short sample_byte = (short)read_8bit(stream->offset+(framesin*16)+2+i/2,stream->streamfile);

        scale = ((i&1 ?
                    sample_byte >> 4 :
                    sample_byte & 0x0f)<<12);

        sample=(int)((scale >> shift_factor)+hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]);

		outbuf[sample_count] = clamp16(sample);
		hist2=hist1;
		hist1=sample;
	}
	stream->adpcm_history1_32=hist1;
	stream->adpcm_history2_32=hist2;
}

/* FF XI's Vag-ish format */
void decode_ffxi_adpcm(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

	int predict_nr, shift_factor, sample;
	int32_t hist1=stream->adpcm_history1_32;
	int32_t hist2=stream->adpcm_history2_32;

	short scale;
	int i;
	int32_t sample_count;
    int32_t predictor;

	int framesin = first_sample/16;

	predict_nr = read_8bit(stream->offset+framesin*9,stream->streamfile) >> 4;
	shift_factor = read_8bit(stream->offset+framesin*9,stream->streamfile) & 0xf;
	first_sample = first_sample % 16;
	
	for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {
        short sample_byte = (short)read_8bit(stream->offset+(framesin*9)+1+i/2,stream->streamfile);

		sample=0;

        scale = ((i&1 ?
                    sample_byte >> 4 :
                    sample_byte & 0x0f)<<12);

#if !VAG_USE_INTEGER_TABLE
        predictor =
                (int)((hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]));
#else
        predictor = 
                (hist1*VAG_coefs[predict_nr][0]+hist2*VAG_coefs[predict_nr][1])/64;
#endif
        sample=(scale >> shift_factor) + predictor;

		outbuf[sample_count] = clamp16(sample);
		hist2=hist1;
		hist1=sample;
	}
	stream->adpcm_history1_32=hist1;
	stream->adpcm_history2_32=hist2;
}

void decode_baf_adpcm(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

	int predict_nr, shift_factor, sample;
	int32_t hist1=stream->adpcm_history1_32;
	int32_t hist2=stream->adpcm_history2_32;

	short scale;
	int i;
	int32_t sample_count;

	int framesin = first_sample/64;

	predict_nr = read_8bit(stream->offset+framesin*33,stream->streamfile) >> 4;
	shift_factor = read_8bit(stream->offset+framesin*33,stream->streamfile) & 0xf;

	first_sample = first_sample % 64;
	
	for (i=first_sample,sample_count=0; i<first_sample+samples_to_do; i++,sample_count+=channelspacing) {
		short sample_byte = (short)read_8bit(stream->offset+(framesin*33)+1+i/2,stream->streamfile);

		scale = ((i&1 ?
			     sample_byte >> 4 :
				 sample_byte & 0x0f)<<12);

		sample=(int)((scale >> shift_factor)+hist1*VAG_f[predict_nr][0]+hist2*VAG_f[predict_nr][1]);

		outbuf[sample_count] = clamp16(sample);
		hist2=hist1;
		hist1=sample;
	}
	stream->adpcm_history1_32=hist1;
	stream->adpcm_history2_32=hist2;
}


/**
 * Sony's HEVAG (High Efficiency VAG) ADPCM, used in PSVita games (hardware decoded).
 * Variation of the regular VAG, uses 4 history samples and a bigger table.
 *
 * Original research and algorithm by id-daemon / daemon1.
 */
void decode_hevag_adpcm(VGMSTREAMCHANNEL * stream, sample * outbuf, int channelspacing, int32_t first_sample, int32_t samples_to_do) {

    uint8_t predict_nr, shift, flag, byte;
    int32_t scale = 0;

    int32_t sample;
    int32_t hist1 = stream->adpcm_history1_32;
    int32_t hist2 = stream->adpcm_history2_32;
    int32_t hist3 = stream->adpcm_history3_32;
    int32_t hist4 = stream->adpcm_history4_32;

    int i, sample_count;


    int framesin = first_sample / 28;

    /* 4 byte header: predictor = 3rd and 1st, shift = 2nd, flag = 4th */
    byte = (uint8_t)read_8bit(stream->offset+framesin*16+0,stream->streamfile);
    predict_nr   = byte >> 4;
    shift = byte & 0x0f;
    byte = (uint8_t)read_8bit(stream->offset+framesin*16+1,stream->streamfile);
    predict_nr = (byte & 0xF0) | predict_nr;
    flag = byte & 0x0f; /* no change in flags */

    first_sample = first_sample % 28;

    for (i = first_sample, sample_count = 0; i < first_sample + samples_to_do; i++, sample_count += channelspacing) {
        sample = 0;

        if (flag < 7 && predict_nr < 128) {

            if (i & 1) {/* odd/even nibble */
                scale = byte >> 4;
            } else {
                byte = read_8bit(stream->offset+(framesin*16)+2+i/2,stream->streamfile);
                scale = byte & 0x0f;
            }
            if (scale > 7) { /* sign fix */
                scale = scale - 16;
            }

            sample = (hist1 * HEVAG_coefs[predict_nr][0] +
                      hist2 * HEVAG_coefs[predict_nr][1] +
                      hist3 * HEVAG_coefs[predict_nr][2] +
                      hist4 * HEVAG_coefs[predict_nr][3] ) / 32;
            sample = (sample + (scale << (20 - shift)) + 128) >> 8;
        }

        outbuf[sample_count] = clamp16(sample);
        hist4 = hist3;
        hist3 = hist2;
        hist2 = hist1;
        hist1 = sample;
    }

    stream->adpcm_history1_32 = hist1;
    stream->adpcm_history2_32 = hist2;
    stream->adpcm_history3_32 = hist3;
    stream->adpcm_history4_32 = hist4;
}
