#include "VideoOutput.h"

#ifdef INCLUDE_VIDEO


#define inline _inline
#define snprintf _snprintf

extern "C"
{
	#include <stdint.h>
	#include <inttypes.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswscale/swscale.h>
}

#pragma comment(lib, "../../ffmpeg/lib/avcodec-52.lib")
#pragma comment(lib, "../../ffmpeg/lib/avdevice-52.lib")
#pragma comment(lib, "../../ffmpeg/lib/avformat-52.lib")
#pragma comment(lib, "../../ffmpeg/lib/avutil-49.lib")
#pragma comment(lib, "../../ffmpeg/lib/swscale-0.lib")

#endif


VideoOutput::VideoOutput(void)
{
#ifdef INCLUDE_VIDEO

	c= NULL;
	codec = NULL;
	picture = NULL;
	f = NULL;

	img_convert_ctx = NULL;

	av_register_all();
#endif
}

bool VideoOutput::StartVideo(const char *filename, int width, int height)
{
#ifdef INCLUDE_VIDEO
	int size;
	i = 0;
	
	/* find the mpeg1 video encoder */
	codec = avcodec_find_encoder(CODEC_ID_MPEG1VIDEO);
		//CODEC_ID_MPEG1VIDEO);
	if (!codec) {
		LOG("codec not found");
		return false;
	}

	c= avcodec_alloc_context();
	picture= avcodec_alloc_frame();

	/* put sample parameters */
	c->bit_rate = 400000;
	
	/* resolution must be a multiple of two */
	if(height %2 != 0) height--;

	//fix for packing - crude solution
	while(width % 4 != 0) width--;

	c->width = width;
	c->height = height;

	PixelFormat pixformat = PIX_FMT_YUV420P;

	/* frames per second */
	c->time_base.den = 24;
	c->time_base.num = 1;
	c->gop_size = 10; /* emit one intra frame every ten frames */
	c->max_b_frames=1;
	c->pix_fmt = pixformat;

	/* open it */
	if (avcodec_open(c, codec) < 0) {
		LOG("could not open codec");
		return false;
	}


	f = fopen(filename, "wb");
	if (!f) {
		LOG("could not open %s", filename);
		return false;
	}

	/* alloc image and output buffer */
	outbuf_size = 100000;
	outbuf = (uint8_t*) malloc(outbuf_size);
	size = c->width * c->height;
	picture_buf = (uint8_t*) malloc((size * 3) / 2); 

	picture->data[0] = picture_buf;
	picture->data[1] = picture->data[0] + size;
	picture->data[2] = picture->data[1] + size / 4;
	picture->linesize[0] = c->width;
	picture->linesize[1] = c->width / 2;
	picture->linesize[2] = c->width / 2;

	imageData = new unsigned char[c->width * c->height * 3]; //Allocate memory for storing the image
	imageData2 = new unsigned char[c->width * c->height * 3];


	if(img_convert_ctx == NULL) {

		img_convert_ctx = sws_getContext(width, height, 
			PIX_FMT_RGB24, 
			width, height, pixformat, SWS_BICUBIC, 
			NULL, NULL, NULL);

		if(img_convert_ctx == NULL) {
			LOG("Cannot initialize the conversion context!");
			return false;
		}
	}
#endif
	return true;
}

void VideoOutput::DoFrame()
{
#ifdef INCLUDE_VIDEO
	int x, y;

	glReadBuffer(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glReadPixels(0, 0, c->width, c->height, GL_RGB, GL_UNSIGNED_BYTE, imageData); //Copy

	for(y = c->height - 1; y >= 0 ; y--)
	{
		for(x = 0; x < c->width; x++)
		{
			imageData2[y * c->width*3 + x*3] = imageData[(c->height - y - 1) * c->width*3 + x*3];
			imageData2[y * c->width*3 + x*3 + 1] = imageData[(c->height - y - 1) * c->width*3 + x*3 + 1];
			imageData2[y * c->width*3 + x*3 + 2] = imageData[(c->height - y - 1) * c->width*3 + x*3 + 2];
		}
	}


	AVPicture rawImage;
	avpicture_fill(&rawImage, (uint8_t*)imageData2, PIX_FMT_RGB24, c->width, c->height);
	

	sws_scale(img_convert_ctx,
		rawImage.data, rawImage.linesize,
		0, c->height,
		picture->data, 
		picture->linesize);

	out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
	LOG("encoding frame %3d (size=%5d)", i++, out_size);
	fwrite(outbuf, 1, out_size, f);

#endif
}

void VideoOutput::StopVideo()
{
#ifdef INCLUDE_VIDEO
	/* get the delayed frames */
	for(; out_size; i++) {
		fflush(stdout);

		out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);
		LOG("write frame %3d (size=%5d)", i, out_size);
		fwrite(outbuf, 1, out_size, f);
	}

	/* add sequence end code to have a real mpeg file */
	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, f);
	fclose(f);
	free(picture_buf);
	free(outbuf);

	avcodec_close(c);
	av_free(c);
	av_free(picture);
	delete [] imageData;
	delete [] imageData2;
	imageData = NULL;

	img_convert_ctx = NULL;
#endif
}