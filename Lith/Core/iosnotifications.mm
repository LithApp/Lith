#include "lith.h"
#include "iosnotifications.h"

#import <UserNotifications/UserNotifications.h>
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//add a category to QIOSApplicationDelegate
@interface QIOSApplicationDelegate : NSObject <UIApplicationDelegate, UNUserNotificationCenterDelegate>
// No need to declare the methods here, since we’re overriding existing ones
@end

@implementation QIOSApplicationDelegate (APNSApplicationDelegate)
  - (BOOL)application:(UIApplication *)application
      didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {

    auto notificationCenter = [UNUserNotificationCenter currentNotificationCenter];
    if (notificationCenter.delegate == self) {
        return YES;
    }
    notificationCenter.delegate = self;

    return YES;
  }

  -(void)userNotificationCenter:(UNUserNotificationCenter *)center
          willPresentNotification:(UNNotification *)notification
              withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
  {
      Q_UNUSED(center)
      // Not handling just receiving notifications for now
      Lith::instance()->log(Logger::Platform, "Received a notification");
      completionHandler(UNNotificationPresentationOptionAlert);
  }

  -(void)userNotificationCenter:(UNUserNotificationCenter *)center
           didReceiveNotificationResponse:(UNNotificationResponse *)response
              withCompletionHandler:(void(^)())completionHandler
  {
      Q_UNUSED(center)
      Q_UNUSED(response)
      // Here notifications that were actually interacted with are handled
      NSDictionary *userInfo = response.notification.request.content.userInfo;
      if (userInfo[@"buffer_number"] != nullptr) {
          Lith::instance()->log(Logger::Platform, "Handling a notification interaction");
          emit Lith::instance()->notificationHandler()->bufferSelected([userInfo[@"buffer_number"] intValue]);
      }
      else {
        Lith::instance()->log(Logger::Unexpected, "Handling a notification interaction without a buffer number");
      }

      completionHandler();
  }

  - (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
      Lith::instance()->notificationHandler()->deviceTokenSet(QByteArray((char*)deviceToken.bytes, deviceToken.length).toHex());
  }

  - (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
      Lith::instance()->log(Logger::Unexpected, QString("Failed registering for notifications: %1").arg(QString::fromNSString(error.localizedDescription)));
  }
@end

void iosRegisterForNotifications() {
  Lith::instance()->log(Logger::Platform, QString("About to register for notifications"));

  dispatch_async(dispatch_get_main_queue(), ^{
        // Register to receive notifications from the system
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];
        [[UIApplication sharedApplication] registerForRemoteNotifications];
  });
}

void iosUnregisterForNotifications() {
  Lith::instance()->log(Logger::Platform, QString("About to unregister for notifications"));
  Lith::instance()->notificationHandler()->deviceTokenSet(QString());

  dispatch_async(dispatch_get_main_queue(), ^{
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeNone) categories:nil]];
        [[UIApplication sharedApplication] unregisterForRemoteNotifications];
  });
}
