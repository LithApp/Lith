Lith - Multiplatform WeeChat relay client
====

Lith is a project mostly focused on bringing a fast and user-friendly WeeChat client to all major desktop and mobile
operating systems. The main focus of this project is to bring a full-featured native relay client to iOS but all major
mobile and desktop platforms are supported to some degree.

![Lith in Linux](https://github.com/LithApp/LithApp.github.io/raw/main/assets/main.png)

## Getting Started

Lith is a utility application for [WeeChat](https://weechat.org) (not to be confused with WeChat), a terminal chat
application mostly focused on the IRC protocol. It is a only a graphical relay for messages that are actually received
and sent by WeeChat. That means you need to prepare a WeeChat instance on on your computer or server (or have a
provider do that for you) to be actually able to use Lith in any meaningful way.

For setup on the WeeChat side, please consult the following sources:
 - [WeeChat official user documentation](https://weechat.org/files/doc/stable/weechat_user.en.html#relay_plugin)

On desktop systems, you can run WeeChat on the same device as Lith but the main focus of this project is to handle
remote connections, especially on mobile platforms and iOS in particular. The remote WeeChat server needs to be
directly over the internet either through a TCP socket or WebSockets.

There is also a browser version of Lith you can try right now thanks to WebAssembly: [https://lith.app/Lith/](https://lith.app/Lith/).

### Features

- Inline preview of images and videos
- Fast buffer list filtering
- Dark and light mode support
- Native local notifications on desktop platforms
- Secure storage of login credentials

### Limitations

- Message text is not selectable
- Keyboard shortcuts can only be tweaked by editing the config file
- Android: Builds are not tested at all at this moment, not signed, currently not installable without signing
- macOS: Builds are not signed + Some keyboard shortcuts are broken
- Windows: Builds are not signed + There is no installer
- WebAssembly: Builds are pretty large (around 12MB at this moment), there are keyboard focus issues and video preview support is not supported.
- More: https://github.com/LithApp/Lith/issues

### Future plans

- Full App Store release
- Push notification support (first on iOS, then Android, probably)

## Binary Packages

Lith currently supports the following platforms:
 - (Usable) **iOS** and **iPadOS**:
    - RECOMMENDED: https://testflight.apple.com/join/QfcVgNHD - "Stable" track - releases ending with .0
    - https://testflight.apple.com/join/xS3hWrmj - "Unstable" track - more or less everything I tag, may be messy
 - (build-only) Android: https://github.com/LithApp/Lith/releases
 - (Usable) **Linux**: https://flathub.org/apps/details/app.lith.Lith
 - (Usable, not tested) Windows: https://github.com/LithApp/Lith/releases
 - (Usable, not tested) macOS: https://github.com/LithApp/Lith/releases
 - (Not tested, feature gaps) WebAssembly: https://lith.app/Lith/


## Build instructions

Lith is written using C++ and the Qt framework with QML. The only supported version is Qt 6.5 and newer. Other
dependencies are listed below.

When building for all supported platforms, only the following steps should be necessary if CMake is setup correctly:

```
mkdir build && cd build
cmake ..
cmake --build .
```

Alternatively, you can just open the project file in Qt Creator.

There is also a package for Arch Linux in the AUR: https://aur.archlinux.org/packages/lith-git

### Dependencies

Besides Qt, Lith currently depends on the packages listed below. You don't need to install them manually. This can be
controlled, see the section below describing the CMake options to choose which packages to use.

Qt always needs to be installed outside of Lith's build system. Other dependencies are:

- [qtkeychain](https://github.com/frankosterfeld/qtkeychain/) - Used to store login info (host, port, passphrase, ...).
- [QCoro](https://github.com/danvratil/qcoro) - Simplifies programming asynchronous code with Qt C++ coroutine
                                                integration.
- [qhot](https://github.com/patrickelectric/qhot) - (Development only) Provides hot reload for QML, I use this to speed
                                                     up UI feature development.

### CMake options

- `LITH_FEATURE_KEYCHAIN` - `ON` by default. Enables keychain integration on Linux, macOS, Windows, iOS and Android.
- `LITH_FEATURE_QHOT` - `OFF` by default. Builds internal `qhot` instance and adds a target to automatically run Lith
                        in `qhot`. Not useful for end users.
- `LITH_FORCE_DOWNLOAD_DEPENDENCIES` - `OFF` by default. Makes CPM go for downloading packages from
                                       GitHub without checking local system. This option takes preference over
                                       `LITH_FORCE_LOCAL_PACKAGES_ONLY`.
- `LITH_FORCE_LOCAL_PACKAGES_ONLY` - `OFF` by default. Enforces usage of only packages installed on local
                                     system.

## Get in touch

For bug reports and questions, feel free to use the Issues page here on GitHub.

There's also an IRC channel: `#lith` on [libera.chat](https://libera.chat) (irc.libera.chat:6697 with SSL)

You can contact me directly on `m@rtinbriza.cz`
