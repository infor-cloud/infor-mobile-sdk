# InforAuthentication iOS mobile SDK
=====
## Summary

InforAuthentication is build for iOS native applications. This will be useful to authenticate your mobile application user using OAuth 2.0 to access Infor ION API services.

- Supports from iOS 8.0 and later versions.
- SSO support will be in iOS 9 and later versions

## Discussion

`InforAuhtentication.framework`


Key points:

Works in both Objective C and Swift projects. No need to add bridging header for swift projects
Developer don't need to check any conditions before saving User data (SSO purpose) .
Don't need to create initialization method and declare instace varilable delcaration for the classes. Can use shareInstance class method.
Can use the same sdk session instance across main app and  app extension targets (your app and extension should be configured with vaild "Add Groups").

# Installation

You can install framework using cocoapods or copying to your project.

`Cocoapods`

You can use cocoapods to integrate the InforAuthentication framework into your project like below:

Add Podfile to your project and add this conent:
```
source 'http://git.infor.com/scm/xos/cocoapodsspecs.git'
source 'https://github.com/CocoaPods/Specs.git'
platform :ios, '9.0'
target 'InforAuthenticationSample' do    # replace with your target
use_frameworks!


pod 'InforAuthentication'
end
```
If you already have Podfile in your project just add  pod 'InforAuthentication' in Podfile.



Now go to you project folder (open-->Terminal, type this : cd <project folder>). then,

```$  pod install ```

Now open xcode project folder and use .xcworkspace to open your project from now.



Copying Framework file to your project:


Just copy the framework (InforAuthentication.framework ) file to your project and add to xcode project target. As this is a universal framework, you need to add run script for your app target.

Steps:

Select application target in xcode and goto Build Phases.
Now, select + (on left to filter option), then select New Run Script Phase.
Select added Run Script and add below script:
```
# Signs a framework with the provided identity
code_sign() {
# Use the current code_sign_identitiy
echo "Code Signing $1 with Identity ${EXPANDED_CODE_SIGN_IDENTITY_NAME}"
echo "/usr/bin/codesign --force --sign ${EXPANDED_CODE_SIGN_IDENTITY} --preserve-metadata=identifier,entitlements $1"
/usr/bin/codesign --force --sign ${EXPANDED_CODE_SIGN_IDENTITY} --preserve-metadata=identifier,entitlements "$1"
}
# Set working directory to product’s embedded frameworks
cd "${BUILT_PRODUCTS_DIR}/${FRAMEWORKS_FOLDER_PATH}"
if [ "$ACTION" = "install" ]; then
echo "Copy .bcsymbolmap files to .xcarchive"
find . -name '*.bcsymbolmap' -type f -exec mv {} "${CONFIGURATION_BUILD_DIR}" \;
else
# Delete *.bcsymbolmap files from framework bundle unless archiving
find . -name '*.bcsymbolmap' -type f -exec rm -rf "{}" +\;
fi

echo "Stripping frameworks"

for file in $(find . -type f -perm +111); do
# Skip non-dynamic libraries
if ! [[ "$(file "$file")" == *"dynamically linked shared library"* ]]; then
continue
fi

# Get architectures for current file
archs="$(lipo -info "${file}" | rev | cut -d ':' -f1 | rev)"
stripped=""
for arch in $archs; do
if ! [[ "${VALID_ARCHS}" == *"$arch"* ]]; then
# Strip non-valid architectures in-place
lipo -remove "$arch" -output "$file" "$file" || exit 1
stripped="$stripped $arch"
fi
done

if [[ "$stripped" != "" ]]; then
echo "Stripped $file of architectures:$stripped"
if [ "${CODE_SIGNING_REQUIRED}" == "YES" ]; then
code_sign "${file}"
fi
fi
done
```

`Configuration`:

1. Set "URL Types" in Info section Xcode Project(Your project-> Target->Info->URL Types). Update Identifier and URL Schema fields with your first part of redirect Url.

2. Add `InforAuthentication.framework` in to your project directory. Then link InforAuthentication.framework to project (Your project-> Target-> General-> Embeded Libraries-> Add-> Select the framework file from the list). Also link Security.framework to project.(This is not required if used cocoapods)

3. Import framework in your code.

Objective C:
```
#import <InforAuthentication/InforAuthentication.h>
```

Swift:
```
import InforAuthentication
```

4. Configuring `AuthenticationManager` (must configure before you intiateAuthentication call).

Objective C:
```
//Setting your client app details
[[AuthenticationManager sharedInstance] configureAuthenticationManagerWithHostUrl:<host_url> scope:<prefered_scope> clientId:<app_client_id> clientSecret:<app_client_secret> redirectUrl:<app_redirect_url> tenant:<tenant>];
[[AuthenticationManager shareInstance] setAuthorizationServerEndPoints:<endpoints_dictionary>];
[AuthenticationManager shareInstance].delegate = self;
[AuthenticationManager sharedInstance].presentingController = <your_view_controller>;
```
Swift:
```
//Setting your client app details
AuthenticationManager.sharedInstance().configureAuthenticationManager(withHostUrl:<host url>, scope:<your_prefered_scope>, clientId:<app_client_id>, clientSecret:<app_client_secret>, redirectUri:<app_redirect_url>, tenant:<tenant>)
AuthenticationManager.sharedInstance().setAuthorizationServerEndPoints(<endpoints_dictionary>)
AuthenticationManager.sharedInstance().delegate = self
AuthenticationManager.sharedInstance().presentingController = <your view controller>
```
The scope can be empy or nil. example scopes are "openid profile" ,"profile".

endpoints_dictionary is dictionary values below format
```
{INFOR_OAUTH_AS_AUTH_ENDPOINT:"oa from QR code",INFOR_OAUTH_AS_TOKEN_ENDPOINT:"ot from QR code",INFOR_OAUTH_AS_REVOKE_ENDPOINT:"or from Qr code"}
```
Parameters mapping QR code data:

host_url - `pu`

app_client_id - `ci`

app_client_secret - `cs`

app_redirect_url - `ru`

There is a property to manage your SSO flow and private session. i.e. `privateModeAuthentication`

This should be used if:

You want start private session(no data to be saved to SSO Data manager or no cookies shares as it will load in internal webview).
Switch account option( if you have one user session with sso and wnat login with other user).
If you know there is session with server with different tenant, then should user private sssion ot you will find an error.

//If want continue with SSO flow, you dont need to use this property as its default is false. If you want to have private session use this property


Objective C:
```
[AuthenticationManager shareInstance].privateModeAuthentication = YES;
```

Swift:
```
AuthenticationManager.shareInstance().privateModeAuthentication = true
```

//Note that this property should be configured before calling initiateAuthentication method. if you set it after session created, library won't consider the property value and use the same value you configured(false if didnt configured) before you fire initiateAuthentication method.




5. Override the following method in your `AppDelegate` class

Objective C:
```
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation
{
if (url.absoluteString) {
NSString *stringPath = url.absoluteString;
NSArray *pathParts =  [stringPath componentsSeparatedByString:[NSString stringWithFormat:@"%@?",[REDIRECT_URL lowercaseString]]];
if (pathParts.count>1 && ([[pathParts objectAtIndex:1] hasPrefix:@"error"] || [[pathParts objectAtIndex:1] hasPrefix:@"code"])) {
[[InforOAuth getInforOAuthSharedInstance] handleOpenURL:url];
}
}
```
Swift:

```
func application(_ app: UIApplication, open url: URL, options: [UIApplicationOpenURLOptionsKey : Any] = [:]) -> Bool{
let stringPath = url.absoluteString
var pathParts = stringPath.components(separatedBy: REDIRECT_URL.lowercased() + "?")
if pathParts.count < 2 && (stringPath.contains("error=") || stringPath.contains("code=")) {
pathParts = stringPath.components(separatedBy: REDIRECT_URL.lowercased()+"/?")
}
if pathParts.count > 1 && (pathParts[1].hasPrefix("error") || pathParts[1].hasPrefix("code")) {
AuthenticationManager.sharedInstance().handleOpen(url);
}
return true
}
```
6. Opting for debug logs (you will be able to see logs in xcode debug area. Please make sure you disable this for production build.

Objective C:
```
[AuthenticationManager sharedInstance].debug = YES;
```
Swift:
```
AuthenticationManager.sharedInstance().debug = true
```
7. Start authentication using below.

Objective C:
```
[[AuthenticationManager sharedInstance] initiateAuthentication];
```
Swift:
```
AuthenticationManager.sharedInstance().initiateAuthentication()
```

8. Implement delegate methods  (protocal confirming methods).

Objective C:
```
//Required methods
-(void) didAuthenticationCompleted:(AuthenticationManager *)authenticationManager withProperties:(NSDictionary *)properties{ //on success
//Use access token details
}

-(void)didAuthenticationManagerFailedWithError:(NSError *)error{
//handle error scenarios
}
//Optional method
-(void) didAuthenticationCancelled{
//cancelled
}
```

Swift:
```
//Required methods to be implemented in your class
func didAuthenticationCompleted(_ authenticationManager: AuthenticationManager!, withProperties properties: [AnyHashable : Any]!) { // on succuss
//Use access token details
}

func didAuthenticationManagerFailedWithError(_ error: Error!) { // on errors
//handle error scenarios
}
//Optional method
func didAuthenticationCancelled() {
print("Cancelled login")
}
```

9. Use refresh token method sto fetch new access token from refresh token

Objective C:
```
[[AuthenticationManager shareInstance] requestForNewTokenFromRefreshToken];


//You should implement below delegate method to if you this method. on succuss, below method will be fired.
-(void) didRefreshTokenRequestCompleted:(AuthenticationManager *)authenticationManager withProperties:(NSDictionary *) properties{
//Use new acccess token details
}


//On fail below method will be called.
-(void)didAuthenticationFailedWithError:(NSError *)error{
if(error.code == OAUTH_ERROR_CODE_TOKEN_REVOKED || error.code == OAUTH_ERROR_CODE_INVALID_CLIENT || error.code == OAUTH_ERROR_CODE_TOKEN_REVOKE_FAILED){ // Token revoked, force logout local session  or client app removed
// Terminate your app session here
}
}

// Or with closure
[[AuthenticationManager sharedInstance] requestForNewTokenFromRefreshTokenWithSuccessBlock:^(AuthenticationManager *authenticationManager) {
//Use new access token details
} withFailBlock:^(NError *error) {

if(error.code == OAUTH_ERROR_CODE_TOKEN_REVOKED || error.code == OAUTH_ERROR_CODE_INVALID_CLIENT || error.code == OAUTH_ERROR_CODE_TOKEN_REVOKE_FAILED){// Token revoked, force logout local session  or client app removed
//local session  or client app removed
// Terminate your app session here
}

}];
```

Swift:
```
AuthenticationManager.sharedInstance().requestForNewTokenFromRefreshToken()


//You should implement below delegate method to if you this method. on succuss, below method will be fired.
func didRefreshTokenRequestCompleted(_ authenticationManager: AuthenticationManager!, withProperties properties: [AnyHashable : Any]!) {
//Use new acccess token details
}


//On fail below method will be called.
func didAuthenticationFailedWithError(_ error: Error!) { // on errors
if error?._code == -10004 || error?._code == -10006 || error?._code == -10007{// Token revoked, force logout local session  or client app removed
//local session  or client app removed
// Terminate your app session here
}
}


// Or with closure
AuthenticationManager.sharedInstance().requestForNewTokenFromRefreshToken(successBlock: { (authenticationManager:AuthenticationManager?) in
//Use new access token details

}) { (_ error:Error?) in

if error?._code == -10004 || error?._code == -10006 || error?._code == -10007{// Token revoked, force logout local session  or client app removed
//local session  or client app removed
// Terminate your app session here
}
else{
print("error \(String(describing: error))")
}
}

```

10. Logging out from session.

Objective C:
```
[[AuthenticationManager shareInstance] logoutFromCurrentSession];
//You should implement below delegate method to if you this method. on succuss, below method will be fired.

-(void) didSessionLoggedOut{
//token revoked, you can perform UI actions (nagivate back to login page) from here.
}


//On fail below method will be called.
func didAuthenticationManagerFailedWithError(_ error: Error!) { // on errors
if error?._code == -10004 || error?._code == -10006 || error?._code == -10007{// Token revoked, force logout local session  or client app removed
//local session  or client app removed
// Terminate your app session here
}
}
```

Swift:
```
AuthenticationManager.sharedInstance().logoutFromCurrentSession()


//You should implement below delegate method to if you this method. on succuss, below method will be fired.
func didSessionLoggedOut() {
//token revoked, you can perform UI actions (nagivate back to login page) from here.
}


//On fail below method will be called.
func didAuthenticationManagerFailedWithError(_ error: Error!) { // on errors
if error?._code == -10004 || error?._code == -10006 || error?._code == -10007{// Token revoked, force logout local session  or client app removed
//local session  or client app removed
// Terminate your app session here
}
}
```

# SSODataManager

`SSODataManager` class used managed the sso user data.

One user data will be save for one server (with ev reference). You wont be allowed to save one user details of second tenant if SSODataManager already have user details for same server with different tenant. Irrespective of tenants, one user details per one server.

1. Configure `SSODataManager` (Recommonded to configure this didFinishLaunchingWithOptions method).

```
// keychain_access_group format: <your team seed id>.com.infor.mobileapps
```

Objective C:
```
[[SSODataManager shareInstance] configureWithKeyChainAccessGroup:<your_keychain_access_group>];

// Example:
[[SSODataManager shareInstance] configureWithKeyChainAccessGroup:@""7U3DC43RL8.com.infor.mobileapps""];
```
```
Swift:
SSODataManager.sharedInstance().configure(withKeyChainAccessGroup: <your_keychain_access_group>)


//Example:
SSODataManager.sharedInstance().configure(withKeyChainAccessGroup: "7U3DC43RL8.com.infor.mobileapps")
```

2. Enable `Keychain Sharing` for your application target in project capabilities section.Enabling this will add entitlement file to your project. And you have to update keychain group with your app keychain access group (<your team seed id>.com.infor.mobileapps). Please note that the suffix .com.infor.mobileapps is fixed. just you need to update your team seed identifier.



2. Adding user details to `SSODataManager`. You need to call this method once you complete authentication and have user details ready to save. Make you sure save correct data as other apps will effect these changes. If you're unable to save, please print error object from error block, it will return the issue.

Objective C:
```
[[SSODataManager sharedInstance] addUserDataForServer:<server_ev> tenant:<tenant> userName:<user_name> displayPictureData:<user_image_data> withSuccessBlock:^(NSString *message) {
//success
} failBlock:^(NSError *error) {
//error
}];
```

Swift:

```
SSODataManager.sharedInstance().addUserData(forServer: <server_ev>, tenant:<tenant>, userName: <user_name>, displayPictureData: <user_image_data>, withSuccessBlock: { (_ message: String?) in
print("\(ApplicationConfiguration.instance().defaultServer.ev) - \(String(describing: message))")
}, fail: { (_ error: Error?) in
print("Error in Adding server to keychains store")
print("\(String(describing: error))")
})
```

// Here "<your_server_ev>" is value of environment variable(ev) from app's QR code.
//  <tenant> is tenant Id (ti) from qr code.
// <user_name> logged in user name
// <user_image_data> - (NSData)data from usrt picture. can be nil, and you need to update this once have user picture with you using update method below.


How do you get the user name & profile pic for the currently logged in user:

You have to make a mingle API calls to get user info and picture once you get token from AS.

User details mingle endpoint: `Mingle/SocialService.Svc/User/Detail`

User Picture: `Mingle/SocialService.Svc/User/{USER_GUID}/ProfilePhoto?thumbnailType=0`

You can get USER_GUID in user details response. Both requests needs header “Authorization” = “Bearer <your_token>.



3. Updating user details to sso data manager.

Objective C:
```
[SSODataManager sharedInstance] updateUserDataForServer:<server_ev> userName:<user_name> displayPictureData:<user_image_data> withSuccessBlock:^(NSString *message) {
//succes
} failBlock:^(NSError *error) {
//error
}];
```
Swift:
```
SSODataManager.sharedInstance().updateUserData(forServer: ApplicationConfiguration.instance().defaultServer.ev, userName: username, displayPictureData: imgData, withSuccessBlock: {(_ message: String?) in
print("\(String(describing: message))")
}, fail: {(_ error: Error?) in
print("\(String(describing: error))")
})
```

4. Fetching user details for the server

Objective C:
```
[SSODataManager sharedInstance] getUserDetailsForServer:"server_ev" withSuccessBlock:^(NSString *userName, NSData *userDisplayImageData) {
//use user details
} withFailBlock:^(NSError *error) {
//error
}];
```

Swift:
```
SSODataManager.sharedInstance().getUserDetails(forServer: "server_ev", withSuccessBlock: {(_ userName: String?, _ userDisplayImageData: Data?) in
//Your code to present user found screen
}, withFail: {(_ error: Error?) -> Void in
print("error \(String(describing: error))")
})
```

5. Checking SSO user data availability for the server

Objective C:
```
SSODataAvailability userAvailability = [[SSODataManager sharedInstance] checkUserDataAvailabilityForServer:<server_ev> tenant:<server_ev>];
if( userAvailability == SSODataAvailabilityFound){
// there is SSO user data for the requested server, show you current user found screen and use getUserData method to fetch user details.
}
else if( userAvailability == SSODataAvailabilityFoundWithDifferentTenant){
// User session is available but with different tenant so SafariViewController flow will give error. So use "privateModeAuthentication" to set private authentication which will    be loaded in WebView. Also not that in this flow you are not allowes to update user data to SSODataManager.
}
else {
// No SSO user data for requested server.
}
```

Swift:
```
let userAvailability: SSODataAvailability = SSODataManager.sharedInstance().checkUserDataAvailability(forServer: <server_ev>, tenant: <server_ev>)
if userAvailability == .found {
// there is SSO user data for the requested server, show you current user found screen and use getUserData method to fetch user details.
}
else if userAvailability == .foundWithDifferentTenant {
// User session is available but with different tenant so SafariViewController flow will give error. So use "privateModeAuthentication" to set private authentication which will    be loaded in WebView. Also not that in this flow you are not allowes to update user data to SSODataManager.
}
else {
// No SSO user data for requested server.
}
```

6. Removing user data from `SSODataManager`.

Make you you use this method if you met any of these conditions.

If `SSODataManager` returned `SSODataAvailabilityFound` and `AuthenticationManager` shows login page then if user login with different user than last saved session and on grant page if user declined access or if user unable to get in to main application (your app backend issue).


Objective C:
```
[SSODataManager sharedInstance]removeUserDataForServer:<server_ev> successBlock:^{
//succes
} failBlock:^(NSError *error) {
//fail
}];
```

Swift:
```
SSODataManager.sharedInstance().removeUserData(forServer: ApplicationConfiguration.instance().defaultServer.ev, successBlock: {()  in
print("Removed server and its associated user details from keychains store")
}, fail: {(_ error: Error?) in
print("Failed to remove server and its associated user details from keychains store.. \(String(describing: error))")
})
```
# Error handling

Error codes from `AuthenticationManager`:


```
enum OAuthErrorCode {
OAuthErrorCodeUserDeclinedAccess = -10001 //If user clicks disallow on grant approval page
OAuthErrorCodeNoCallbackFromAuthServer = -10002    //There is no callback implemented from server
OAuthErrorCodeInvalidAuthServerURL = -10003    //Provided invalid authorization url
OAuthErrorCodeTokenRevoked = -10004    //Refesh token revoked. Perform force local session logout as token revoked from other source.
OAuthErrorCodeTokenNotAuthorized = -10005    //Token is not authorized by server
OAuthErrorCodeInvalidClient = -10006    //Provided invalid client details to library
OAuthErrorCodeTokenRevokeFailed  = -10007   // Revoke(logout)token failed,perform force local session logout as client app removed from portal.
OAuthErrorCodeNoUserSession = -10008    //There is no active session and you are trying to request token or logging out.
OAuthErrorCodeNoRefreshTokenProvided = -10009 // No refresh token provided
}
```
If you set `persistentSessionStoring` false and not sending the refresh token to lib to perform new token fetch or revoke token.



Error codes from `SSODataManager`:

```
enum SSODataManagerErrorCode {
SSODataManagerErrorCodeInvalidData = -20001 //Provided empty input parameters to method or invalid data.

SSODataManagerErrorCodeNoChangeInData = -20002  //Provided information is already exists in keychain and no update required.

SSODataManagerErrorCodeUpdateFailed = -20003  //Failed to udpate server information to keychain since there is no such server in keychain to update, you may need to add server.

SSODataManagerErrorCodeInvalidInitialization = -20004 //Invalid intitilization, you must provide the keychain access group name.

SSODataManagerErrorCodeUnableToFetch = -20005 //Unable to fetch data from keychain.

SSODataManagerErrorCodeSameServerDifferentTenant = -20006 //There is same server with different tenant data in keychain and only one entry per one server is allowed.

SSODataManagerErrorCodeDeleteFailed = -20007  //Threre no such server in keychain to delete.
}
```


`AuthenticationManager` instace sharing between iOS App and app extensions(like Share Extension, File Provider Extension, etc):

If your ios applications also have app extensions and you need to access the ION API service urls from your app extensions, then you are recommended to use this configuration to enable AuthenticationManager session sharing between the app and app extensions.

Important notes: You should configure your app and extension app ID's with "Add Groups" Capability. and should use the same add group identfier.

Configure Add groups for your app ID. Go to apple member center(https://developer.apple.com/account/ios/identifier/bundle), select/create your app iD and enable "App Groups" application service for app ID. Before this you need to create add group identifer(from add groups section in member center) to configure while enabling it for app identifier.
In Xcode project and go to targets and select app target and enable "Add groups" in capability section( if you configured add group with your app identifier and then can see the add group identifier)


3. Finally, add key `InforAuthenticationAddGroupIdentifier` with value (add group identifier) in your main app and intended app extension's plists.
```
<key>InforAuthenticationAddGroupIdentifier</key>

<string>group.com.infor.yourapp</string>
```


Handling certificate trust challenges:

If you want to handle the authentication server service's certificate trust challenges by your own, you can use the below delegate methods.

Objective C:
```
-(void)URLSessionFromAuthenticationManager:(NSURLSession *)session withRequest:(NSURLRequest *)request didReceiveChallenge:(NSURLAuthenticationChallenge *)challenge completionHandler:(void (^)(NSURLSessionAuthChallengeDisposition, NSURLCredential *))completionHandler{
// handle URLAuthenticationChallenge and at last complete your prefered action (proceeding, cancelling request) using completionHandler
}
-(void) secureSessionFailedWithError:(NSError *) error{
// this will be called if you prefered to cancel the request in above challenage method.
}
```
Swift:

```
func urlSession(fromAuthenticationManager session: URLSession!, with request: URLRequest!, didReceive challenge: URLAuthenticationChallenge!, completionHandler: ((URLSession.AuthChallengeDisposition, URLCredential?) -> Void)!)

{
// handle URLAuthenticationChallenge and at last complete your prefered action (proceeding, cancelling request) using completionHandler}func secureSessionFailedWithError(_ error: Error!) {// this will be called if you prefered to cancel the request in above challenage method.
}
```

Handling multiple sessions:

You can use persistentSessionStoring to work with mutliple sessions in your app. You have to set it False. By default its true which means library will maintain one perisistent session. Note that if you set false, there won't be perisistent session with library, you have to save your session details(access_token,refresh_token, is authenticated or not) by your own. And you have use provide the refresh token for fetching new token details and revoking(logout) refresh token. Use below methods to call refresh token and revoke token by sending refresh_token. You must set delegate before you make fire these methods.

Objective C:
```
//Fetching new token details:
[[AuthenticationManager sharedInstance] requestForNewTokenFromRefreshToken:<your_refresh_token>];

//Revoking refresh token(logout)
[[AuthenticationManager sharedInstance] logoutFromSessionWithRefreshToken:<your_refresh_token>];
```

Swift:
```
//Fetching new token details:
AuthenticationManager.sharedInstance().requestForNewToken(fromRefreshToken: "your_refresh_token")


//Revoking refresh token(logout)
AuthenticationManager.sharedInstance().logoutFromSession(withRefreshToken: "your_refresh_token")
```

Sample apps:

InforAuthenticationSample

## Contact

If you have a question, problem or feedback, you can contact at nookaraju.maddila@infor.com
