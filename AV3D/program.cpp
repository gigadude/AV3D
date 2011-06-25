#include <windows.h>
#include <stdio.h>

#include "avinclude.h"
#include "glinclude.h"
#include "glwindow.h"
#include "resource.h"

class AV3DWindow : public GLWindow
{
    public:
    AV3DWindow(HINSTANCE hInstance) : GLWindow(hInstance, "AV3D", 800, 600)
    {
    }

    protected:
    void OnLoad()
    {
        glEnable(GL_TEXTURE_2D);
        glShadeModel(GL_SMOOTH);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    }

    void OnRender()
    {
        GLWindow::OnRender();
    }
};

class VideoWall
{
    public:
    VideoWall(int width, int height)
    {
        _width = width;
        _height = height;
    }

    private:
    int _width, _height;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    GLWindow* window = new AV3DWindow(hInstance);
    window->Show();
    window->MainLoop();
}

    /*
    uint8_t* buffer;
    int numBytes;
    AVCodec* codec;
    AVCodecContext* codec_ctx;
    AVFormatContext* ctx;
    AVFrame* frame;
    AVFrame* orig_frame;
    AVPacket packet;
    int i, frameFinished;

    av_register_all();
    if (av_open_input_file(&ctx, "C:\\plasma.avi", NULL, 0, NULL))
    {
        printf("COULD NOT OPEN FILE\r\n");
        return 0;
    }
    else printf("FILE OPENED\r\n");

    if (av_find_stream_info(ctx) < 0)
    {
        printf("NO STREAM INFO FOUND\r\n");
        return 0;
    }
    else printf("STREAM INFO FOUND\r\n");

    printf("\r\nINFO:\r\n");
    dump_format(ctx, 0, "C:\\plasma.avi", 0);
    printf("\r\n");
    
    codec_ctx = ctx->streams[0]->codec;
    codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (!codec)
    {
        printf("NO CODEC FOUND\r\n");
        return 0;
    }
    else printf("CODEC FOUND\r\n");

    if (avcodec_open(codec_ctx, codec) < 0)
    {
        printf("COULD NOT OPEN CODEC\r\n");
        return 0;
    }
    else printf("CODEC OPENED\r\n");

    frame = avcodec_alloc_frame();
    orig_frame = avcodec_alloc_frame();

    numBytes = avpicture_get_size(PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height);
    buffer = (uint8_t*) av_malloc(numBytes*sizeof(uint8_t));
    avpicture_fill((AVPicture*)frame, buffer, PIX_FMT_RGB24, codec_ctx->width, codec_ctx->height);

    i=0;
    while (av_read_frame(ctx, &packet) >= 0)
    {
        if (packet.stream_index == 0)
        {
            avcodec_decode_video2(codec_ctx, orig_frame, &frameFinished, &packet);
            if (frameFinished)
            {
                SwsContext *img_convert_ctx;
                img_convert_ctx = sws_getContext(
                    codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt,
                    codec_ctx->width, codec_ctx->height, PIX_FMT_RGB24,
                    SWS_BICUBIC, NULL, NULL, NULL);

                sws_scale(img_convert_ctx, orig_frame->data, orig_frame->linesize, 0, codec_ctx->height, frame->data, frame->linesize);
                //img_convert((AVPicture*)frame, PIX_FMT_RGB24, (AVPicture*)orig_frame, codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height);
                if (i++ < 5)
                {
                    int y;
                    FILE* file;
                    char filename[32];
                    sprintf(filename, "frame%d.ppm", i);
                    file = fopen(filename, "wb");
                    fprintf(file, "P6\n%d %d\n255\n", codec_ctx->width, codec_ctx->height);

                    for (y=0; y<codec_ctx->height; y++)
                    {
                        fwrite(frame->data[0]+y*frame->linesize[0], 1, codec_ctx->width * 3, file);
                    }
                    fclose(file);
                }
            }
        }
        av_free_packet(&packet);
    }

    av_free(buffer);
    av_free(orig_frame);
    av_free(frame);

    av_close_input_file(ctx);

    getchar();
    return 0;
    */