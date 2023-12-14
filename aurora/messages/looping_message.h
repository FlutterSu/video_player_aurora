// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_LOOPING_MESSAGE_H_
#define PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_LOOPING_MESSAGE_H_

#include <flutter/encodable.h>

class LoopingMessage {
 public:
  LoopingMessage() = default;
  ~LoopingMessage() = default;

  // Prevent copying.
  LoopingMessage(LoopingMessage const&) = default;
  LoopingMessage& operator=(LoopingMessage const&) = default;

  void SetTextureId(int64_t texture_id) { texture_id_ = texture_id; }

  int64_t GetTextureId() const { return texture_id_; }

  void SetIsLooping(bool is_looping) { is_looping_ = is_looping; }

  bool GetIsLooping() const { return is_looping_; }

  Encodable::Map ToMap() {
    Encodable::Map map = {
        {"textureId", Encodable::Int(texture_id_)},
        {"isLooping", Encodable::Boolean(is_looping_)}};
    return Encodable::Map(map);
  }

  static LoopingMessage FromMap(const Encodable& value) {
    LoopingMessage message;
    if (std::holds_alternative<Encodable::Map>(value)) {
      auto map = std::get<Encodable::Map>(value);

      Encodable& texture_id =
          map[Encodable::String("textureId")];
      if (std::holds_alternative<int32_t>(texture_id) ||
          std::holds_alternative<int64_t>(texture_id)) {
        message.SetTextureId(texture_id.GetInt());
      }

      Encodable& is_looping =
          map[Encodable::String("isLooping")];
      if (std::holds_alternative<bool>(is_looping)) {
        message.SetIsLooping(std::get<bool>(is_looping));
      }
    }

    return message;
  }

 private:
  int64_t texture_id_ = 0;
  bool is_looping_ = false;
};

#endif  // PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_LOOPING_MESSAGE_H_