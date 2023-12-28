# video_player_aurora

The implementation of the Video Player plugin for Aurora OS. APIs are designed to be API compatible with the the official [`video_player`](https://github.com/flutter/packages/tree/main/packages/video_player/video_player).

![image](https://user-images.githubusercontent.com/62131389/124210378-43f06400-db26-11eb-8723-40dad0eb67b0.png)

## Required libraries

This plugin uses [GStreamer](https://gstreamer.freedesktop.org/) internally.

```Shell
# Install as needed.
$ aurora_psdk sb2 -t Aurora_OS-4.0.2.249-armv7hl.default -m sdk-install -R zypper in gstreamer1.0-plugins-base-devel
```

## Usage

### pubspec.yaml
```yaml
dependencies:
  video_player: ^2.6.1
  video_player_aurora:
    git:
      url: https://gitlab.com/alexsherkhan/video_player_aurora
      path: ./
      ref: main
```

### Source code

Import `video_player` in your Dart code:
```dart
import 'package:video_player/video_player.dart';
```

### Enable GstEGLImage

If GstEGLImage is enabled on your target device, adding the following code to `<user's project>/aurora/CMakeLists.txt` may improve playback performance.

```
add_definitions(-DUSE_EGL_IMAGE_DMABUF)
set(USE_EGL_IMAGE_DMABUF "on")
```

### Customize for your target devices

To improve the performance of this plugin, you will need to customize the pipeline in the source file. Please modify the source file and replace the `videoconvert` element with a H/W accelerated element of your target device to perform well.

`bool GstVideoPlayer::CreatePipeline()` in packages/video_player/aurora/gst_video_player.cc

#### default:

```
playbin uri=<file> video-sink="videoconvert ! video/x-raw,format=RGBA ! fakesink"
```

#### i.MX 8M platforms:

```
playbin uri=<file> video-sink="imxvideoconvert_g2d ! video/x-raw,format=RGBA ! fakesink"
```
