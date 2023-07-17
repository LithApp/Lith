#include "lith.h"

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
        return true;
    }
    notificationCenter.delegate = self;

    // Register to receive notifications from the system
    [application registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeSound | UIUserNotificationTypeAlert | UIUserNotificationTypeBadge) categories:nil]];

    [application registerForRemoteNotifications];
    return YES;
  }

  -(void)userNotificationCenter:(UNUserNotificationCenter *)center
          willPresentNotification:(UNNotification *)notification
              withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
  {
      Q_UNUSED(center)
      // Not handling just receiving notifications for now
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
          emit Lith::instance()->notificationHandler()->bufferSelected([userInfo[@"buffer_number"] intValue]);
      }
      else {
        Lith::instance()->log(Logger::Unexpected, "Received a notification without a buffer number");
      }

      completionHandler();
  }

  - (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken {
      Lith::instance()->notificationHandler()->deviceTokenSet(QByteArray((char*)deviceToken.bytes, deviceToken.length).toHex());
  }

  - (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error {
      NSLog(@"Did Fail to Register for   Remote Notifications");
      NSLog(@"%@, %@", error, error.localizedDescription);
      Lith::instance()->log(Logger::Unexpected, QString("Failed registering for notifications: %1").arg(QString::fromNSString(error.localizedDescription)));
  }
@end
