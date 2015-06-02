//
//  FacebookInterface.mm
//
//  Created by Ruoqian, Chen on 2014/10/22
//
//  Links:
//     Login/Logout  : https://github.com/fbsamples/ios-howtos/tree/master/FBLoginCustomUISample
//     Post Photo    : https://github.com/facebook/facebook-ios-sdk/tree/master/samples/HelloFacebookSample
//     Send Requests : https://github.com/facebookarchive/ios-3.5-howtos/tree/master/SendRequestsHowTo

#include "FacebookMgr.h"
#include "FacebookInterface.h"

#import <FacebookSDK/FacebookSDK.h>

bool FacebookInterface::isLogin( void )
{
	auto state = FBSession.activeSession.state;
	CCLOG("Facebook State %d", state);
	return (FBSessionStateOpen == state);
}

void FacebookInterface::login(int cbIndex, const char* scope)
{
	if (FBSession.activeSession.state == FBSessionStateOpen) {
		return;
	}
	
	[FBSession openActiveSessionWithReadPermissions:@[@"public_profile"]
									   allowLoginUI:YES
								  completionHandler:
	 ^(FBSession *session, FBSessionState state, NSError *error) {
		 FacebookMgr::GetInstance()->NotifyObserver(cbIndex);
	 }];
}

void FacebookInterface::logout(void)
{
	if (FBSession.activeSession.state == FBSessionStateOpen
		|| FBSession.activeSession.state == FBSessionStateOpenTokenExtended) {
		
		[FBSession.activeSession closeAndClearTokenInformation];
		FacebookMgr::GetInstance()->NotifyObserver(FBACTION_LOGOUT);
	}
}

void FBCheckLogin(void(^action)(void))
{
	if (FBSession.activeSession.state == FBSessionStateOpen
		|| FBSession.activeSession.state == FBSessionStateOpenTokenExtended) {
		action();
		return;
	}
	
	[FBSession openActiveSessionWithReadPermissions:@[@"public_profile"]
									   allowLoginUI:YES
								  completionHandler:
	 ^(FBSession *session, FBSessionState state, NSError *error) {
		 action();
	 }];
}


// Convenience method to perform some action that requires the "publish_actions" permissions.
void FBPerformPublishAction (void(^action)(void))
{
    // we defer request for permission to post to the moment of post, then we check for the permission
    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
        // if we don't already have the permission, then we request it now
        [FBSession.activeSession requestNewPublishPermissions:@[@"publish_actions"]
                                              defaultAudience:FBSessionDefaultAudienceFriends
                                            completionHandler:^(FBSession *session, NSError *error) {
                                                if (!error) {
                                                    action();
                                                } else if (error.fberrorCategory != FBErrorCategoryUserCancelled) {
                                                    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Permission denied"
                                                                                                        message:@"Unable to get permission to post"
                                                                                                       delegate:nil
                                                                                              cancelButtonTitle:@"OK"
                                                                                              otherButtonTitles:nil];
                                                    [alertView show];
                                                }
                                            }];
    } else {
        action();
    }
	
}

void FacebookInterface::postStatus(const char* name,const char* caption,const char* description,const char* link,const char* picture)
{
	// Just use the icon image from the application itself.  A real app would have a more
	// useful way to get an image.
	NSString* picPath = [NSString stringWithUTF8String:picture];
	UIImage *img = [UIImage imageWithContentsOfFile: picPath];
	BOOL canPresent = [FBDialogs canPresentShareDialogWithPhotos];
	NSLog(@"canPresent: %d", canPresent);
    
	FBPhotoParams *params = [[FBPhotoParams alloc] init];
	params.photos = @[img];
	
	BOOL isSuccessful = NO;
	if (canPresent) {
		FBAppCall *appCall = [FBDialogs presentShareDialogWithPhotoParams:params
															  clientState:nil
																  handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
																	  if (error) {
																		  NSLog(@"Error: %@", error.description);
																	  } else {
																		  NSLog(@"Success!");
																	  }
																  }];
		isSuccessful = (appCall  != nil);
	}

	FBRequestConnectionErrorBehavior errorBehavior;
	errorBehavior = (FBRequestConnectionErrorBehavior)(FBRequestConnectionErrorBehaviorReconnectSession
	| FBRequestConnectionErrorBehaviorAlertUser
	| FBRequestConnectionErrorBehaviorRetry);
	
	if (!isSuccessful) {
		FBCheckLogin(^{
			FBPerformPublishAction(^{
				FBRequestConnection *connection = [[FBRequestConnection alloc] init];
				connection.errorBehavior = errorBehavior;

				[connection addRequest:[FBRequest requestForUploadPhoto:img]
					 completionHandler:^(FBRequestConnection *innerConnection, id result, NSError *error) {
				 }];
				[connection start];
			});
		});
	}
}

NSDictionary* parseURLParams (NSString *query)
{
    NSArray *pairs = [query componentsSeparatedByString:@"&"];
    NSMutableDictionary *params = [[NSMutableDictionary alloc] init];
    for (NSString *pair in pairs) {
        NSArray *kv = [pair componentsSeparatedByString:@"="];
        NSString *val =
        [kv[1] stringByReplacingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        params[kv[0]] = val;
    }
    return params;
}

void FBSendRequest()
{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization
                        dataWithJSONObject:@{
											 @"social_karma": @"5",
											 @"badge_of_awesomeness": @"1"}
                        options:0
                        error:&error];
    if (!jsonData) {
        NSLog(@"JSON error: %@", error);
        return;
    }
    
    NSString *giftStr = [[NSString alloc]
                         initWithData:jsonData
                         encoding:NSUTF8StringEncoding];
    
    NSMutableDictionary* params = [@{@"data" : giftStr} mutableCopy];
    
    // Display the requests dialog
    [FBWebDialogs
     presentRequestsDialogModallyWithSession:nil
     message:@"Learn how to make your iOS apps social."
     title:nil
     parameters:params
     handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
         if (error) {
             // Error launching the dialog or sending the request.
             NSLog(@"Error sending request.");
         } else {
             if (result == FBWebDialogResultDialogNotCompleted) {
                 // User clicked the "x" icon
                 NSLog(@"User canceled request.");
             } else {
                 // Handle the send request callback
                 NSDictionary *urlParams = parseURLParams([resultURL query]);
                 if (![urlParams valueForKey:@"request"]) {
                     // User clicked the Cancel button
                     NSLog(@"User canceled request.");
                 } else {
                     // User clicked the Send button
                     NSString *requestID = [urlParams valueForKey:@"request"];
                     NSLog(@"Request ID: %@", requestID);
                 }
             }
         }
     }];
}


void FacebookInterface::sendRequests(void)
{
	FBCheckLogin(^{
		FBSendRequest();
	});
}

void FacebookInterface::request(int nIndex, const char*pszRequest, const char *pszPermission)
{
	bool bNeedPermission = pszPermission ? true : false;
	if (bNeedPermission) {
		if ([FBSession.activeSession.permissions indexOfObject:[NSString stringWithUTF8String:pszPermission]] != NSNotFound) {
			pszPermission = false;
		}
	}
	
	if (bNeedPermission) {
		
		NSArray *permissions = [[NSArray alloc] initWithObjects:
								[NSString stringWithUTF8String:pszPermission], nil];
		
		[[FBSession activeSession] requestNewReadPermissions:permissions completionHandler:^(FBSession *session, NSError *error) {
			if (!error) {
				CCLOG("FB Request %s permission %s ok", pszRequest, pszPermission);
			}
			else {
				CCLOG("FB Request %s permission %s error", pszRequest, pszPermission);
			}
			
		}];
		
		return;
	}
	
	[[FBRequest requestForGraphPath:[NSString stringWithUTF8String:pszRequest]] startWithCompletionHandler: ^(
																											  FBRequestConnection *connection, NSDictionary *result, NSError *error) {
		if (error) {
			 CCLOG("FB Request %s error", pszRequest);
			 return;
		}
		
		NSData *jsonData = [NSJSONSerialization dataWithJSONObject:result options:NSJSONWritingPrettyPrinted error:&error];
		if (!jsonData) {
			CCLOG("FB Response trans json error");
		}
		
		NSString* strJson = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
		const char *pszJson = [strJson UTF8String];
		FacebookMgr::GetInstance()->OnGetResponse(nIndex, pszJson);
	}];
}

void FacebookInterface::inviteFriends(const char *pszAppLink, const char *pszImgUrl)
{
}
