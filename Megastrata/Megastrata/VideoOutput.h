#pragma once
#include <utils.h>

struct AVCodecContext;
struct AVCodec;
struct AVFrame;
struct SwsContext;

class VideoOutput
{
public:

	bool StartVideo(const char *filename, int width, int height);
	void StopVideo();
	void DoFrame();
	VideoOutput(void);

private:
	AVCodecContext *c;
	AVCodec *codec;
	AVFrame *picture;
	FILE *f;

	int i;
	int out_size;
	int outbuf_size;

	unsigned char *outbuf, *picture_buf;
	unsigned char *imageData;
	unsigned char *imageData2;

	SwsContext *img_convert_ctx;
};