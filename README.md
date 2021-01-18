Lith - Multiplatform WeeChat relay client
====

Lith is a project mostly focused on bringing a fast and user-friendly WeeChat client to all major desktop and mobile operating systems. The main focus of this project is to bring a full-featured native relay client to iOS (due to the lack of a native client there).

![Lith in Linux](https://github.com/LithApp/LithApp.github.io/raw/main/assets/main.png)

## Getting Started

Lith is supposed to be run in addition to an existing WeeChat instance. In case you're planning to run Lith on your desktop-type device, WeeChat can be running locally and in case of both of mobile and desktop systems, WeeChat can be connected to over the network. In this case, it's necessary for WeeChat server to be directly reachable from your Lith device directly over the network.

Setup is necessary on the WeeChat side, please consult the following sources:
 - [WeeChat official documentation](https://weechat.org/files/doc/stable/weechat_user.en.html#relay_plugin)
 - [A simplified guide with just the necessary commands](https://gist.github.com/miblodelcarpio/20da48bcdfb55c0860c44fd42d1b9647)

### Binary Packages

Lith currently supports the following platforms:
 - (Beta) **iOS** and **iPadOS**: https://testflight.apple.com/join/QfcVgNHD (AppStore version coming soon)
 - (Alpha) Android: https://github.com/LithApp/Lith/releases
 - (Beta) **Linux**: https://flathub.org/apps/app.lith.Lith
 - (Alpha) Windows: https://github.com/LithApp/Lith/releases
 - (Alpha) macOS: https://github.com/LithApp/Lith/releases
 
## Features

- Inline preview of images and videos
- Fast buffer list filtering

## Limitations

- Handshake protocol (introduced in WeeChat 2.9) support is missing
- Data compression support is missing
- Message text is not selectable
- Android: Builds are not signed
- macOS: Builds are not signed + Some keyboard shortcuts are broken
- Windows: Builds are not signed + There is no installer

## Future plans

- Full App Store release
- Push notification support (first on iOS, then Android, probably)

## Build instructions

Lith is written using C++ and the Qt framework with QML. The only supported version is Qt 5.15.

When building for all supported platforms, only the following steps should be necessary (provided you have set up your Qt environment correctly):
```
mkdir build && cd build
qmake ..
make # nmake on Windows
```
Alternatively, you can just open the project file in Qt Creator.
