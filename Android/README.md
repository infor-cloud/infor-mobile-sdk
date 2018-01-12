
## InforAuthentication
inforauthentication is build for Android native applications. This will be useful to authentice using OAuth 2.0 for Infor ION API access.InforAuhtentication.aar

## Dependency

###### 1.  Add the below lines in your manifest.xml under your launcher activity tag.

```
<intent-filter>
    <data android:scheme="your package name"/>
    <action android:name="android.intent.action.VIEW"/>
    <category android:name="android.intent.category.DEFAULT"/>
    <category android:name="android.intent.category.BROWSABLE"/>
</intent-filter>
```

###### 2. Copy "inforauthentication.aar" file to your libs directory and paste the following lines of code in your build.gradle(project  level).

```
allprojects {
    repositories {
        jcenter()
        flatDir {
            dirs 'libs'
        }
        google()
        maven {
            url 'https://jitpack.io'
        }
        maven { url 'http://nlbavwmobile1.infor.com:8081/repository/maven-snapshots/' }
        maven { url 'http://nlbavwmobile1.infor.com:8081/repository/maven-releases/' }
        maven { url "https://maven.google.com" }
    }
}
```

b) add the following lines of code in your build.gradle(module level).

```
compile 'com.infor.mobile:infor-authentication:0.1.7'
compile 'com.android.support:cardview-v7:25.2.0'
compile 'com.github.simbiose:Encryption:2.0.1'
```

```
repositories {
    maven {
        url 'http://nlbavwmobile1.infor.com:8081/repository/maven-releases/'
    }
    jcenter()
    google()
}
```


###### 3. Configuring AuthenticationManager (must configure before you intiateAuthentication call).

```

AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).configureAuthenticationManager(getApplicationContext(), qrCodeInformation); 

```
(or)
															
																
```
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).configureAuthenticationManager(getApplicationContext(), hostUrl, clientId, redirectUrl, tenantId, scope, clientSecret);
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).setAuthorizationEndPoints(getApplicationContext(), tokenEndPoint, authEndPoint, revokeEndPoint);
```

scope can be empy or nil. example scopes are "openid profile" ,"profile".
Parameters mapping QR code data:
hostUrl - pu 
clientId - ci
clientSecret - cs
redirectUrl - ru
tenantId - ti
There is a property to manage your SSO flow and private session. i.e.privateModeAuthentication
This should be used if:
You want start private session(no data to be saved to SSO Data manager or no cookies shares as it will load in internal webview). 
Switch account option( if you have one user session with sso and wnat login with other user).
If you know there is session with server with different tenant, then should user private sssion ot you will find an error.

//If want continue with SSO flow, you dont need to use this property as its default is false. If you want to have private session use this property
```
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).setPrivateModeAuthentication(getApplicationContext(), true);
```

//Note that this property should be configured before calling initiateAuthentication method. if you set it after session created, library won't consider the property value and use the same value you configured(false if didn't configured) before you fire initiateAuthentication method.

###### 4. Start authentication using below.
```
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).initiateAuthentication(this, this); 
```


###### 5. Implement interface and callback methods (implements AuthenticationListener):
```
@Override
public void onAuthenticationCompletedSuccessfully(Context context, String accessToken, String refreshToken, String expiresIn) {
    // Save accessToken & refreshToken and use for API Calls
}

@Override
public void onAuthenticationFailedWithError(Context context, String errorMessage) {
	// handle error Message
}

@Override
public void onAuthenticationCancelled(Context context) {
	// cancelled authenticataion
}

@Override
public void onAuthenticationCancelled(Context context, String errorMessage) {
	// cancelled authentication with error message.
}
```
###### 6. Use refresh token method sto fetch new access token from refresh token
```
//You need to pass the refreshToken that has been saved.
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).requestForNewTokenFromRefreshToken(getApplicationContext(), oldRefreshToken, this);
```

and the folllowing methods will be called appropriately.

```
@Override
public void onAuthenticationCompletedSuccessfully(Context context, String accessToken, String refreshToken, String expiresIn) {
 // Save accessToken & refreshToken and use for API Calls
}

@Override
public void onAuthenticationFailedWithError(Context context, String errorMessage) {
 // handle error Message
}

@Override
public void onAuthenticationCancelled(Context context) {
 // cancelled authenticataion
}

@Override
public void onAuthenticationCancelled(Context context, String errorMessage) {
 // cancelled authentication with error message.
}
```

AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).getAccessTokenFromCode(getApplicationContext(), codeYourecieveFromBrwowser, this);

//onCreate() or onNewIntent() Methods will be called once the app is redirected from the external browser if privateModeAuthentication is disabled. The relevant data comes to these methods in a scheme where you need to get the values and pass the code to InforAuthenticaion in order to get accessToken & refreshToken. 

###### 7. Logging out from session
//You need to pass the refreshToken that has been saved.
```
AuthenticationManager.getAuthenticationManagerInstance(getApplicationContext()).logoutFromCurrentSession(getApplicationContext(), refreshToken, this);
```

###### 8. Implement interface and callback methods (implements AuthenticationLogoutListener):
```
@Override
public void onLogoutSuccessfully(Context context) {
    // Logged out Successfully.
}

@Override
public void onLogoutFailedWithError(Context context, String errorMessage) {
	// Logged out Failed with errorMessage.
}
```

## SSODataManager ( This class used managed the sso user data) 

One user data will be save for one server (with ev reference). You wont be allowed to save one user details of second tenant if SSODataManager already have user details for same server with different tenant. Irrespective of tenants, one user details per one server.

###### 1. Configure SSODataManager and set the interface
```
SSODataManager.getSSODataManagerInstance(getApplicationContext());
SSODataManager.getSSODataManagerInstance(getApplicationContext()).setSSODataListener(this);
```

###### 2. Adding user details to SSODataManager. You need to call this method once you complete authentication and have user details ready to save. Make you sure save correct data as other apps will effect these changes. If you're unable to save, please check the errorMessage in the callback method.

```
SSODataManager.getSSODataManagerInstance(getApplicationContext()).addUserDataForServer(getApplicationContext(), environmentVariable, tenantId, userName, userBitmap, this);
```

and the folllowing methods will be called appropriately.

```
@Override
public void oSuccessfullyAddedUserDetails() {
// User Details are added 
}

@Override
public void onSuccessfullyUpdatedUserDetails() {
 // Updated User Details
}

@Override
public void onPermissionError(Context mContext, String permissionError) {
 // throws error if any permission need to be provided.
}

@Override
public void onError(String errorMessage) {
 // throws error message if any.
}

```

How do you get the user name & profile pic for the currently logged in user:
                You have to make a mingle API calls to get user info and picture once you get token from AS.
                User details mingle endpoint: Mingle/SocialService.Svc/User/Detail
                User Picture: Mingle/SocialService.Svc/User/{USER_GUID}/ProfilePhoto?thumbnailType=0
                You can get USER_GUID in user details response. Both requests needs header “Authorization” = “Bearer <your_token>.

###### 3. Updating user details

```
SSODataManager.getSSODataManagerInstance(getApplicationContext()).updateUserDataForServer(getApplicationContext(), environmentVariable, userName, mUserBitmap);
```

###### 4. Fetching user details for one server

```
SSODataManager.getSSODataManagerInstance(getApplicationContext()).getUserDetailsForServer(getApplicationContext(), environmentVariable); 
```

// returns jsonObject String;


Method will return a String that which exactly as like JSONObject. You can get the details as below, 

```
String mUserDetails = SSODataManager.getSSODataManagerInstance(getApplicationContext()).getUserDetailsForServer(getApplicationContext(), environmentVariable);
if (mUserDetails != null && mUserDetails.length() != 0) {
    try {
        JSONObject mUserObj = new JSONObject(mUserDetails);
        if (mUserObj != null) {
                 String userName = mUserObj.optString(UserData.USERNAME);
                 String mUserImage = mUserObj.optString(UserData.USERIMAGE);
                 Bitmap muserBitmap = mSdkInstance.getBitmapFromString(UserData);
        }
      } catch (JSONException e) {
           e.printStackTrace();
       }
}
```

// If you are not able to show the bitmap in your application. The SDK has a function to display the bitmap. The following method will help you to load the bitmap,

```
encodedString – Which you get from getUserDetailsForServer Method
public Bitmap getBitmapFromString(String encodedString) {

}
```

###### 5. Checking SSO user data availability for sever

```
SSODataManager.getSSODataManagerInstance(getApplicationContext()).checkUserDataAvailabilityForServe(getApplicationContext(), environmentVariable, tenantId);


if (SSODataManager.getSSODataManagerInstance(getApplicationContext()).checkUserDataAvailabilityForServe(getApplicationContext(), environmentVariable, tenantId)) == SSOData.SSODataAvailablity.SSODataAvailabilityFound) {
// there is SSO user data for the requested server, show you current user found screen and use getUserData method to fetch user details.
  
} else if (SSODataManager.getSSODataManagerInstance(getApplicationContext()).checkUserDataAvailabilityForServe(getApplicationContext(), environmentVariable, tenantId)) == SSOData.SSODataAvailablity.SSODataAvailabilityFoundWithDifferentTenant) {
// User session is available but with different tenant so SafariViewController flow will give error. So use "privateModeAuthentication" to set private authentication which will    be loaded in WebView. Also not that in this flow you are not allowes to update user data to SSODataManager.
} else {
// No SSO user data for requested server.
}
```

###### 6. Removing user data from SSODataManager.        

```
SSODataManager.getSSODataManagerInstance(getApplicationContext()).removeUserData(getApplicationContext(), environmentVariable, this);
```
