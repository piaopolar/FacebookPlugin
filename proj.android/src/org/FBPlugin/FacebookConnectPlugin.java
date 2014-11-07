package org.FBPlugin;

import java.util.Arrays;
import java.util.List;

import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.json.JSONArray;
import org.json.JSONObject;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.facebook.FacebookRequestError;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.model.GraphObject;

public class FacebookConnectPlugin {
	private static final String TAG = "FacebookPlugin";
	private static FacebookConnectPlugin instance;
	private UiLifecycleHelper uiHelper;
	private Session.StatusCallback statusCallback = new SessionStatusCallback();
	private Activity activity;
	private static int callIndex;
	public FacebookConnectPlugin(Activity activity) {
		this.activity = activity;
		instance = this;
		uiHelper = new UiLifecycleHelper(activity, statusCallback);
	}

	public void onCreate(Bundle savedInstanceState) {
		uiHelper.onCreate(savedInstanceState);
	}

	public static void login(int cbIndex, String scope) {
		callIndex = cbIndex;
		instance.FBLogin();
	}

	public void FBLogin() {
		
		Session session = Session.getActiveSession();
		if (!session.isOpened() && !session.isClosed()) {
			session.openForRead(new Session.OpenRequest(activity)
					.setPermissions(Arrays.asList("public_profile", "user_friends")).setCallback(
							statusCallback));
		} else {
			Session.openActiveSession(activity, true, statusCallback);
		}
	}
	public static void logout(int cbIndex) {
		instance.FBLogout();
		callIndex = cbIndex;
	}
	
	public void FBLogout() {
		 Session session = Session.getActiveSession();
	        if (!session.isClosed()) {
	            session.closeAndClearTokenInformation();
	        }
	}

	public static boolean isLogin() {
		Session session = Session.getActiveSession();
		if (session == null) {
			return false;
		}
		
		boolean bOpened = session.isOpened();
		boolean bClosed = session.isClosed();
		Log.i(TAG, "isLogin() bOpened: " + bOpened + " bClosed: "
				+ bClosed);
		return bOpened && !bClosed;
	}

	public static String getStatus(int cbIndex) {
		
		callIndex = cbIndex;
		
		return instance.getStatus_();
	}
	
	public String getStatus_() {
		
		Session session = Session.getActiveSession();
	
		if (session.getState().equals(SessionState.CREATED))
		{
			return "CREATED";
		}
		else if (session.getState().equals(SessionState.CREATED_TOKEN_LOADED))
		{
			return "CREATED_TOKEN_LOADED";
		}
		else if (session.getState().equals(SessionState.OPENING))
		{
			return "OPENING";
		}
		else if (session.getState().equals(SessionState.OPENED))
		{
			return "OPENED";
		}
		else if (session.getState().equals(SessionState.OPENED_TOKEN_UPDATED))
		{
			return "OPENED_TOKEN_UPDATED";
		}
		else if (session.getState().equals(SessionState.CLOSED_LOGIN_FAILED))
		{
			return "CLOSED_LOGIN_FAILED";
		}
		else if (session.getState().equals(SessionState.CLOSED))
		{
			return "CLOSED";
		}
		
		return "";
	}
	public void onResume() {
		// For scenarios where the main activity is launched and user
		// session is not null, the session state change notification
		// may not be triggered. Trigger it if it's open/closed.
		Session session = Session.getActiveSession();
		if (session != null && (session.isOpened() || session.isClosed())) {
			onSessionStateChange(session, session.getState(), null);
		}
		uiHelper.onResume();
	}

	public void onPause() {
		uiHelper.onPause();
	}

	public void onActivityResult(int requestCode, int resultCode, Intent data) {
		uiHelper.onActivityResult(requestCode, resultCode, data);
	}

	public void onSaveInstanceState(Bundle outState) {
		uiHelper.onSaveInstanceState(outState);
	}

	public void onDestory() {
		uiHelper.onDestroy();
	}
	
	public static void FBGraphPathRequest(final String strGraphPath) {
		Log.i(TAG, "FBGraphPathRequest: " + strGraphPath);
		
		instance.activity.runOnUiThread(new Runnable() {
			public void run() {
				final Session session = Session.getActiveSession();
				
				Log.i(TAG, "FBGraphPathRequest runOnUiThread: " + session + " " + strGraphPath);
				
				Request graphPathRequest = Request.newGraphPathRequest(session, strGraphPath, new Request.Callback() {
					
					@Override
					public void onCompleted(Response response) {
						Log.i(TAG, "FBGraphPathRequest onCompleted: " + strGraphPath);
						
						// Hide the progressContainer now that the network call has completed				
						FacebookRequestError error = response.getError();
						if (error != null) {
							Log.i(TAG, error.toString());
						} else if (session == Session.getActiveSession()) {
							if (response != null) {
								// Get the result
								GraphObject graphObject = response.getGraphObject();
								JSONObject jsonObject = graphObject.getInnerJSONObject();
								String strResponse =  jsonObject.toString();
								Log.i(TAG, "FBGraphPathRequest response: " + strResponse);
								runNativeCallback(callIndex, strResponse);
							}
						}
					}
				});

				Request.executeBatchAsync(graphPathRequest);
			}			
		});		
	}
	
	public static void FBRequest(String strRequest, String strPermission) {
		Session session = Session.getActiveSession();			
		if (session == null || !session.isOpened()) {
			Log.i(TAG, "FBRequest: no session error");
			return;
		}
		
		boolean bPermissionAllow = (strPermission.length() == 0);
		if (!bPermissionAllow) {
			List<String> permissions = session.getPermissions();			 
			if (permissions.contains(strPermission)) {
				bPermissionAllow = true;
			}
		}
		
		if (bPermissionAllow) {
			FBGraphPathRequest(strRequest);
			return;
		}

		// the user didn't grant this permission, so we need to prompt them.
		Log.i(TAG, "request permissions: " + strPermission);
		session.requestNewPublishPermissions(new Session.NewPermissionsRequest(instance.activity, strPermission));
	}

	public static void request(int cbIndex, String strRequest, String strPermission) {
		Log.i(TAG, "request type:" + cbIndex + "Request: " + strRequest + " Permission: " + strPermission);
		callIndex = cbIndex;

		FBRequest(strRequest, strPermission);
	}

	private void onSessionStateChange(Session session, SessionState state,
			Exception exception) {
		if (state.isOpened()) {
			Log.i(TAG, "Logged in...");
		} else if (state.isClosed()) {
			Log.i(TAG, "Logged out...");
		}
	}
	public static native void nativeCallback(int cbIndex, String params);
	
	public static void runNativeCallback(final int cbIndex, final String params) {
		Cocos2dxGLSurfaceView.getInstance().queueEvent(new Runnable() {
			@Override public void run() {		
				nativeCallback(cbIndex, params);
			} 
		});
	}	
	
	
	private class SessionStatusCallback implements Session.StatusCallback {
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			// Respond to session state changes, ex: updating the view
			onSessionStateChange(session, state, exception);
			
			runNativeCallback(callIndex,"");
		}
	}	
}
