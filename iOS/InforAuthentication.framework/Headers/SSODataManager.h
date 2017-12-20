//
//  SSODataManager.h
//  InforAuthentication
//  
//  Created by Nookaraju Maddila on 10/10/17.
//  Copyright © 2017 Infor. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Definitions.h"

/**
 *  SSODataManager class
 */
@interface SSODataManager : NSObject
/**
 *  UserDataFetchSuccessBlock block declaration, Invokes on successfully fetching user details from the shared keychain for a perticular server
 *  @userName userName User display name of logged in user
 *  @userDisplayImageData userDisplayImageData User display image data of logged in user
 */
typedef void (^UserDataFetchSuccessBlock)(NSString *userName, NSData *userDisplayImageData);

/**
 *  UserDataSaveSuccessBlock block declaration, Invokes on success updation of shared keychain with data
 * @message success message
 */
typedef void (^UserDataSaveSuccessBlock)(NSString *message);

/**
 *  UserDataRemoveSuccessBlock block declaration, Invokes on successful deletion of user data from manager
 * @message success message
 */
typedef void (^UserDataRemoveSuccessBlock)(void);

/**
 *  UserDataFailBlock declaration, Invokes on getting failed in fatching or inserting some data
 *  @param error Gives the detailed error object with code and message
 */
typedef void (^UserDataFailBlock)(NSError *error);

/*
 * Expects a string value. string object and should provide valid access gtoup indentifier same as in your entitlements.plist file.
 */
@property (nonatomic,readonly,strong) NSString *keyChainAccessGroup;

/**
 * This method initializes the Infor SSODataManager and assigns the required access group value
 * @param keyChainAccessGroup -string object and should provide valid access gtoup indentifier same as in your entitlements.plist file.
 **/
- (id)initWithKeyChainAccessGroup : (NSString *)keyChainAccessGroup;

/**---------------------------------------------------------------------------------------
 * @name Shared instance method
 *  ---------------------------------------------------------------------------------------
 */
/**
 * It creates a global instance for SSODataManager class that you can use through out the application.
 *
 *  @return returns a global SSODataManager
 *  Make sure to use this after intialiting the of object SSODataManager with access group
 */
+ (SSODataManager *)sharedInstance;

/**
 * This method assigns the required access group value
 * @param keyChainAccessGroup -string object and should provide valid access gtoup indentifier same as in your entitlements.plist file.
 */
-(void) configureWithKeyChainAccessGroup:(NSString*) keyChainAccessGroup;

/**
 * This method returns bool with configuration state.
 */
-(BOOL) isConfiguredWithAccessgroup;

/**
 * addUserDataForServer -this method is to add environment id, tenant, username and user display image data to shared keychain store
 *  @param environmentVariable -its a unique variable given to the enviroment which you can get from scanning QR Code
 *  @param tenant -its a Tenant which you can get from scanning QR Code
 *  @param username -its the display name of logged in user
 *  @param userDisplayPictureData -image file data of logged in user display picture
 *  @param successBlock -Success block for adding user details
 *  @param failBlock -Fail block for adding user details
 */
- (void)addUserDataForServer:(NSString *)environmentVariable tenant:(NSString *)tenant userName:(NSString *)username displayPictureData:(NSData *)userDisplayPictureData withSuccessBlock:(UserDataSaveSuccessBlock)successBlock failBlock:(UserDataFailBlock)failBlock;

/**
 * updateUserDataForServer -this method is to update shared keychain store with username and user display image data for a particular server
 *  @param environmentVariable -its a unique variable given to the enviroment which you can get from scanning QR Code
 *  @param username -its the display name of logged in user
 *  @param userDisplayPictureData -image file data of logged in user display picture
 *  @param successBlock -object of KeychainUpdateSuccessBlock
 *  @param failBlock -object of KeychainUpdateFailBlock
 */
- (void)updateUserDataForServer:(NSString *)environmentVariable userName:(NSString *)username displayPictureData:(NSData *)userDisplayPictureData withSuccessBlock:(UserDataSaveSuccessBlock)successBlock failBlock:(UserDataFailBlock)failBlock;

/**
 * getUserDetailsForServer -this method is to fetch the details of a user from keychain store who is available at particular server
 *  @param environmentVariable -its a unique variable given to the enviroment which you can get from scanning QR Code
 *  @param successBlock -Success block for updating user details
 *  @param failBlock -Fail block for updating user details
 */
-(void)getUserDetailsForServer:(NSString *)environmentVariable withSuccessBlock:(UserDataFetchSuccessBlock)successBlock withFailBlock:(UserDataFailBlock)failBlock;

/**
 * checkUserDataAvailabilityForServer -this method is to find the session availabilty of a user in shared keychain store.
 *  @param environmentVariable -its a unique variable given to the enviroment which you can get from scanning QR Code
 *  @param tenant -its a Tenant which you can get from scanning QR Code
 *  @return UserSessionAvailabilty -its the enum which gives different states of session availabilty
 */
-(SSODataAvailability)checkUserDataAvailabilityForServer:(NSString *)environmentVariable tenant:(NSString *)tenant;


/**
 *  removeUserDataForServer -this method is to delete a particular server and its associated user details from keychains store.
 *  @param environmentVariable -its a unique variable given to the enviroment which you can get from scanning QR Code
 *  @param successBlock -object of KeychainUpdateSuccessBlock
 *  @param failBlock -object of KeychainUpdateFailBlock
 */
-(void)removeUserDataForServer:(NSString *)environmentVariable  successBlock:(UserDataRemoveSuccessBlock)successBlock failBlock:(UserDataFailBlock)failBlock;



@end
