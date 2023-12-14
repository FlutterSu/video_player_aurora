// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2021 Sony Group Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
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