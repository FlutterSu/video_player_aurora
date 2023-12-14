#ifndef FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_
#define FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_

#include <video_player_aurora/globals.h>
#include <flutter/plugin-interface.h>
#include <flutter/basic-message-channel.h>
#include <flutter/event-channel.h>
#include <flutter/texture-variant.h>

class PLUGIN_EXPORT VideoPlayerAuroraPlugin final : public PluginInterface {
 public:
  void RegisterWithRegistrar(PluginRegistrar& registrar) override;
  
  VideoPlayerAuroraPlugin(PluginRegistrar* plugin_registrar,
                    TextureRegistrar* texture_registrar);
  virtual ~VideoPlayerAuroraPlugin();

private:
 struct FlutterVideoPlayer {
    int64_t texture_id;
    std::unique_ptr<GstVideoPlayer> player;
    std::unique_ptr<BufferVariant> texture;
    std::unique_ptr<FlutterPixelBuffer> buffer;
#ifdef USE_EGL_IMAGE_DMABUF
    std::unique_ptr<FlutterDesktopEGLImage> egl_image;
#endif  // USE_EGL_IMAGE_DMABUF
    std::unique_ptr<EventChannel>
        event_channel;
    // std::unique_ptr<flutter::EventSink<Encodable>> event_sink;
  };

  void HandleInitializeMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleCreateMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleDisposeMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandlePauseMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandlePlayMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleSetLoopingMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleSetVolumeMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleSetMixWithOthersMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleSetPlaybackSpeedMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandleSeekToMethodCall(
      const Encodable& message,
      MessageReply reply);
  void HandlePositionMethodCall(
      const Encodable& message,
      MessageReply reply);

  void SendInitializedEventMessage(int64_t texture_id);
  void SendPlayCompletedEventMessage(int64_t texture_id);
  void SendIsPlayingStateUpdate(int64_t texture_id, bool is_playing);

  Encodable WrapError(const std::string& message,
                      const std::string& code = std::string(),
                      const std::string& details = std::string());

  const std::string GetExecutableDirectory();

  void HandleMethodCall(const MethodCall& method_call);

  void OnMethodCall(const MethodCall& call);
  void unimplemented(const MethodCall& call);

  PluginRegistrar* plugin_registrar_;
  TextureRegistrar* texture_registrar_;
  std::unordered_map<int64_t, std::unique_ptr<FlutterVideoPlayer>> players_;
};

#endif FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_