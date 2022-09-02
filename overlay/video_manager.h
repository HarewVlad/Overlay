struct VideoManager {
  bool Initialize();
  bool StartRecording(DXGI_FORMAT format, int width, int height);
  AVPixelFormat DXGIFormatToAVFormat(DXGI_FORMAT format);
  bool RecordFrame(void *data, int stride);
  bool StopRecording();

  AVFrame *m_frame;
  AVCodecContext *m_codec_context;
  SwsContext *m_sws_context;
  AVFormatContext *m_output_format_context;
  const AVOutputFormat *m_output_format;
  int m_frame_count;
};