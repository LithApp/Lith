#import "UIKit/UIKit.h"

// Include our C++ class
//#include “NotificationHandler.h"

#include <QtDebug>

// This is hidden, so we declare it here to hook into it
@interface QIOSApplicationDelegate
@end

//add a category to QIOSApplicationDelegate
@interface QIOSApplicationDelegate (APNSApplicationDelegate)
// No need to declare the methods here, since we’re overriding existing ones
@end

@implementation QIOSApplicationDelegate (APNSApplicationDelegate)

- (BOOL)application:(UIApplication *)application
    didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Register to receive notifications from the system
    [application registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];

    [application registerForRemoteNotifications];
    NSLog(@"registered for remote notifications");
    return YES;
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
    NSLog(@"Did Register for Remote Notifications with Device Token (%@)", deviceToken);
    //NotificationHandler::GetInstance()->RegisterToken(deviceToken.bytes, deviceToken.length);
    QString hex = QByteArray((char*)deviceToken.bytes, deviceToken.length).toHex();
    /*
    QString split;
    for (int i = 0; i < hex.count(); i += 8) {
        split.append(hex.mid(i, 8));
        if (i < hex.count() - 8)
            split.append(" ");
    }
    */

    qCritical() << "DID REGISTER FOR NOTIFICATIONS" << hex;
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
    NSLog(@"Did Fail to Register for Remote Notifications");
    NSLog(@"%@, %@", error, error.localizedDescription);
    qCritical() << "BULLSHIT";
}

@end
