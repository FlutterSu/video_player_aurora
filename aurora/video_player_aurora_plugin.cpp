#include <video_player_aurora/video_player_aurora_plugin.h>
#include <sys/utsname.h>

void VideoPlayerAuroraPlugin::RegisterWithRegistrar(
    PluginRegistrar& registrar) {
}
void VideoPlayerAuroraPlugin::HandleMethodCall(const MethodCall& method_call) {
}
void VideoPlayerAuroraPlugin::OnMethodCall(const MethodCall& call) {
  HandleMethodCall(call);
}

void VideoPlayerAuroraPlugin::unimplemented(const MethodCall& call) {
  call.SendSuccessResponse(nullptr);
}