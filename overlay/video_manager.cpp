int RoundDown2(int value) {
  return value - value % 2;
}

bool VideoManager::Initialize(int width, int height) {
  const char *filename = "video.mp4";

  av_register_all();
  avcodec_register_all();

  m_output_format = av_guess_format(nullptr, filename, nullptr);
  if (!m_output_format) {
    return false;
  }

  int result = avformat_alloc_output_context2(&m_output_format_context, m_output_format, nullptr, filename);
  if (result) {
    return false;
  }

  AVCodec *codec = avcodec_find_encoder(m_output_format->video_codec);
  if (!codec) {
    return false;
  }

  AVStream *stream = avformat_new_stream(m_output_format_context, codec);
  if (!stream) {
    return false;
  }

  m_codec_context = avcodec_alloc_context3(codec);
  if (!m_codec_context) {
    return false;
  }

  stream->codecpar->codec_id = m_output_format->video_codec;
  stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  stream->codecpar->width = RoundDown2(width);
  stream->codecpar->height = RoundDown2(height);
  stream->codecpar->format = AV_PIX_FMT_YUV420P;
  stream->codecpar->bit_rate = 2000000;
  avcodec_parameters_to_context(m_codec_context, stream->codecpar);
  m_codec_context->time_base.num = 1;
  m_codec_context->time_base.den = 1;
  m_codec_context->max_b_frames = 2;
  m_codec_context->gop_size = 10;
  m_codec_context->framerate.num = 60;
  m_codec_context->framerate.den = 1;

  if (stream->codecpar->codec_id == AV_CODEC_ID_H264) {
    av_opt_set(m_codec_context, "preset", "ultrafast", 0);
  }
  else if (stream->codecpar->codec_id == AV_CODEC_ID_H265)
  {
    av_opt_set(m_codec_context, "preset", "ultrafast", 0);
  }

  avcodec_parameters_from_context(stream->codecpar, m_codec_context);

  result = avcodec_open2(m_codec_context, codec, NULL);
  if (result < 0) {
    return false;
  }

  if (!(m_output_format->flags & AVFMT_NOFILE)) {
    result = avio_open(&m_output_format_context->pb, filename, AVIO_FLAG_WRITE);
    if (result < 0) {
      return false;
    }
  }

  result = avformat_write_header(m_output_format_context, NULL);
  if (result < 0) {
    return false;
  }

  av_dump_format(m_output_format_context, 0, filename, 1);

  m_frame = av_frame_alloc();
  if (!m_frame) {
    return false;
  }

  m_frame->format = AV_PIX_FMT_YUV420P;
  m_frame->width = m_codec_context->width;
  m_frame->height = m_codec_context->height;

  result = av_frame_get_buffer(m_frame, 32);
  if (result < 0) {
    return false;
  }

  m_sws_context = sws_getContext(m_codec_context->width, m_codec_context->height, 
    AV_PIX_FMT_RGBA, m_codec_context->width, m_codec_context->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);
  if (!m_sws_context) {
    return false;
  }

  return true;
}

bool VideoManager::StartRecording() {
  m_frame_count = 0;

  return true;
}

bool VideoManager::RecordFrame(void *data, int stride) {
  int source_linesize[1] = {stride};
  sws_scale(m_sws_context, (const uint8_t *const *)&data, source_linesize, 0, m_codec_context->height,
        m_frame->data, m_frame->linesize);
  m_frame->pts = (1.0 / 60) * 90000 * (m_frame_count++);

  int result = avcodec_send_frame(m_codec_context, m_frame);
  if (result < 0) {
    return false;
  }

  AV_TIME_BASE;
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
  pkt.flags |= AV_PKT_FLAG_KEY;

  if (avcodec_receive_packet(m_codec_context, &pkt) == 0) {
    av_interleaved_write_frame(m_output_format_context, &pkt);
    av_packet_unref(&pkt);
  }

  return true;
}

bool VideoManager::StopRecording() {
  // Delayed frames
  AVPacket pkt;
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

  for (;;) {
    avcodec_send_frame(m_codec_context, NULL);
    if (avcodec_receive_packet(m_codec_context, &pkt) == 0) {
      av_interleaved_write_frame(m_output_format_context, &pkt);
      av_packet_unref(&pkt);
    } else {
      break;
    }
  }

  av_write_trailer(m_output_format_context);
  if (!(m_output_format_context->flags & AVFMT_NOFILE)) {
    int result = avio_close(m_output_format_context->pb);
    if (result < 0) {
      return false;
    }
  }

  return true;
}

// TODO: Move to different file
void VideoTest() {
  // Video video;
  // video.Initialize(1920, 1080);

  // uint8_t* frameraw = new uint8_t[1920 * 1080 * 4];
  // memset(frameraw, 222, 1920 * 1080 * 4);

  // video.StartRecording();
  // for (int i = 0; i < 60; ++i) {
  //   video.RecordFrame(frameraw);
  // }

  // video.StopRecording();
  // video.Shutdown();
}

void VideoManager::Shutdown() {
  av_frame_free(&m_frame);
  avcodec_free_context(&m_codec_context);
  avformat_free_context(m_output_format_context);
  sws_freeContext(m_sws_context);
}