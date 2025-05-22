This is annoying but I'm still not sure what exact steps need to be taken to refresh iOS signing certificates. 

2025 UPDATE:

Provisioning files are now listed in exportOptions.plist along with the name of the certificate. I added more XCODE target attributes to Lith in CMake. All in all, this SHOULD be enough to only use a single provisioning file and a single certificate (Apple Distribution, the other ones are for Mac or private use only). 

#--------------------------------------
These are notes from before 2025, probably too complicated 

A few notes:

What I think worked:
- Generating the "Apple Development" certificate
  - This is likely used only for myself and can be removed
- Generating the "Apple Distribution" certificate
  - Not sure what is this used for, experiment with removal
- Generating the "iPhone Distribution" certificate
  - This is required, almost certainly
  - In some places, it seems to be called "iOS Distribution"
- In the end, I did this:
  - Added `-allowProvisioningUpdates` to all `xcodebuild` lines in `build.sh`
  - Ran `build.sh` with this modification
  - Ran `xcrun altool --validate-app --file build_ios/Lith.ipa --type ios --username "..." --password "..."`. 
  - This created two provisioning profiles. I uploaded those and build passed.
 
What didn't work:
- Going to [developer.apple.com](developer.apple.com), creating Provisioning Profiles and putting them in the folder as `Lith_Development.mobileprovision` and `Lith_Testflight.mobileprovision`.
  - I don't really know why, this should be fine too, I may have missed the signing key in Secrets at this point
- Creating and uploading the archive through XCode GUI
  - Same as above, not sure why it didn't work
  
Modified secrets
- `CERT_APPLE_DEVELOPMENT_P12`
- `CERT_APPLE_DISTRIBUTION_P12`
- `CERT_IOS_DISTRIBUTION_P12`
- `DEVELOPMENT_PROVISIONING_PROFILE`
- `IOS_PROVISIONING_PROFILE`
- `MANUAL_PROVISIONING_PROFILE`
- `MANUAL_PROVISIONING_PROFILE_2`

Other notes
- Only one provisioning profile should be necessary
- I remember reading claims the profile cannot be renamed (meaning: the same UUID name needs to be used even when copied to CI).
- A very similar process will be used for macOS
- Signing keys should be exported as p12 after being imported locally
