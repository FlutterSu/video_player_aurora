// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_MIX_WITH_OTHERS_MESSAGE_H_
#define PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_MIX_WITH_OTHERS_MESSAGE_H_

#include <flutter/encodable.h>

class MixWithOthersMessage {
 public:
  MixWithOthersMessage() = default;
  ~MixWithOthersMessage() = default;

  // Prevent copying.
  MixWithOthersMessage(MixWithOthersMessage const&) = default;
  MixWithOthersMessage& operator=(MixWithOthersMessage const&) = default;

  void SetMixWithOthers(bool mixWithOthers) {
    mix_with_others_ = mixWithOthers;
  }

  bool GetMixWithOthers() const { return mix_with_others_; }

  Encodable::Map ToMap() {
    Encodable::Map map = {
        {"mixWithOthers", mix_with_others_}};

    return Encodable::Map(map);
  }

  static MixWithOthersMessage FromMap(const Encodable& value) {
    MixWithOthersMessage message;
    if (std::holds_alternative<Encodable::Map>(value)) {
      auto map = std::get<Encodable::Map>(value);

      Encodable& mixWithOthers =
          map[Encodable::String("mixWithOthers")];
      if (std::holds_alternative<bool>(mixWithOthers)) {
        message.SetMixWithOthers(std::get<bool>(mixWithOthers));
      }
    }

    return message;
  }

 private:
  bool mix_with_others_ = false;
};

#endif  // PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_MIX_WITH_OTHERS_MESSAGE_H_