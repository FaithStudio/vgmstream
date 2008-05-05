#include "ps2_npsf.h"
#include "../util.h"

/* Sony .ADS with SShd & SSbd Headers */

VGMSTREAM * init_vgmstream_ps2_npsf(const char * const filename) {
    VGMSTREAM * vgmstream = NULL;
    STREAMFILE * infile = NULL;

    int loop_flag=0;
	int channel_count;
    off_t start_offset;
    int i;

    /* check extension, case insensitive */
    if (strcasecmp("npsf",filename_extension(filename))) goto fail;

    /* try to open the file for header reading */
    infile = open_streamfile(filename);
    if (!infile) goto fail;

    /* check NPSF Header */
    if (read_32bitBE(0x00,infile) != 0x4E505346)
        goto fail;

	/* check loop */
	loop_flag = (read_32bitLE(0x14,infile)!=0xFFFFFFFF);
    channel_count=read_32bitLE(0x0C,infile);
    
	/* build the VGMSTREAM */
    vgmstream = allocate_vgmstream(channel_count,loop_flag);
    if (!vgmstream) goto fail;

	/* fill in the vital statistics */
	vgmstream->channels = read_32bitLE(0x0C,infile);
    vgmstream->sample_rate = read_32bitLE(0x18,infile);

	/* Check for Compression Scheme */
	vgmstream->coding_type = coding_PSX;
    vgmstream->num_samples = read_32bitLE(0x08,infile)*28/16;

	/* Get loop point values */
	if(vgmstream->loop_flag) {
		vgmstream->loop_start_sample = read_32bitLE(0x14,infile);
		vgmstream->loop_end_sample = read_32bitLE(0x08,infile)*28/16;
	}

	vgmstream->interleave_block_size = read_32bitLE(0x04,infile)/2;
    vgmstream->layout_type = layout_interleave;
    vgmstream->meta_type = meta_PS2_NPSF;

	start_offset = (off_t)read_32bitLE(0x10,infile);

    close_streamfile(infile); infile=NULL;

    /* open the file for reading by each channel */
    {
        for (i=0;i<channel_count;i++) {
            vgmstream->ch[i].streamfile = open_streamfile_buffer(filename,0x8000);

            if (!vgmstream->ch[i].streamfile) goto fail;

            vgmstream->ch[i].channel_start_offset=
                vgmstream->ch[i].offset=
                start_offset+vgmstream->interleave_block_size*i;
        }
    }

    return vgmstream;

    /* clean up anything we may have opened */
fail:
    if (infile) close_streamfile(infile);
    if (vgmstream) close_vgmstream(vgmstream);
    return NULL;
}
