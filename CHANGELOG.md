### 1.4.0 (2023-07-22)
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

### 1.3.27 (2023-05-28)
- Changelog starts here
