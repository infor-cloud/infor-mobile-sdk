//
//  AuthenticationManager.h
//  InforAuthentication
//
//  Created by Nookaraju Maddila on 10/10/17.
//  Copyright © 2017 Infor. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@class AuthenticationManager;

/**
 *  This is delegate for AuthenticationManager Class.
 */
@protocol AuthenticationManagerDelegate <NSObject>

/**
 *  Following are required delegate methods
 */
@required

/**
 *  It returns error message if rasies in between the authorization process.
 *
 *  @param error is an error that prevented the authorization.
 */
-(void) didAuthenticationManagerFailedWithError:(NSError *) error;

/**
 *  This delegate method fired on successful authentication process.
 *
 *  @param authenticationManager AuthenticationManager's instance.
 *  @param properties is NSDictionary that will have access_token,refresh_token, expires_in and id_token.
 */
-(void) didAuthenticationCompleted:(AuthenticationManager *)authenticationManager withProperties:(NSDictionary *) properties;

@optional

/**
 *  This delegate method fired on successful refresh token request.
 *
 *  @param authenticationManager AuthenticationManager's instance.
 *  @param properties NSDictionary that will have new access_token,refresh_token, expires_in and id_token.
 */
-(void) didRefreshTokenRequestCompleted:(AuthenticationManager *)authenticationManager withProperties:(NSDictionary *) properties;

/**
 *  This delegate method fired on successful logout request.
 *
 */
-(void) didSessionLoggedOut;

/**
 *  This delegate method fired if user aborts action
 *
 */
-(void) didAuthenticationCancelled;

/**
 *  This delegate method fired when user cancels the insercure connection prompts.
 * @param error returns any error on SSL challenges
 *
 */
- (void)secureSessionFailedWithError:(NSError *) error;

/**
 *  This delegate method fired on insercure connection prompts for NSSession Tasks
 * @param session returns session object
 * @param request returns current request details
 * @param challenge returns NSURLAuthenticationChallenge object
 * @param completionHandler must handle completion handler
 *
 */
- (void)URLSessionFromAuthenticationManager:(NSURLSession *)session withRequest:(NSURLRequest*) request didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition, NSURLCredential *))completionHandler;
@end


/**
 *  AuthenticationManager Class
 *  Library uses the OAuth 2.0
 */
@interface AuthenticationManager : NSObject


// Block Declarations

/**
 *  FetchRefreshTokenBlockFail block declararion, invokes on refresh token request success.
 * @authenticationManager returns AuthenticationManager object, you can get access token details from this object
 *
 */
typedef void(^FetchRefreshTokenBlockSuccess)(AuthenticationManager *authenticationManager);

/**
 *  FetchRefreshTokenBlockFail block declararion, invokes on refresh token request failure.
 * @error returns error
 *
 */
typedef void(^FetchRefreshTokenBlockFail)(NSError *error);


//Constants (end point keys)
FOUNDATION_EXPORT NSString * const INFOR_OAUTH_AS_AUTH_ENDPOINT;
FOUNDATION_EXPORT NSString * const INFOR_OAUTH_AS_TOKEN_ENDPOINT;
FOUNDATION_EXPORT NSString * const INFOR_OAUTH_AS_REVOKE_ENDPOINT;

/**
 * delegate object for AuthenticationManager class
 */
@property (nonatomic,weak) id <AuthenticationManagerDelegate> delegate;

/**
 *  Expects a string, should be a valid authorization host url.
 */
@property (nonatomic,strong) NSString *issuer;

/**
 * Expects a string, should be a valid scope.
 * scope is optional (can be nil)
 */
@property (nonatomic,strong) NSString *scope;

/**
 * Expects a string, should be a valid client Id.
 */
@property (nonatomic,strong) NSString *clientId;

/**
 * Expects a string, should be a valid client secret.
 */
@property (nonatomic,strong) NSString *client_secret;

/**
 *  Expects a string, should be a valid redirecting url.
 */
@property (nonatomic,strong) NSString *redirecURL;

/**
 * Expects a string, should be a valid tenant.
 */
@property (nonatomic,strong) NSString *tenant;

/**
 * Expects a bool value. Lib will log in Xcode debug area if set to true.
 * 
 * Default value is false.
 */
@property (nonatomic,assign) BOOL debug;

/**
 * If set to true, periodically send a request to AuthorizationServer(AS) for new access token from last refresh token.
 * Default value is false.
 */
@property (nonatomic,assign) BOOL refreshTokenAutomatically;

/**
 * Expects a bool value,if set to YES then application loads authorization page in safari (Out of App) and application control redirected back to app on successful authorization.
 * Default value is NO.
 */
@property (nonatomic,assign) BOOL useDeviceSafariToAuthenticate;

/**
 * If you want use other viewcontroller rather than delegate to present OAuth View.
 */
@property (nonatomic,strong) UIViewController *presentingController;

/**
 *
 * if you set privateModeAuthentication to true, authentication will performed in Webview and there won't be SSO support for that session and you are not allowed to save/update session user data to SSODataManager.
 * By default its false.
 * This should configured before you call intitateAuthentication method.
 * Setting this after authentication won't be considered.
 */
@property (nonatomic,assign) BOOL privateModeAuthentication;

/**
 * If your application have extenstion(s) and need the same AuthenticationManager instance their targets to requesting new tokens, then you can do this using add groups. Enable add groups in app targets (app and exensions) capabilities and enable the add groups for app id (app and exensions)  and add "InforAuthenticationAddGroupIdentifier" key with your add group identifer as its value in your app's (for app & app extension) plist. This will help with getting new tokens in app extenstion if you have authenticated in main app and you cannot perform initiateAuthentication in app extension*/
@property (nonatomic,readonly,strong) NSString *addGroupIdentifier;


/**
 If you set this to false, you don't want lib to save any token details and killing app instance wont leave any lib session data. And you are responsible for handling active session is there or not based on your app level refresh_token data. You have to make new refresh token and revoke token call by providing app stored refresh_token.
 * Note that isAuthenticated wont return correct session state as ther won't be any data save in lib.
 * You have to use below method to fetch new token from refresh token. here refresh_token not an option value.
  -(void)requestForNewTokenFromRefreshToken:(NSString *)refresh_token
 *You have to use below method to revoke/logout. Here refresh_token is optional if you have already peformed refresh token call. Otherwise you should provide the refresh token.
 -(void) logoutFromSessionWithRefreshToken:(NSString *)refresh_token
 *
 *by default, true.
 */
@property (nonatomic,assign) BOOL persistentSessionStoring;

/**---------------------------------------------------------------------------------------
 * @name Output Parametrs
 *  ---------------------------------------------------------------------------------------
 */
/**
 * It returns access token string.
 */
@property (nonatomic,readonly,strong) NSString *access_token;

/**
 * It returns refresh token string, Its useful for requesting new access_token.
 */
@property (nonatomic,readonly,strong) NSString *refresh_token;

/**
 * It returns id token byte string.
 */
@property (nonatomic,readonly,strong) NSString *id_token;

/**
 * It returns token life validity in number of seconds.
 */
@property (nonatomic,readonly,assign) NSTimeInterval expires_in;

#pragma mark - Class method
/**---------------------------------------------------------------------------------------
 * @name Global instance method
 *  ---------------------------------------------------------------------------------------
 */
/**
 * It creates a global instance for AuthenticationManager class that you can use throughout the application life cycle.
 *
 *  @return return a global AuthenticationManager instance
 */
+(AuthenticationManager *) sharedInstance;

#pragma mark - Instance methods

/**---------------------------------------------------------------------------------------
 * @name configuring AuthenticationManager with client app details
 *  ---------------------------------------------------------------------------------------
 */
/**
 *  This method configures the client app details with AuthenticationManager.
 *
 *  @param strASHostUrl    a string, should be a valid authorization server host url.
 *  @param strScope     a string, should be a valid scope.
 *  @param strClientId  a string, should be a valid client Id.
 *  @param clientSecret  a string,  can be nil for MCC registed application (infor apps), but should be valid client secret for tenant specific apps.
 *  @param urlString    a string, should be a valid redirecting url.
 *  @param tenant       a string, should be a valid tenant.
 */
-(void) configureAuthenticationManagerWithHostUrl:(NSString *) strASHostUrl scope:(NSString *) strScope clientId:(NSString *) strClientId  clientSecret:(NSString *)clientSecret redirectUrl:(NSString *) urlString tenant:(NSString *) tenant;


/**---------------------------------------------------------------------------------------
 * @name Configure AS Endpoints.
 *  ---------------------------------------------------------------------------------------
 */
/**
 * This can be used to configure custom AS endpoints.
 * Dictionary should contains respective values for keys:"INFOR_OAUTH_AS_AUTH_ENDPOINT"(oa in QR data),"INFOR_OAUTH_AS_TOKEN_ENDPOINT" (ot in QR data) & "INFOR_OAUTH_AS_REVOKE_ENDPOINT"(or in QR data).
 * If dont configure end points it will take fixed endpoints by the library.
 *  @param endpoints dictionary of endpoints
 */
-(void)setAuthorizationServerEndPoints:(NSDictionary *)endpoints;


/**---------------------------------------------------------------------------------------
 * @name Starting authorization
 *  ---------------------------------------------------------------------------------------
 */
/**
 *  It starts the authorization process. Make sure that you must set "Delegate" to AuthenticationManager instance before calling this method.
 * @discussion - didAuthenticationFailedWithError:  will be called on request fail.
 *  - didAuthenticationCompleted: withProperties:  will be called on authentication completion.
 */
-(void) initiateAuthentication;


/**---------------------------------------------------------------------------------------
 * @name Support method
 *  ---------------------------------------------------------------------------------------
 */
/**
 *  This should be called from handleOpenURL: method in AppDelegate.
 *
 *  @param url this paramater contains the callback url and authorization code.
 */
-(void) handleOpenURL:(NSURL *)url;


/**---------------------------------------------------------------------------------------
 * @name Requesting new toke from refresh token
 *  ---------------------------------------------------------------------------------------
 */
/**
 *  It requests for new access token details from last refresh token. You should be authorized and must have valid refresh token to call this method.
 *
 *  - didRefreshTokenRequestCompleted: withProperties:  will be called on request success.
 *  - didAuthenticationFailedWithError:  will be called on request fail.
 */
-(void)requestForNewTokenFromRefreshToken;


/**---------------------------------------------------------------------------------------
 * @name Clearing the current session. Uses most recent refresh token from current session to revoke
 *  ---------------------------------------------------------------------------------------
 */
/**
 * This method used to revoke the last grant access
 *
 */
-(void) logoutFromCurrentSession;



/**---------------------------------------------------------------------------------------
 * @name check the login state
 *  ---------------------------------------------------------------------------------------
 */
/**
 * This method returns true if session is valid.
 * @return returns bool value
 *
 */
-(BOOL) isAuthenticated;

/************ If you set persistentSessionStoring to false, use below methods**************/

/**---------------------------------------------------------------------------------------
 * @name Requesting new access token with blocks (This will method expect is explict properties and refresh token to fetch new token.) mostly used in mutli session application requirement with swtching without logging out any session.
 *  ---------------------------------------------------------------------------------------
 */
/**
 * It requests for new access token details from explict session details. You should be authorized and must have valid refresh token to call this method. Please note that this is not perisist session, you must save token details in FetchRefreshTokenBlockSuccess.
 *  @param refresh_token app level saved refresh token
 */
-(void) requestForNewTokenFromRefreshToken:(NSString *)refresh_token;


/**---------------------------------------------------------------------------------------
 * @name This method revokes the explict session provided
 *  ---------------------------------------------------------------------------------------
 */
/**
 * Use this block method to revoke/logout explict session(not a current session) This method will not return any of delegate methods, should implement blocks.
 *  @param refresh_token app level saved refresh token
 */
-(void) logoutFromSessionWithRefreshToken:(NSString *)refresh_token;


/*************************************** Block method **************************************/

/**---------------------------------------------------------------------------------------
 * @name Requesting new access token with blocks (will use current session refresh token)
 *  ---------------------------------------------------------------------------------------
 */
/**
 * It requests for new access token details from last refresh token. You should be authorized and must have valid refresh token to call this method. This method will not call any of delegate methods, should use blocks.
 *  @param successBlock returns FetchRefreshTokenBlockSuccess on request success.
 *  @param failBlock returns FetchRefreshTokenBlockFail on request failure.
 */
-(void)requestForNewTokenFromRefreshTokenWithSuccessBlock:(FetchRefreshTokenBlockSuccess)successBlock withFailBlock:(FetchRefreshTokenBlockFail) failBlock;

@end
