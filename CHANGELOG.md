## 1.6.0 (Not released yet)
- Tagging scheme changed, version tags are now prefixed with a `v`
- Versioning scheme changed. Now X.Y.Z releases where Y is even are considered stable. This means:
  - Version 1.6.0 will be stable and 1.6.1 may be its patch release in the future. Development will continue in the `1.6` branch.
  - The 1.7.X series will get tags from the `master` branch until it's time to release 1.8.0 or 2.0.0 with new features. This scheme is used by GNOME, for example.
- Official builds are now built with Qt 6.6.0.
### Breaking changes
- Pixel-based font sizes are now used, this unfortunately means your previous font settings have been reset.
- For some users, server credentials may get lost and will need to be entered again, sorry.
- WeeChat handshake is enabled by default. Disable this if you're still on WeeChat 2.8 or older. 2.9 was released in 2020 so you're better off updating though.
### Bugfixes
- Don't animate BusyIndicator when it's invisible
  - Believe it or not, this changes CPU load on my box from about 8% to not detectable...
- Fix deadlock if notifications service is not registered (@chinese-soup)
- Fix self-signed certificate error ignoring, clean up socket signal connections [#92](https://github.com/LithApp/Lith/issues/92)
- Fix crash coming from the network thread being deleted during shutdown
- Give button icons a size when loading, reduce RAM usage by 10MB
- Load smaller app icon instead of resizing (reduces RAM usage by ~15MB)
- Generate @2x etc. versions of icons to reduce memory load
- Avoid using (alpha) blending as much as possible to improve performance
- Avoid using `clip: true` usages to improve performance
- Fix multiple memory leaks coming from HotList
- Reset the reconnect timer after actually successfully connecting
- Clear the input field first, then send a command to WeeChat, fixes a glitch on slow network
- Add alt+up/down to default buffer switch shorcuts [#133](https://github.com/LithApp/Lith/issues/133)
  - macOS equivalent of this is Option+up/down, added because Option+left/right is a text editing shortcut
- Add a timeout to color scheme switching to prevent doing it multiple times in a row
- Clearing hotlist in another client is now also propagated to Lith
- All direct user inputs interacting with the channel now clear current hotlist.
  - That means if you send a message, kick somebody or use a /whatever command, hotlist will be cleared;
- Make the BufferList search selection (up, down when filtering) not go out of bounds
- Make some of the views show properly on tiny screens [#144](https://github.com/LithApp/Lith/issues/144)
- Implemented a custom effect for Channel scrolling overshoots to prevent it from jumping and glitching
### New features
- WASM build of the most recent unstable version accessible through https://lith.app/Lith
- Secure data storage (Keychain, Wallet, etc.) is now used for server  credentials. 
  - Not enabled on macOS by default because it shows multiple popups on startup
  - Not supported in WASM
- Empty password is now allowed [#95](https://github.com/LithApp/Lith/issues/95)
- Stop trying to reconnect after 3 failed password attempts.
- Added a button to reconnect to server from the error popup instead of going through settings.
- Added `--help` and `--version` commandline settings. [#157](https://github.com/LithApp/Lith/issues/157)
- App version is now displayed on the title screen (accessible by long-pressing the selected buffer in the buffer list).
- Added option to show the date when day changes in between messages. Messages can show the date too now.
- The visual style of all UI elements has been improved, with more contrast and some gradients. Colors are now more muted, too.
  - [#155](https://github.com/LithApp/Lith/issues/155) [#158](https://github.com/LithApp/Lith/issues/158) [#164](https://github.com/LithApp/Lith/issues/164), also closed some issues introduced while working on this.
- Reordered Input Bar settings entries to reflect the real UI order (#165)[https://github.com/LithApp/Lith/issues/165]
### Other and/or smaller changes
#### Small fixes
- Respect user-set time format when searching through messages
- Added entitlements and app category to the macOS app
- Include the asset catalog in the macOS build too to give it an icon
- Add a message to the Microphone requirement that's (probably?) carried from QtMultimedia
- Show an error messages in image preview popup on error [#150](https://github.com/LithApp/Lith/issues/150)
- Change the application organization domain to lith.app (This may reset Lith configuration for some users unfortunately)
#### Small features
- Allow setting negative line spacing because why not
- Make it possible to always show the scrollbar in buffer and message list
- Add a filter to show only buffers with new messages by default [#156](https://github.com/LithApp/Lith/issues/156)
#### UI Polish
- Balance line layout for messages with emojis
- Improved safe area handling (iPhone notch etc.)
- Views that are out of bounds of the screen are set to invisible to preserve resources.
- Use regular mouse cursor shape over messages (text cannot be selected anyway).
- Removed the custom animated text editing cursor in message input bar. It consumed a lot of resources and was not needed anymore.
- Labels in the buffer header are now aligned and their contents won't overflow the header boundaries. [#163](https://github.com/LithApp/Lith/issues/163)
  - This is the reason behind switching to font sizes in pixels.
#### Development-related
- Android is part of the CI again. Packages are built but cannot be installed at this moment unfortunately.
- Added qtquickcontrols2.conf to set LithStyle to the app
- The codebase is now split into QML modules, C++ types are exposed to QML declaratively.
- For development, there's an option to build an embedded [`qhot`](https://github.com/patrickelectric/qhot) executable to enable hot-reloading of QML code.
- Debug window was added with control over some UI properties like safe areas.
- Internal data view for all WeeChat entities was added.


## 1.4.0 (2023-07-22)
- Versioning scheme changed, from now on all X.Y.0 releases are public, X.Y.[1..] are development releases
- The minimum required Qt version is now 6.5
- Port the build system to CMake
- Performance issues caused by nonstop loading of more messages were resolved
- Missing messages in buffers with 4096 (or whatever your WeeChat buffer size limit is) and more lines are fixed
- Hotlist format can now be specified through an editor
- Buffer backlog can be searched
- The UI component library (buttons, etc) has been improved
- Various performance improvements
- Nick shortening input SpinBox now works properly with "Disabled" input (@syyyr)
- Input field can remember local unsent input when switching buffers
- iOS: The application now has a launch screen
- iOS: There is basic support for push notifications (not enabled yet)
- debug: There's now an internal debug log view
- debug: It's possible to record incoming WeeChat messages for debugging purposes

## 1.3.27 (2023-05-28)
- Changelog starts here
