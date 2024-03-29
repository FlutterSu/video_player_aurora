// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2021 Sony Group Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <video_player_aurora/video_player_aurora_plugin.h>

#include <flutter/basic-message-channel.h>
#include <flutter/encodable.h>
#include <flutter/event-channel.h>
#include <flutter/method-channel.h>
#include <flutter/plugin-registrar.h>
#include <flutter/message-codec-type.h>
#include <flutter/method-codec-type.h>
#include <unistd.h>

#include <unordered_map>
#include <filesystem>

#include "gst_video_player.h"
#include "messages/messages.h"
#include "video_player_stream_handler_impl.h"

constexpr char kVideoPlayerApiChannelInitializeName[] =
    "dev.flutter.pigeon.VideoPlayerApi.initialize";
constexpr char kVideoPlayerApiChannelSetMixWithOthersName[] =
    "dev.flutter.pigeon.VideoPlayerApi.setMixWithOthers";
constexpr char kVideoPlayerApiChannelCreateName[] =
    "dev.flutter.pigeon.VideoPlayerApi.create";
constexpr char kVideoPlayerApiChannelDisposeName[] =
    "dev.flutter.pigeon.VideoPlayerApi.dispose";
constexpr char kVideoPlayerApiChannelSetLoopingName[] =
    "dev.flutter.pigeon.VideoPlayerApi.setLooping";
constexpr char kVideoPlayerApiChannelSetVolumeName[] =
    "dev.flutter.pigeon.VideoPlayerApi.setVolume";
constexpr char kVideoPlayerApiChannelPauseName[] =
    "dev.flutter.pigeon.VideoPlayerApi.pause";
constexpr char kVideoPlayerApiChannelPlayName[] =
    "dev.flutter.pigeon.VideoPlayerApi.play";
constexpr char kVideoPlayerApiChannelPositionName[] =
    "dev.flutter.pigeon.VideoPlayerApi.position";
constexpr char kVideoPlayerApiChannelSetPlaybackSpeedName[] =
    "dev.flutter.pigeon.VideoPlayerApi.setPlaybackSpeed";
constexpr char kVideoPlayerApiChannelSeekToName[] =
    "dev.flutter.pigeon.VideoPlayerApi.seekTo";

constexpr char kVideoPlayerVideoEventsChannelName[] =
    "flutter.io/videoPlayer/videoEvents";

constexpr char kEncodableMapkeyResult[] = "result";
constexpr char kEncodableMapkeyError[] = "error";


VideoPlayerAuroraPlugin::VideoPlayerAuroraPlugin()
{        
  // Needs to call 'gst_init' that initializing the GStreamer library before
  // using it.
  GstVideoPlayer::GstLibraryLoad();
}

VideoPlayerAuroraPlugin::~VideoPlayerAuroraPlugin() {
    for (auto itr = players_.begin(); itr != players_.end(); itr++) {
      auto texture_id = itr->first;
      auto* player = itr->second.get();
      player->player = nullptr;
      player->buffer = nullptr;
      texture_registrar_->UnregisterTexture(texture_id);
    }
    players_.clear();

    GstVideoPlayer::GstLibraryUnload();
}

void VideoPlayerAuroraPlugin::RegisterWithRegistrar(
    PluginRegistrar& registrar) {
      
    plugin_registrar_ = &(registrar);
    texture_registrar_ = registrar.GetTextureRegistrar();

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelInitializeName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleInitializeMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelCreateName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleCreateMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelDisposeName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleDisposeMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelPauseName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandlePauseMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelPlayName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandlePlayMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelSetLoopingName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleSetLoopingMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelSetVolumeName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleSetVolumeMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelSetMixWithOthersName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleSetMixWithOthersMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelSetPlaybackSpeedName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleSetPlaybackSpeedMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelSeekToName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandleSeekToMethodCall(message);});

    plugin_registrar_->RegisterBasicMessageChannel(kVideoPlayerApiChannelPositionName,
    MessageCodecType::Standard,
    [this](const BasicMessage &message) { 
      this->HandlePositionMethodCall(message);});
}

void VideoPlayerAuroraPlugin::unimplemented(const MethodCall& call) {
  call.SendSuccessResponse(nullptr);
}

void VideoPlayerAuroraPlugin::HandleInitializeMethodCall(
    const BasicMessage& message) {
  Encodable::Map result;

  BasicMessage reply = message;

  result.emplace(Encodable::String(kEncodableMapkeyResult),
                 Encodable::Null());
  reply.SendResponse(Encodable::Map(result));
}

void VideoPlayerAuroraPlugin::HandleCreateMethodCall(
    const BasicMessage& message) {
  auto meta = CreateMessage::FromMap(message.GetValue());
  std::string uri;
  if (!meta.GetAsset().empty()) {
    // todo: gets propery path of the Flutter project.
    std::string flutter_project_path = GetExecutableDirectory() + "/";
    uri = "file://"+ flutter_project_path +"flutter_assets/"+ meta.GetAsset();
  } else {
    uri = meta.GetUri();
  }

  auto instance = std::make_unique<FlutterVideoPlayer>();
  
#ifdef USE_EGL_IMAGE_DMABUF
  // instance->egl_image = std::make_unique<FlutterDesktopEGLImage>();
  // instance->texture =
  //     std::make_unique<flutter::TextureVariant>(flutter::EGLImageTexture(
  //         [instance = instance.get()](
  //             size_t width, size_t height, void* egl_display,
  //             void* egl_context) -> const FlutterDesktopEGLImage* {
  //           instance->egl_image->width = instance->player->GetWidth();
  //           instance->egl_image->height = instance->player->GetHeight();
  //           instance->egl_image->egl_image =
  //               instance->player->GetEGLImage(egl_display, egl_context);
  //           return instance->egl_image.get();
  //         }));
#else
    const auto texture_id =
      texture_registrar_->RegisterTexture(
          [instance = instance.get()](size_t width, size_t height) -> std::optional<BufferVariant> {        
            instance->buffer = std::make_unique<FlutterPixelBuffer>();
            instance->buffer->width = instance->player->GetWidth();
            instance->buffer->height = instance->player->GetHeight();
            instance->buffer->buffer = instance->player->GetFrameBuffer();
            
            if (instance->buffer.get()->buffer && instance->buffer->width != 0 && instance->buffer->height != 0) {
                return std::make_optional(BufferVariant(
                    FlutterPixelBuffer{instance->buffer.get()->buffer, (size_t) instance->buffer->width, (size_t) instance->buffer->height}));
            }      
            return std::nullopt;
      });
  
#endif  // USE_EGL_IMAGE_DMABUF

  instance->texture_id = texture_id;
  {
    auto event_channel =
        std::make_unique<EventChannel>(
            kVideoPlayerVideoEventsChannelName + std::to_string(texture_id),
            MethodCodecType::Standard);
    plugin_registrar_->RegisterEventChannel(
        kVideoPlayerVideoEventsChannelName + std::to_string(texture_id),
        MethodCodecType::Standard,
        [instance = instance.get(), host = this](const Encodable &){
          host->SendInitializedEventMessage(instance->texture_id);
          return EventResponse();
        },
        [instance = instance.get()](const Encodable &){
          return EventResponse();
        }
    );      
    instance->event_channel = std::move(event_channel);  
  }
  {
    auto player_handler = std::make_unique<VideoPlayerStreamHandlerImpl>(
        // OnNotifyInitialized
        [texture_id, host = this]() {
          host->SendInitializedEventMessage(texture_id);
        },
        // OnNotifyFrameDecoded
        [texture_id, host = this]() {
          host->texture_registrar_->MarkTextureAvailable(texture_id);
        },
        // OnNotifyCompleted
        [texture_id, host = this]() {
          host->SendPlayCompletedEventMessage(texture_id);
        },
        [texture_id, host = this](bool is_playing) {
          host->SendIsPlayingStateUpdate(texture_id, is_playing);
        });
    instance->player =
        std::make_unique<GstVideoPlayer>(uri, std::move(player_handler));
    players_[texture_id] = std::move(instance);
  }

  Encodable::Map value;
  TextureMessage result;
  result.SetTextureId(texture_id);
  value.emplace(Encodable::String(kEncodableMapkeyResult),
                result.ToMap());

  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(value));            
}

void VideoPlayerAuroraPlugin::HandleDisposeMethodCall(
    const BasicMessage& message) {
  auto parameter = TextureMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    auto* player = players_[texture_id].get();
    player->player = nullptr;
    player->buffer = nullptr;
    players_.erase(texture_id);
    texture_registrar_->UnregisterTexture(texture_id);

    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandlePauseMethodCall(
    const BasicMessage& message) {
  auto parameter = TextureMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->Pause();
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandlePlayMethodCall(
    const BasicMessage& message) {
  auto parameter = TextureMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->Play();
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandleSetLoopingMethodCall(
    const BasicMessage& message) {
  auto parameter = LoopingMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->SetAutoRepeat(parameter.GetIsLooping());
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandleSetVolumeMethodCall(
    const BasicMessage& message) {
  auto parameter = VolumeMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->SetVolume(parameter.GetVolume());
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandleSetMixWithOthersMethodCall(
    const BasicMessage& message) {
  // todo: implements here.

  Encodable::Map result;
  result.emplace(Encodable::String(kEncodableMapkeyResult),
                 Encodable::Null());
  
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandlePositionMethodCall(
    const BasicMessage& message) {
  auto parameter = TextureMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    auto position = players_[texture_id]->player->GetCurrentPosition();
    if (position < 0) {
      auto error_message = "Failed to get current position with texture id: " +
                           std::to_string(texture_id);
      result.emplace(Encodable::String(kEncodableMapkeyError),
                     Encodable(WrapError(error_message)));
    } else {
      PositionMessage send_message;
      send_message.SetTextureId(texture_id);
      send_message.SetPosition(position);
      result.emplace(Encodable::String(kEncodableMapkeyResult),
                     send_message.ToMap());
    }
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandleSetPlaybackSpeedMethodCall(
    const BasicMessage& message) {
  auto parameter = PlaybackSpeedMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->SetPlaybackRate(parameter.GetSpeed());
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::HandleSeekToMethodCall(
    const BasicMessage& message) {
  auto parameter = PositionMessage::FromMap(message.GetValue());
  const auto texture_id = parameter.GetTextureId();
  Encodable::Map result;

  if (players_.find(texture_id) != players_.end()) {
    players_[texture_id]->player->SetSeek(parameter.GetPosition());
    result.emplace(Encodable::String(kEncodableMapkeyResult),
                   Encodable::Null());
  } else {
    auto error_message = "Couldn't find the player with texture id: " +
                         std::to_string(texture_id);
    result.emplace(Encodable::String(kEncodableMapkeyError),
                   Encodable(WrapError(error_message)));
  }
  BasicMessage reply = message;
  reply.SendResponse(Encodable::Map(result));  
}

void VideoPlayerAuroraPlugin::SendInitializedEventMessage(int64_t texture_id) {
  if (players_.find(texture_id) == players_.end()) {
    return;
  }

  auto duration = players_[texture_id]->player->GetDuration();
  auto width = players_[texture_id]->player->GetWidth();
  auto height = players_[texture_id]->player->GetHeight();
  Encodable::Map encodables = {
      {"event", Encodable::String("initialized")},
      {"duration", Encodable::Int(duration)},
      {"width", Encodable::Int(width)},
      {"height", Encodable::Int(height)}};
  Encodable event(encodables);
  players_[texture_id]->event_channel->SendEvent(event);
}

void VideoPlayerAuroraPlugin::SendPlayCompletedEventMessage(int64_t texture_id) {
  if (players_.find(texture_id) == players_.end()) {
    return;
  }

  Encodable::Map encodables = {
      {"event", Encodable::String("completed")}};
  Encodable event(encodables);
  players_[texture_id]->event_channel->SendEvent(event);
}

void VideoPlayerAuroraPlugin::SendIsPlayingStateUpdate(int64_t texture_id,
                                                 bool is_playing) {
  if (players_.find(texture_id) == players_.end()) {
    return;
  }

  Encodable::Map encodables = {
      {"event", Encodable::String("isPlayingStateUpdate")},
      {"isPlaying", Encodable::Boolean(is_playing)}};
  Encodable event(encodables);
  players_[texture_id]->event_channel->SendEvent(event);
}

Encodable VideoPlayerAuroraPlugin::WrapError(
    const std::string& message, const std::string& code,
    const std::string& details) {
  Encodable::Map map = {
      {"message", Encodable::String(message)},
      {"code", Encodable::String(code)},
      {"details", Encodable::String(details)}};
  return Encodable::Map(map);
}

const std::string VideoPlayerAuroraPlugin::GetExecutableDirectory() {
  
  char* value = getenv("XDG_DATA_DIRS");
  std::string xdgDataDirsEnv;
  if (value){
    xdgDataDirsEnv = std::string(value);
  } else {
    xdgDataDirsEnv = "";
  }

  std::vector<std::string> dirs = {};
  if (xdgDataDirsEnv.empty()) {
    dirs.push_back(std::string("~/.local/share/"));
    dirs.push_back(std::string("/usr/local/share"));
    dirs.push_back(std::string("/usr/share"));
  } else {
    std::string delimiter = ":";
    size_t pos = 0;
    std::string token;

    while ((pos = xdgDataDirsEnv.find(delimiter)) != std::string::npos) {
      token = xdgDataDirsEnv.substr(0, pos);
      dirs.push_back(token);
      xdgDataDirsEnv.erase(0, pos + delimiter.length());
    }
  }
  
  static char buf[1024] = {};
  readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  std::string exe_path = std::string(buf);
  size_t slash_pos = exe_path.find_last_of('/');
  std::string exe_name = exe_path.substr(slash_pos, exe_path.length()-1);
  
  for (auto it = dirs.begin(); it != dirs.end(); ++it)
    if (std::filesystem::is_directory( *(it) + exe_name + "/flutter_assets"))
      return *(it) + exe_name;
 
  return "";
}