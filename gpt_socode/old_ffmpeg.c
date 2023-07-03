#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/channel_layout.h>
#include <libavutil/parseutils.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/avutil.h>


#define ERR_BUFF_SIZE 1024

int extract_frames(const char* filename, const char* output_dir) {
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* dec = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int ret, i;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, filename, NULL, NULL)) < 0) {
        char err_buf[ERR_BUFF_SIZE];
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Could not open input file '%s': %s\n", filename, err_buf);
        return ret;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        char err_buf[ERR_BUFF_SIZE];
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Could not find stream information: %s\n", err_buf);
        return ret;
    }

    av_dump_format(fmt_ctx, 0, filename, 0);

    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream* stream = fmt_ctx->streams[i];

        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            dec_ctx = avcodec_alloc_context3(NULL);
            avcodec_parameters_to_context(dec_ctx, stream->codecpar);

            dec = avcodec_find_decoder(dec_ctx->codec_id);
            if (!dec) {
                fprintf(stderr, "Failed to find codec\n");
                return AVERROR_DECODER_NOT_FOUND;
            }

            if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
                char err_buf[ERR_BUFF_SIZE];
                av_strerror(ret, err_buf, ERR_BUFF_SIZE);
                fprintf(stderr, "Failed to open codec: %s\n", err_buf);
                return ret;
            }
        }
    }

    if (!dec_ctx) {
        fprintf(stderr, "Could not find video stream\n");
        return AVERROR_STREAM_NOT_FOUND;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return AVERROR(ENOMEM);
    }

    i = 0;
    while (1) {
        ret = av_read_frame(fmt_ctx, &pkt);
        if (ret < 0)
            break;

        if (pkt.stream_index == dec_ctx->stream_index) {
            ret = avcodec_send_packet(dec_ctx, &pkt);
            if (ret < 0) {
                fprintf(stderr, "Error sending a packet for decoding\n");
                return ret;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    break;
                else if (ret < 0) {
                    fprintf(stderr, "Error during decoding\n");
                    return ret;
                }

                char filename[1024];
                snprintf(filename, sizeof(filename), "%s/frame-%03d.ppm", output_dir, i++);
                FILE* f = fopen(filename, "wb");
            }
        }
    }

    return 0;
}

int encode_video(const char* filename, const char* input_dir, int fps, int width, int height) {
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* enc_ctx = NULL;
    AVCodec* enc = NULL;
    AVStream* stream = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int ret, i;

    av_register_all();

    if ((ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename)) < 0) {
        char err_buf[ERR_BUFF_SIZE];
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Could not allocate output context: %s\n", err_buf);
        return ret;
    }

    enc = avcodec_find_encoder_by_name("libx264");
    if (!enc) {
        fprintf(stderr, "Could not find encoder\n");
        return AVERROR_ENCODER_NOT_FOUND;
    }

    stream = avformat_new_stream(fmt_ctx, enc);
    if (!stream) {
        fprintf(stderr, "Could not allocate stream\n");
        return AVERROR(ENOMEM);
    }

    enc_ctx = avcodec_alloc_context3(enc);
    if (!enc_ctx) {
        fprintf(stderr, "Could not allocate encoder context\n");
        return AVERROR(ENOMEM);
    }

    enc_ctx->codec_id = enc->id;
    enc_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    enc_ctx->width = width;
    enc_ctx->height = height;
    enc_ctx->time_base = (AVRational){ 1, fps };
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    if ((ret = avcodec_open2(enc_ctx, enc, NULL)) < 0) {
        char err_buf[ERR_BUFF_SIZE];
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Could not open codec: %s\n", err_buf);
        return ret;
    }

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE)) < 0) {
            char err_buf[ERR_BUFF_SIZE];
            av_strerror(ret, err_buf, ERR_BUFF_SIZE);
            fprintf(stderr, "Could not open output file '%s': %s\n", filename, err_buf);
            return ret;
        }
    }

    if ((ret = avformat_write_header(fmt_ctx, NULL)) < 0) {
        char err_buf[ERR_BUFF_SIZE];
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error writing header: %s\n", err_buf);
        return ret;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        return AVERROR(ENOMEM);
    }

    frame->format = enc_ctx->pix_fmt;
    frame->width = enc_ctx->width;
    frame->height = enc_ctx->height;

    return 0;
}

int convert_audio(const char* src_filename, const char* dst_filename, enum AVSampleFormat dst_sample_fmt, int dst_sample_rate, int dst_channels)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* src_fmt_ctx = NULL, * dst_fmt_ctx = NULL;
    AVStream* src_stream = NULL, * dst_stream = NULL;
    AVCodecContext* src_dec_ctx = NULL, * dst_enc_ctx = NULL;
    AVCodec* dst_codec = NULL;
    int dst_stream_idx = 0;
    SwrContext* swr_ctx = NULL;

    av_register_all();

    if ((ret = avformat_open_input(&src_fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(src_fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    if ((src_stream = av_find_best_stream(src_fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0)) < 0) {
        fprintf(stderr, "Error finding audio stream in input file\n");
        goto end;
    }

    src_dec_ctx = src_stream->codec;
    dst_codec = avcodec_find_encoder(AV_CODEC_ID_PCM_S16LE);
    if (!dst_codec) {
        fprintf(stderr, "Error finding PCM encoder\n");
        goto end;
    }

    dst_fmt_ctx = avformat_alloc_context();
    if (!dst_fmt_ctx) {
        fprintf(stderr, "Error allocating output context\n");
        goto end;
    }

    dst_stream = avformat_new_stream(dst_fmt_ctx, NULL);
    if (!dst_stream) {
        fprintf(stderr, "Error creating output stream\n");
        goto end;
    }

    dst_enc_ctx = avcodec_alloc_context3(dst_codec);
    if (!dst_enc_ctx) {
        fprintf(stderr, "Error allocating encoder context\n");
        goto end;
    }

    dst_enc_ctx->channels = dst_channels;
    dst_enc_ctx->channel_layout = av_get_default_channel_layout(dst_channels);
    dst_enc_ctx->sample_rate = dst_sample_rate;
    dst_enc_ctx->sample_fmt = dst_sample_fmt;
    dst_stream->time_base = (AVRational){ 1, dst_sample_rate };

    if ((ret = avcodec_open2(dst_enc_ctx, dst_codec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening encoder: %s\n", err_buf);
        goto end;
    }

    if ((ret = avio_open(&dst_fmt_ctx->pb, dst_filename, AVIO_FLAG_WRITE)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening output file: %s\n", err_buf);
        goto end;
    }

    return 0;
}

int extract_frames2(const char* src_filename, const char* dst_dirname)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* dec = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int stream_idx = 0;
    char dst_filename[1024];
    int frame_count = 0;
    int got_frame = 0;
    struct SwsContext* sws_ctx = NULL;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    if ((stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0)) < 0) {
        fprintf(stderr, "Error finding video stream in input file\n");
        goto end;
    }

    dec_ctx = fmt_ctx->streams[stream_idx]->codec;
    dec = avcodec_find_decoder(dec_ctx->codec_id);
    if (!dec) {
        fprintf(stderr, "Error finding decoder\n");
        goto end;
    }

    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening decoder: %s\n", err_buf);
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating frame\n");
        goto end;
    }

    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == stream_idx) {
            avcodec_decode_video2(dec_ctx, frame, &got_frame, &pkt);
            if (got_frame) {
                sws_ctx = sws_getContext(dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                    dec_ctx->width, dec_ctx->height, AV_PIX_FMT_RGB24,
                    SWS_BILINEAR, NULL, NULL, NULL);
                if (!sws_ctx) {
                    fprintf(stderr, "Error creating scaling context\n");
                    goto end;
                }

                snprintf(dst_filename, sizeof(dst_filename), "%s/frame%d.bmp", dst_dirname, frame_count++);
                FILE* fp = fopen(dst_filename, "wb");
                if (!fp) {
                    fprintf(stderr, "Error opening output file: %s\n", dst_filename);
                    goto end;
                }

                uint8_t* data[AV_NUM_DATA_POINTERS] = { 0 };
                data[0] = frame->data[0];
                int linesize[AV_NUM_DATA_POINTERS] = { 0 };
            }
        }
    }

    return 0;
}

int transcode(const char* src_filename, const char* dst_filename, const char* dst_format)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* in_fmt_ctx = NULL, * out_fmt_ctx = NULL;
    AVCodecContext* in_dec_ctx = NULL, * out_enc_ctx = NULL;
    AVCodec* in_dec = NULL, * out_enc = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int in_stream_idx = 0, out_stream_idx = 0;
    int got_frame = 0;
    struct SwsContext* sws_ctx = NULL;

    av_register_all();

    if ((ret = avformat_open_input(&in_fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(in_fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    if ((in_stream_idx = av_find_best_stream(in_fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0)) < 0) {
        fprintf(stderr, "Error finding video stream in input file\n");
        goto end;
    }

    in_dec_ctx = in_fmt_ctx->streams[in_stream_idx]->codec;
    in_dec = avcodec_find_decoder(in_dec_ctx->codec_id);
    if (!in_dec) {
        fprintf(stderr, "Error finding decoder\n");
        goto end;
    }

    if ((ret = avcodec_open2(in_dec_ctx, in_dec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening decoder: %s\n", err_buf);
        goto end;
    }

    if (!(out_fmt_ctx = avformat_alloc_context())) {
        fprintf(stderr, "Error allocating output context\n");
        goto end;
    }

    out_fmt_ctx->oformat = av_guess_format(dst_format, NULL, NULL);
    if (!out_fmt_ctx->oformat) {
        fprintf(stderr, "Error guessing output format\n");
        goto end;
    }

    if ((ret = avformat_new_stream(out_fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error creating output stream: %s\n", err_buf);
        goto end;
    }

    out_stream_idx = out_fmt_ctx->nb_streams - 1;
    out_enc_ctx = out_fmt_ctx->streams[out_stream_idx]->codec;
    out_enc = avcodec_find_encoder(out_enc_ctx->codec_id);
    if (!out_enc) {
        fprintf(stderr, "Error finding encoder\n");
        goto end;
    }

    return 0;
}

int extract_frames(const char* src_filename, const char* dst_filename)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* dec = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int stream_idx = 0;
    int got_frame = 0;
    int frame_num = 0;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    if ((stream_idx = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0)) < 0) {
        fprintf(stderr, "Error finding video stream in input file\n");
        goto end;
    }

    dec_ctx = fmt_ctx->streams[stream_idx]->codec;
    dec = avcodec_find_decoder(dec_ctx->codec_id);
    if (!dec) {
        fprintf(stderr, "Error finding decoder\n");
        goto end;
    }

    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening decoder: %s\n", err_buf);
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating frame\n");
        goto end;
    }

    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == stream_idx) {
            ret = avcodec_decode_video2(dec_ctx, frame, &got_frame, &pkt);
            if (ret < 0) {
                av_strerror(ret, err_buf, ERR_BUFF_SIZE);
                fprintf(stderr, "Error decoding video: %s\n", err_buf);
                goto end;
            }

            if (got_frame) {
                char filename[256];
                snprintf(filename, 256, "%s_%04d.jpg", dst_filename, frame_num);
                FILE* f = fopen(filename, "wb");
                if (!f) {
                    fprintf(stderr, "Error opening file for writing\n");
                    goto end;
                }

                AVFrame* rgb_frame = av_frame_alloc();
                if (!rgb_frame) {
                    fprintf(stderr, "Error allocating RGB frame\n");
                    fclose(f);
                    goto end;
                }

                ret = av_image_alloc(rgb_frame->data, rgb_frame->linesize, dec_ctx->width, dec_ctx->height, AV_PIX_FMT_RGB24, 1);
                if (ret < 0) {
                    av_strerror(ret, err_buf, ERR_BUFF_SIZE);
                    fprintf(stderr, "Error allocating RGB frame\n");
                    goto end;
                }
            }
        }
    }
    return 0;
}

int encode_video(const char* src_pattern, const char* dst_filename, int fps, int width, int height)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* out_fmt_ctx = NULL;
    AVCodecContext* enc_ctx = NULL;
    AVCodec* enc = NULL;
    AVFrame* frame = NULL;
    AVPacket pkt;
    int frame_num = 0;
    char filename[256];

    av_register_all();

    if ((ret = avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, dst_filename)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error allocating output format context: %s\n", err_buf);
        goto end;
    }

    enc = avcodec_find_encoder_by_name("libx264");
    if (!enc) {
        fprintf(stderr, "Error finding encoder\n");
        goto end;
    }

    enc_ctx = avcodec_alloc_context3(enc);
    if (!enc_ctx) {
        fprintf(stderr, "Error allocating codec context\n");
        goto end;
    }

    enc_ctx->width = width;
    enc_ctx->height = height;
    enc_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc_ctx->time_base = (AVRational){ 1, fps };
    enc_ctx->framerate = (AVRational){ fps, 1 };

    if ((ret = avcodec_open2(enc_ctx, enc, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening encoder: %s\n", err_buf);
        goto end;
    }

    AVStream* out_stream = avformat_new_stream(out_fmt_ctx, NULL);
    if (!out_stream) {
        fprintf(stderr, "Error creating new output stream\n");
        goto end;
    }

    out_stream->codecpar->codec_id = enc->id;
    out_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    out_stream->codecpar->width = width;
    out_stream->codecpar->height = height;
    out_stream->codecpar->format = enc_ctx->pix_fmt;
    out_stream->codecpar->bit_rate = 4000000;
    av_dict_set(&out_stream->metadata, "title", "My video", 0);

    if ((ret = avio_open(&out_fmt_ctx->pb, dst_filename, AVIO_FLAG_WRITE)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening output file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_write_header(out_fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error writing header: %s\n", err_buf);
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating frame\n");
        goto end;
    }

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    return 0;
}

int decode_audio(const char* src_filename, const char* dst_filename)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* dec = NULL;
    AVPacket pkt;
    AVFrame* frame = NULL;
    int stream_index;
    int sample_rate, channels;
    int i, j;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (stream_index < 0) {
        fprintf(stderr, "Error finding audio stream\n");
        goto end;
    }

    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx) {
        fprintf(stderr, "Error allocating codec context\n");
        goto end;
    }

    if ((ret = avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[stream_index]->codecpar)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error copying codec parameters: %s\n", err_buf);
        goto end;
    }

    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening codec: %s\n", err_buf);
        goto end;
    }

    sample_rate = dec_ctx->sample_rate;
    channels = av_get_channel_layout_nb_channels(dec_ctx->channel_layout);

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Error allocating frame\n");
        goto end;
    }

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    FILE* fp = fopen(dst_filename, "wb");
    if (!fp) {
        fprintf(stderr, "Error opening output file\n");
        goto end;
    }

    while (av_read_frame(fmt_ctx, &pkt) >= 0) {
        if (pkt.stream_index == stream_index) {
            ret = avcodec_send_packet(dec_ctx, &pkt);
            if (ret < 0) {
                av_strerror(ret, err_buf, ERR_BUFF_SIZE);
                fprintf(stderr, "Error sending packet to decoder: %s\n", err_buf);
                goto end;
            }
        }
    }

    return 0;
}

int extract_audio(const char* src_filename, const char* dst_filename)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* dec_ctx = NULL;
    AVCodec* dec = NULL;
    AVPacket pkt;
    AVFrame* frame = NULL;
    FILE* dst_file = NULL;
    int stream_index;
    int got_frame;
    int total_samples = 0;
    int64_t dst_ch_layout = AV_CH_LAYOUT_STEREO;
    int dst_nb_samples;
    int dst_linesize;
    int max_dst_nb_samples;
    uint8_t** dst_data = NULL;
    AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;
    SwrContext* swr_ctx = NULL;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening input file: %s\n", err_buf);
        goto end;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error finding stream info: %s\n", err_buf);
        goto end;
    }

    av_dump_format(fmt_ctx, 0, src_filename, 0);

    stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &dec, 0);
    if (stream_index < 0) {
        fprintf(stderr, "Error finding audio stream\n");
        goto end;
    }

    dec_ctx = avcodec_alloc_context3(dec);
    if (!dec_ctx) {
        fprintf(stderr, "Error allocating codec context\n");
        goto end;
    }

    if ((ret = avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[stream_index]->codecpar)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error setting codec parameters: %s\n", err_buf);
        goto end;
    }

    if ((ret = avcodec_open2(dec_ctx, dec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening codec: %s\n", err_buf);
        goto end;
    }

    if (!(frame = av_frame_alloc())) {
        fprintf(stderr, "Error allocating frame\n");
        goto end;
    }

    if ((ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dec_ctx->channels, 1024, dst_sample_fmt, 0)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error allocating destination samples: %s\n", err_buf);
        goto end;
    }

    max_dst_nb_samples = 1024;
    dst_nb_samples = max_dst_nb_samples;

    return 0;
}

int generate_color_bars(const char* filename, int width, int height, int duration)
{
    int ret;
    char err_buf[ERR_BUFF_SIZE];
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* codec_ctx = NULL;
    AVCodec* codec = NULL;
    AVPacket pkt;
    AVFrame* frame = NULL;
    uint8_t* data = NULL;
    int size;
    int stream_index;
    int i, j, k;
    int r, g, b;
    int64_t pts = 0;
    const AVRational frame_rate = { 25, 1 };
    const AVRational time_base = { 1, AV_TIME_BASE };

    av_register_all();

    if ((ret = avformat_alloc_output_context2(&fmt_ctx, NULL, NULL, filename)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error creating output context: %s\n", err_buf);
        goto end;
    }

    codec = avcodec_find_encoder_by_name("libx264");
    if (!codec) {
        fprintf(stderr, "Error finding H.264 encoder\n");
        goto end;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        fprintf(stderr, "Error allocating codec context\n");
        goto end;
    }

    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->time_base = frame_rate;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_ctx->gop_size = 12;
    codec_ctx->max_b_frames = 2;

    if ((ret = avcodec_open2(codec_ctx, codec, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error opening codec: %s\n", err_buf);
        goto end;
    }

    if (!(fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        if ((ret = avio_open(&fmt_ctx->pb, filename, AVIO_FLAG_WRITE)) < 0) {
            av_strerror(ret, err_buf, ERR_BUFF_SIZE);
            fprintf(stderr, "Error opening output file: %s\n", err_buf);
            goto end;
        }
    }

    if ((ret = avformat_write_header(fmt_ctx, NULL)) < 0) {
        av_strerror(ret, err_buf, ERR_BUFF_SIZE);
        fprintf(stderr, "Error writing header: %s\n", err_buf);
        goto end;
    }

    size = av_image_get_buffer_size(codec_ctx->pix_fmt, width, height, 1);
    data = av_malloc(size);
    av_image_fill_arrays(frame->data, frame->linesize, data, codec_ctx->pix_fmt, width, height, 1);

    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    return 0;
}
