package org.Plugin.Facebook;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookRequestError;
import com.facebook.FacebookSdk;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.ShareApi;
import com.facebook.share.model.AppInviteContent;
import com.facebook.share.model.GameRequestContent;
import com.facebook.share.model.SharePhoto;
import com.facebook.share.model.SharePhotoContent;
import com.facebook.share.widget.AppInviteDialog;
import com.facebook.share.widget.GameRequestDialog;
import com.facebook.share.widget.MessageDialog;
import com.facebook.share.widget.ShareDialog;

public class FacebookPlugin {
	private static final String TAG = "FacebookPlugin";
	private static final String PERMISSION = "publish_actions";
	private static FacebookPlugin sInstance;
	private static int sCallIndex;
	private CallbackManager mCallbackManager;
	private Activity mActivity;
	private String mStrPicPath;
	private ShareDialog mShareDialog;
	private PendingAction mPendingAction = PendingAction.NONE;
	private GameRequestDialog mRequestDialog;

	public FacebookPlugin(Activity activity) {
		this.mActivity = activity;
		sInstance = this;
		
		Context context = activity.getApplicationContext();
		
	    FacebookSdk.sdkInitialize(context);
	    mCallbackManager = CallbackManager.Factory.create();
	    LoginManager.getInstance().registerCallback(mCallbackManager, new FacebookCallback<LoginResult>() {

			@Override
			public void onSuccess(LoginResult result) {
				runNativeCallback(sCallIndex, "");
				handlePendingAction();
			}

			@Override
			public void onCancel() {
				runNativeCallback(sCallIndex, "");
			}

			@Override
			public void onError(FacebookException error) {
				runNativeCallback(sCallIndex, "");
			}	    	
	    });
	    
	    mShareDialog = new ShareDialog(activity);

        mRequestDialog = new GameRequestDialog(activity);
        mRequestDialog.registerCallback(mCallbackManager, new FacebookCallback<GameRequestDialog.Result>() {
            public void onSuccess(GameRequestDialog.Result result) {
                String strResult = result.toString();
                Log.i(TAG, "Game Request Result: " + strResult);
            }

            public void onCancel() {}

            public void onError(FacebookException error) {}
        });
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		mCallbackManager.onActivityResult(requestCode, resultCode, data);
		runNativeCallback(0, "");
	}

	public static void login(int cbIndex, String scope) {
		sCallIndex = cbIndex;

		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				sInstance.FBLogin();	
			}
		});
	}

	public void FBLogin() {
		LoginManager.getInstance().logInWithReadPermissions(mActivity, Arrays.asList("public_profile", "user_friends"));
	}
	
	public static void logout(int cbIndex) {
		sCallIndex = cbIndex;
		
		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				sInstance.FBLogout();
				runNativeCallback(sCallIndex, "");
			}
		});
	}

	public void FBLogout() {
		Log.i(TAG, "Logout");
		LoginManager.getInstance().logOut();
	}	
	
    private enum PendingAction {
        NONE,
        POST_PHOTO,
        POST_STATUS_UPDATE
    }	
	
	public static void postStatus(int cbIndex, String aName, String aCaption, String aDescription, String aLink, final String strPicturePath) {
		sCallIndex = cbIndex;

		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
		        boolean canPresentShareDialogWithPhotos = ShareDialog.canShow(
		                SharePhotoContent.class);
		        sInstance.mStrPicPath = strPicturePath;
		        sInstance.performPublish(PendingAction.POST_PHOTO, canPresentShareDialogWithPhotos);
			}
		});
	}

    private boolean hasPermission(String strPermission) {
        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        return accessToken != null && accessToken.getPermissions().contains(strPermission);
    }

    private void performPublish(PendingAction action, boolean allowNoToken) {
        mPendingAction = action;
        if (allowNoToken) {
            handlePendingAction();
            return;
        }

        AccessToken accessToken = AccessToken.getCurrentAccessToken();
        if (accessToken != null && hasPermission(PERMISSION)) {
        	handlePendingAction();
        } else {
        	sCallIndex = 0;
            LoginManager.getInstance().logInWithPublishPermissions(mActivity, Arrays.asList(PERMISSION));
        }
    }	
	
    private void handlePendingAction() {
        PendingAction previouslyPendingAction = mPendingAction;
        // These actions may re-set pendingAction if they are still pending, but we assume they
        // will succeed.
        mPendingAction = PendingAction.NONE;

        switch (previouslyPendingAction) {
            case NONE:
                break;
            case POST_PHOTO:
            	FBPostPhoto();
                break;
        }
    }

    private void FBPostPhoto() {
    	Log.i(TAG, "FBPostPhoto");
    	Bitmap image = BitmapFactory.decodeFile(mStrPicPath);
    	SharePhoto photo = new SharePhoto.Builder().setBitmap(image).build();
		SharePhotoContent sharePhotoContent = new SharePhotoContent.Builder()
		        .addPhoto(photo)
		        .build();

        boolean canPresentShareDialogWithPhotos = ShareDialog.canShow(
                SharePhotoContent.class);
        if (canPresentShareDialogWithPhotos) {
            mShareDialog.show(sharePhotoContent);
        } else if (hasPermission(PERMISSION)) {
            ShareApi.share(sharePhotoContent, null);
        } else {
            mPendingAction = PendingAction.POST_PHOTO;
        }
    }
	
	public static boolean isLogin() {
		AccessToken accessToken = AccessToken.getCurrentAccessToken();
		Log.i(TAG, "isLogin() :" + accessToken);
		return accessToken != null;
	}
	
	public static void gameRequests(int cbIndex) {
		sCallIndex = cbIndex;
		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
		        GameRequestContent content = new GameRequestContent.Builder()
                .setMessage("This is Request Message")
                .build();
		        sInstance.mRequestDialog.show(content);
			}
		});
	}
	
	public static void inviteFriends(final String strAppLinkUrl, final String strImagUrl) {
		sCallIndex = 0;
		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				sInstance.FBInvite(strAppLinkUrl, strImagUrl);
			}
		});		
	}

	public void FBInvite(String strAppLinkUrl, String strImagUrl) {
		if (AppInviteDialog.canShow()) {
		    AppInviteContent content = new AppInviteContent.Builder()
		                .setApplinkUrl(strAppLinkUrl)
		                .setPreviewImageUrl(strImagUrl)
		                .build();
		    AppInviteDialog.show(sInstance.mActivity, content);
		}
	}
	
	public static void request(int cbIndex, final String strRequest, final String strPermission) {
		Log.i(TAG, "request type:" + cbIndex + "Request: " + strRequest + " Permission: " + strPermission);
		sCallIndex = cbIndex;

		sInstance.mActivity.runOnUiThread(new Runnable() {
			@Override
			public void run() {
				sInstance.FBRequest(strRequest, strPermission);
			}
		});
	}

	public void FBRequest(String strRequest, String strPermission) {
		
		boolean bPermissionAllow = (strPermission.length() == 0);
		if (!bPermissionAllow) {
			if (hasPermission(strPermission)) {
				bPermissionAllow = true;
			}
		}
		
		if (bPermissionAllow) {
			FBGraphPathRequest(strRequest);
			return;
		}

		// the user didn't grant this permission, so we need to prompt them.
		Log.i(TAG, "request permissions: " + strPermission);
		LoginManager.getInstance().logInWithReadPermissions(mActivity, Arrays.asList(strPermission));
	}
	
	public void FBGraphPathRequest(final String strGraphPath) {
		Log.i(TAG, "FBGraphPathRequest: " + strGraphPath);

		GraphRequest request = GraphRequest.newGraphPathRequest(
				AccessToken.getCurrentAccessToken(), strGraphPath,
				new GraphRequest.Callback() {
					
					@Override
					public void onCompleted(GraphResponse response) {						
						Log.i(TAG, "FBGraphPathRequest onCompleted: " + strGraphPath);
						FacebookRequestError error = response.getError();
						if (error != null) {
							Log.i(TAG, error.toString());
						} else if (response != null) {
							try {
								JSONObject jsonObject = response.getJSONObject();
								String strResponse =  jsonObject.toString();
								Log.i(TAG, "FBGraphPathRequest response: " + strResponse);
								runNativeCallback(sCallIndex, strResponse);
							} catch (Exception e) {
			                    e.printStackTrace();
			                }
						}	
					}
				});
        request.executeAsync();
	}
	

	public static native void nativeCallback(int cbIndex, String params);
	
	public static void runNativeCallback(final int cbIndex, final String params) {
		Cocos2dxGLSurfaceView.getInstance().queueEvent(new Runnable() {
			@Override public void run() {		
				nativeCallback(cbIndex, params);
			} 
		});
	}
}
