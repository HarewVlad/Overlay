struct VideoManager {
  bool Initialize(int width, int height);
  bool StartRecording();
  bool RecordFrame(void *data, int stride);
  bool StopRecording();
  void Shutdown();

  AVFrame *m_frame;
  AVCodecContext *m_codec_context;
  SwsContext *m_sws_context;
  AVFormatContext *m_output_format_context;
  AVOutputFormat *m_output_format;
  int m_frame_count;
};