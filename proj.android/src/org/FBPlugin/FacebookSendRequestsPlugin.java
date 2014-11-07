package org.FBPlugin;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.widget.FacebookDialog;
import com.facebook.widget.FacebookDialog.PendingCall;
import com.facebook.widget.WebDialog;
import com.facebook.widget.WebDialog.OnCompleteListener;

public class FacebookSendRequestsPlugin {
	private static final String TAG = "FacebookPostPlugin";
	private static FacebookSendRequestsPlugin instance;
	private UiLifecycleHelper uiHelper;
	private Session.StatusCallback statusCallback = new SessionStatusCallback();
	private Activity activity;
	private Handler handler;

	public FacebookSendRequestsPlugin(Activity activity) {
		this.activity = activity;
		instance = this;
		uiHelper = new UiLifecycleHelper(activity, statusCallback);
		handler = new Handler() {
			@Override
			public void handleMessage(Message msg) {
				super.handleMessage(msg);
				switch (msg.what) {
				case 1:
					FBSendRequests();
					break;
				default:
					break;
				}
			}
		};
	}

	public static void sendRequests(int cbIndex) {
		Message message = Message.obtain();
		message.what = 1;
		instance.handler.sendMessage(message);
	}

	public void FBSendRequests() {
		
		Bundle params = new Bundle();
	    params.putString("message", "Learn how to make your Android apps social");

	    WebDialog requestsDialog = (
	        new WebDialog.RequestsDialogBuilder(activity,
	            Session.getActiveSession(),
	            params))
	            .setOnCompleteListener(new OnCompleteListener() {

	                @Override
	                public void onComplete(Bundle values,
	                    FacebookException error) {
	                    if (error != null) {
	                        if (error instanceof FacebookOperationCanceledException) {
	                            Toast.makeText(activity.getApplicationContext(), 
	                                "Request cancelled", 
	                                Toast.LENGTH_SHORT).show();
	                        } else {
	                            Toast.makeText(activity.getApplicationContext(), 
	                                "Network Error", 
	                                Toast.LENGTH_SHORT).show();
	                        }
	                    } else {
	                        final String requestId = values.getString("request");
	                        if (requestId != null) {
	                            Toast.makeText(activity.getApplicationContext(), 
	                                "Request sent",  
	                                Toast.LENGTH_SHORT).show();
	                        } else {
	                            Toast.makeText(activity.getApplicationContext(), 
	                                "Request cancelled", 
	                                Toast.LENGTH_SHORT).show();
	                        }
	                    }   
	                }

	            })
	            .build();
	    requestsDialog.show();
		
	}

	private class SessionStatusCallback implements Session.StatusCallback,
			FacebookDialog.Callback {
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {
			// Respond to session state changes, ex: updating the view
			onSessionStateChange(session, state, exception);
		}

		@Override
		public void onComplete(PendingCall pendingCall, Bundle data) {
			// TODO Auto-generated method stub
			Log.i(TAG, "ShareDialog onComplete");
			boolean didCancel = FacebookDialog.getNativeDialogDidComplete(data);
			String completionGesture = FacebookDialog
					.getNativeDialogCompletionGesture(data);
			String postId = FacebookDialog.getNativeDialogPostId(data);
			Log.i(TAG, "didCancel:" + didCancel + "completionGesture:"
					+ completionGesture + "postId:" + postId);
		}

		@Override
		public void onError(PendingCall pendingCall, Exception error,
				Bundle data) {
			// TODO Auto-generated method stub
			Log.i(TAG, "ShareDialog onError");
			boolean didCancel = FacebookDialog.getNativeDialogDidComplete(data);
			String completionGesture = FacebookDialog
					.getNativeDialogCompletionGesture(data);
			String postId = FacebookDialog.getNativeDialogPostId(data);
			Log.i(TAG, "didCancel:" + didCancel + "completionGesture:"
					+ completionGesture + "postId:" + postId);

		}
	}

	private void onSessionStateChange(Session session, SessionState state,
			Exception exception) {
		if (state.isOpened()) {
			Log.i(TAG, "Logged in...");
		} else if (state.isClosed()) {
			Log.i(TAG, "Logged out...");
		}
	}

	public void onCreate(Bundle savedInstanceState) {
		uiHelper.onCreate(savedInstanceState);
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
		uiHelper.onActivityResult(requestCode, resultCode, data, dialogCallback);
	}

	public void onSaveInstanceState(Bundle outState) {
		uiHelper.onSaveInstanceState(outState);
	}

	public void onDestory() {
		uiHelper.onDestroy();
	}

	private FacebookDialog.Callback dialogCallback = new FacebookDialog.Callback() {
		@Override
		public void onError(FacebookDialog.PendingCall pendingCall,
				Exception error, Bundle data) {
			Log.d("HelloFacebook", String.format("Error: %s", error.toString()));
		}

		@Override
		public void onComplete(FacebookDialog.PendingCall pendingCall,
				Bundle data) {
			Log.d("HelloFacebook", "Success!");
		}
	};
}
