struct VideoManager {
  bool Initialize();
  bool StartRecording(int width, int height);
  bool RecordFrame(void *data, int stride);
  bool StopRecording();
  void Shutdown();

  AVFrame *m_frame;
  AVCodecContext *m_codec_context;
  SwsContext *m_sws_context;
  AVFormatContext *m_output_format_context;
  AVOutputFormat *m_output_format;
  const char *m_filename = "video.mp4";
  int m_frame_count;
};