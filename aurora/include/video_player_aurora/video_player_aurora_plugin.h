#ifndef FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_
#define FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_

#include <video_player_aurora/globals.h>
#include <flutter/plugin-interface.h>

class PLUGIN_EXPORT VideoPlayerAuroraPlugin final : public PluginInterface {
 public:
  void RegisterWithRegistrar(PluginRegistrar& registrar) override;
 private:

  void HandleMethodCall(const MethodCall& method_call);

  void OnMethodCall(const MethodCall& call);
  void unimplemented(const MethodCall& call);
};

#endif FLUTTER_PLUGIN_VIDEO_PLAYER_AURORA_PLUGIN_H_