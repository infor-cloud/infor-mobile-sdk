//
//  Definitions.h
//  InforAuthentication
//
//  Created by Nookaraju Maddila on 10/10/17.
//  Copyright © 2017 Infor. All rights reserved.
//

#ifndef Definitions_h
#define Definitions_h

/** OAuth error codes used by AuthenticationManager */
typedef NS_ENUM(NSInteger, OAuthErrorCode) {
    /** If user clicks disallow on grant approval page*/
    OAuthErrorCodeUserDeclinedAccess = -10001,
    /** There is no callback implemented from server */
    OAuthErrorCodeNoCallbackFromAuthServer = -10002,
    /** Provided invalid authorization url */
    OAuthErrorCodeInvalidAuthServerURL = -10003,
    /**Refesh token revoked. Perform force local session logout as token revoked from other source. */
    OAuthErrorCodeTokenRevoked = -10004,
    /** Token is not authorized by server */
    OAuthErrorCodeTokenNotAuthorized = -10005,
    /** Provided invalid client details to library */
    OAuthErrorCodeInvalidClient = -10006,
    /**Revoke(logout)token failed,perform force local session logout as client app removed from portal. */
    OAuthErrorCodeTokenRevokeFailed  = -10007,
    /**there is no active session and you are trying to request token or logging out*/
    OAuthErrorCodeNoUserSession = -10008,
    /**If you set persistentSessionStoring false and not sending the refresh token to lib to perform new token fetch or revoke token.*/
    OAuthErrorCodeNoRefreshTokenProvided = -10009
};

/**   Enum to describes the state of user session stored in keychains*/
typedef NS_ENUM(NSInteger, SSODataAvailability) {
    /** Failed to check availability*/
    SSODataAvailabilityUndefined = 0,
    /** User session is available for provided server*/
    SSODataAvailabilityFound = 1,
    /** There is no user session for provided server*/
    SSODataAvailabilityNotFound = -1,
    /** User session is available for provided server but with other tenant*/
    SSODataAvailabilityFoundWithDifferentTenant = 2
};

/** Error codes used by InforSSOKeychainManager */
typedef NS_ENUM(NSInteger, SSODataManagerErrorCode) {
    /** Provided empty input parameters to method or invalid data*/
    SSODataManagerErrorCodeInvalidData = -20001,
    /** Provided information is already exists in keychain and no update required*/
    SSODataManagerErrorCodeNoChangeInData = -20002,
    /** Failed to udpate server information to keychain since there is no such server in keychain to update, you may need to add server*/
    SSODataManagerErrorCodeUpdateFailed = -20003,
    /** Invalid intitilization, you must provide the keychain access group name*/
    SSODataManagerErrorCodeInvalidInitialization = -20004,
    /** Unable to fetch data from keychain*/
    SSODataManagerErrorCodeUnableToFetch= -20005,
    /** There is same server with different tenant data in keychain since only one entry per one server is allowed*/
    SSODataManagerErrorCodeSameServerDifferentTenant = -20006,
    /** Threre no such server in keychain to delete*/
    SSODataManagerErrorCodeDeleteFailed = -20007
};

#endif /* Definitions_h */
