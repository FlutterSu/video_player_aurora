// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_POSITION_MESSAGE_H_
#define PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_POSITION_MESSAGE_H_

#include <flutter/encodable.h>

class PositionMessage {
 public:
  PositionMessage() = default;
  ~PositionMessage() = default;

  // Prevent copying.
  PositionMessage(PositionMessage const&) = default;
  PositionMessage& operator=(PositionMessage const&) = default;

  void SetTextureId(int64_t texture_id) { texture_id_ = texture_id; }

  int64_t GetTextureId() const { return texture_id_; }

  void SetPosition(int64_t position) { position_ = position; }

  int64_t GetPosition() const { return position_; }

  Encodable::Map ToMap() {
    Encodable::Map toMapResult = {
        {"textureId", Encodable::Int(texture_id_)},
        {"position", Encodable::Int(position_)}};

    return Encodable::Map(toMapResult);
  }

  static PositionMessage FromMap(const Encodable& value) {
    PositionMessage message;
    if (std::holds_alternative<Encodable::Map>(value)) {
      auto map = std::get<Encodable::Map>(value);

      Encodable& texture_id =
          map[Encodable::String("textureId")];
      if (std::holds_alternative<int64_t>(texture_id)) {
        message.SetTextureId(std::get<int64_t>(texture_id));
      }

      Encodable& position =
          map[Encodable::String("position")];
      if (std::holds_alternative<int64_t>(position)) {
        message.SetPosition(std::get<int64_t>(position));
      }
    }

    return message;
  }

 private:
  int64_t texture_id_ = 0;
  int64_t position_ = 0;
};

#endif  // PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_POSITION_MESSAGE_H_