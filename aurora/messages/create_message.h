// Copyright 2023 Alexander Syrykh. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_CREATE_MESSAGE_H_
#define PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_CREATE_MESSAGE_H_

#include <flutter/encodable.h>

class CreateMessage {
 public:
  CreateMessage() = default;
  ~CreateMessage() = default;

  // Prevent copying.
  CreateMessage(CreateMessage const&) = default;
  CreateMessage& operator=(CreateMessage const&) = default;

  void SetAsset(const std::string& asset) { asset_ = asset; }

  std::string GetAsset() const { return asset_; }

  void SetUri(const std::string& uri) { uri_ = uri; }

  std::string GetUri() const { return uri_; }

  void SetPackageName(const std::string& packageName) {
    package_name_ = packageName;
  }

  std::string GetPackageName() const { return package_name_; }

  void SetFormatHint(const std::string& formatHint) {
    format_hint_ = formatHint;
  }

  std::string GetFormatHint() const { return format_hint_; }

  Encodable::Map ToMap() {
    // todo: Add httpHeaders.
    Encodable::Map map = {
        {"asset",Encodable::String(asset_)},
        {"uri", Encodable::String(uri_)},
        {"packageName", Encodable::String(package_name_)},
        {"formatHint",Encodable::String(format_hint_)}};
    return Encodable::Map(map);
  }

  static CreateMessage FromMap(const Encodable& value) {
    CreateMessage message;
    if (std::holds_alternative<Encodable::Map>(value)) {
      auto map = std::get<Encodable::Map>(value);

      Encodable& asset = map[Encodable::String("asset")];
      if (std::holds_alternative<std::string>(asset)) {
        message.SetAsset(std::get<std::string>(asset));
      }

      Encodable& uri = map[Encodable::String("uri")];
      if (std::holds_alternative<std::string>(uri)) {
        message.SetUri(std::get<std::string>(uri));
      }

      Encodable& packageName =
          map[Encodable::String("packageName")];
      if (std::holds_alternative<std::string>(packageName)) {
        message.SetPackageName(std::get<std::string>(packageName));
      }

      Encodable& formatHint =
          map[Encodable::String("formatHint")];
      if (std::holds_alternative<std::string>(formatHint)) {
        message.SetFormatHint(std::get<std::string>(formatHint));
      }
    }

    return message;
  }

 private:
  std::string asset_;
  std::string uri_;
  std::string package_name_;
  std::string format_hint_;
};

#endif  // PACKAGES_VIDEO_PLAYER_VIDEO_PLAYER_AURORA_MESSAGES_CREATE_MESSAGE_H_