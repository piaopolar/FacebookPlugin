package org.FBPlugin;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Date;
import java.util.List;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Intent;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.Toast;

import com.facebook.FacebookAuthorizationException;
import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.FacebookRequestError;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
import com.facebook.android.AsyncFacebookRunner;
import com.facebook.model.GraphObject;
import com.facebook.model.GraphPlace;
import com.facebook.model.GraphUser;
import com.facebook.model.OpenGraphAction;
import com.facebook.model.OpenGraphObject;
import com.facebook.widget.*;
import com.facebook.widget.FacebookDialog.*;
import com.facebook.widget.WebDialog.OnCompleteListener;
import com.banana.FBpluginDemo.R;

public class FacebookPostPlugin {
	private static final String TAG = "FacebookPostPlugin";
	private static FacebookPostPlugin instance;
	private UiLifecycleHelper uiHelper;
	private Session.StatusCallback statusCallback = new SessionStatusCallback();
	private Activity activity;
	private Handler handler;
    private PendingAction pendingAction = PendingAction.NONE;
    private static final String PERMISSION = "publish_actions";
    private GraphPlace place;
    private List<GraphUser> tags;

    private enum PendingAction {
        NONE,
        POST_PHOTO,
        POST_STATUS_UPDATE
    }

	static public String name = null;
	static public String caption = null;
	static public String description = null;
	static public String link = null;
	static public String picture = null;
	
	public FacebookPostPlugin(Activity activity) 
	{
		this.activity = activity;
		instance = this;
		uiHelper = new UiLifecycleHelper(activity, statusCallback);
		handler = new Handler()
		{
			@Override
			public void handleMessage(Message msg) 
			{
				super.handleMessage(msg);
				switch(msg.what)
				{
				case 1:
					FBPostPhoto();
					break;
				default:
					break;
				}
			}
		};
	}

	public static void postStatus(int cbIndex,String aName,String aCaption,String aDescription,String aLink,String aPicture)
	{
		name = aName;
		caption = aCaption;
		description = aDescription;
		link = aLink;
		picture = aPicture;
		
		Message message = Message.obtain();
		message.what = 1;
		instance.handler.sendMessage(message);
		//instance.postStatus_();
	}
	
	public static boolean copyFile(String sourceLocation, String destLocation) {
	    try {
	        File sd = Environment.getExternalStorageDirectory();
	        if(sd.canWrite()){
	            File source=new File(sourceLocation);
	            File dest=new File(destLocation);
	            File destDir = dest.getParentFile();
	            
	            if (!destDir.exists()) {
	            	destDir.mkdirs();
	            }
	            
	            if(!dest.exists()){
	                dest.createNewFile();
	            }
	            if(source.exists()){
	                InputStream  src = new FileInputStream(source);
	                OutputStream dst = new FileOutputStream(dest);
	                 // Copy the bits from instream to outstream
	                byte[] buf = new byte[1024];
	                int len;
	                while ((len = src.read(buf)) > 0) {
	                    dst.write(buf, 0, len);
	                }
	                src.close();
	                dst.close();
	            }
	        }
	        return true;
	    } catch (Exception ex) {
	        ex.printStackTrace();
	        return false;
	    }
	}

    private void postPhoto() {
    	boolean canPresentShareDialogWithPhotos = FacebookDialog.canPresentShareDialog(activity,
                FacebookDialog.ShareDialogFeature.PHOTOS);
    	
//		String dst = "/mnt/sdcard/CMWar/scrshot.jpg";
//		copyFile(picture, dst);
//		String imageUrl = "file://";
//		imageUrl = imageUrl + dst;   	
//   	Bitmap image = BitmapFactory.decodeFile(dst);
    	
    	Bitmap image = BitmapFactory.decodeFile(picture);
        if (canPresentShareDialogWithPhotos) {
            FacebookDialog shareDialog = createShareDialogBuilderForPhoto(image).build();
            uiHelper.trackPendingDialogCall(shareDialog.present());
        } else if (hasPublishPermission()) {
            Request request = Request.newUploadPhotoRequest(Session.getActiveSession(), image, new Request.Callback() {
                @Override
                public void onCompleted(Response response) {
                    showPublishResult(activity.getString(R.string.photo_post), response.getGraphObject(), response.getError());
                }
            });
            request.executeAsync();
        } else {
            pendingAction = PendingAction.POST_PHOTO;
        }
    }	
    
	private void publishFeedDialog() {
		Bundle params = new Bundle();

		Log.i("name", name);
		Log.i("caption",caption);
		Log.i("description",description);
		Log.i("link",link);
		Log.i("picture",picture);
		
		params.putString("name", name);
		params.putString("caption",caption);
		params.putString("description",description);
		params.putString("link",link);
		params.putString("picture",picture);
		
		WebDialog feedDialog = (new WebDialog.FeedDialogBuilder(activity,
				Session.getActiveSession(), params)).setOnCompleteListener(
				new OnCompleteListener() {

					@Override
					public void onComplete(Bundle values,
							FacebookException error) {
						if (error == null) {
							// When the story is posted, echo the success
							// and the post Id.
							final String postId = values.getString("post_id");
							if (postId != null) {
								Toast.makeText(activity,
										"Posted story, id: " + postId,
										Toast.LENGTH_SHORT).show();
							} else {
								// User clicked the Cancel button
								Toast.makeText(
										activity.getApplicationContext(),
										"Publish cancelled", Toast.LENGTH_SHORT)
										.show();
							}
						} else if (error instanceof FacebookOperationCanceledException) {
							// User clicked the "x" button
							Toast.makeText(activity.getApplicationContext(),
									"Publish cancelled", Toast.LENGTH_SHORT)
									.show();
						} else {
							// Generic, ex: network error
							Toast.makeText(activity.getApplicationContext(),
									"Error posting story", Toast.LENGTH_SHORT)
									.show();
						}
					}

				}).build();
		feedDialog.show();
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

    private void onSessionStateChange(Session session, SessionState state, Exception exception) {
        if (pendingAction != PendingAction.NONE &&
                (exception instanceof FacebookOperationCanceledException ||
                exception instanceof FacebookAuthorizationException)) {
                new AlertDialog.Builder(activity)
                    .setTitle(R.string.cancelled)
                    .setMessage(R.string.permission_not_granted)
                    .setPositiveButton(R.string.ok, null)
                    .show();
            pendingAction = PendingAction.NONE;
        } else if (state == SessionState.OPENED_TOKEN_UPDATED) {
            handlePendingAction();
        }
    }

    @SuppressWarnings("incomplete-switch")
    private void handlePendingAction() {
        PendingAction previouslyPendingAction = pendingAction;
        // These actions may re-set pendingAction if they are still pending, but we assume they
        // will succeed.
        pendingAction = PendingAction.NONE;

        switch (previouslyPendingAction) {
            case POST_PHOTO:
                postPhoto();
                break;
            case POST_STATUS_UPDATE:
                postStatusUpdate();
                break;
        }
    }

    private interface GraphObjectWithId extends GraphObject {
        String getId();
    }

    private void showPublishResult(String message, GraphObject result, FacebookRequestError error) {
        String title = null;
        String alertMessage = null;
        if (error == null) {
            title = activity.getString(R.string.success);
            String id = result.cast(GraphObjectWithId.class).getId();
            alertMessage = activity.getString(R.string.successfully_posted_post, message, id);
        } else {
            title = activity.getString(R.string.error);
            alertMessage = error.getErrorMessage();
        }

        new AlertDialog.Builder(activity)
                .setTitle(title)
                .setMessage(alertMessage)
                .setPositiveButton(R.string.ok, null)
                .show();
    }

    private void onClickPostStatusUpdate() {
    	boolean canPresentShareDialog = FacebookDialog.canPresentShareDialog(activity,
                FacebookDialog.ShareDialogFeature.SHARE_DIALOG);
        performPublish(PendingAction.POST_STATUS_UPDATE, canPresentShareDialog);
    }
    

    private FacebookDialog.ShareDialogBuilder createShareDialogBuilderForLink() {
        return new FacebookDialog.ShareDialogBuilder(activity)
                .setName("Hello Facebook")
                .setDescription("The 'Hello Facebook' sample application showcases simple Facebook integration")
                .setLink("http://developers.facebook.com/android");
    }

    private void postStatusUpdate() {
    	boolean canPresentShareDialog = FacebookDialog.canPresentShareDialog(activity,
                FacebookDialog.ShareDialogFeature.SHARE_DIALOG);
        if (canPresentShareDialog) {
            FacebookDialog shareDialog = createShareDialogBuilderForLink().build();
            uiHelper.trackPendingDialogCall(shareDialog.present());
        } else if (hasPublishPermission()) {
            final String message = activity.getString(R.string.status_update);
            Request request = Request
                    .newStatusUpdateRequest(Session.getActiveSession(), message, place, tags, new Request.Callback() {
                        @Override
                        public void onCompleted(Response response) {
                            showPublishResult(message, response.getGraphObject(), response.getError());
                        }
                    });
            request.executeAsync();
        } else {
            pendingAction = PendingAction.POST_STATUS_UPDATE;
        }
    }

    private void FBPostPhoto() {
    	boolean canPresentShareDialogWithPhotos = FacebookDialog.canPresentShareDialog(activity,
                FacebookDialog.ShareDialogFeature.PHOTOS);
        performPublish(PendingAction.POST_PHOTO, canPresentShareDialogWithPhotos);
    }
    
    private FacebookDialog.PhotoShareDialogBuilder createShareDialogBuilderForPhoto(Bitmap... photos) {
        return new FacebookDialog.PhotoShareDialogBuilder(activity)
                .addPhotos(Arrays.asList(photos));
    }
	
    private void showAlert(String title, String message) {
        new AlertDialog.Builder(activity)
                .setTitle(title)
                .setMessage(message)
                .setPositiveButton(R.string.ok, null)
                .show();
    }

    private boolean hasPublishPermission() {
        Session session = Session.getActiveSession();
        return session != null && session.getPermissions().contains("publish_actions");
    }

    private void performPublish(PendingAction action, boolean allowNoSession) {
        Session session = Session.getActiveSession();
        if (session != null) {
            pendingAction = action;
            if (hasPublishPermission()) {
                // We can do the action right away.
                handlePendingAction();
                return;
            } else if (session.isOpened()) {
                // We need to get new permissions, then complete the action when we get called back.
                session.requestNewPublishPermissions(new Session.NewPermissionsRequest(activity, PERMISSION));
                return;
            }
        }

        if (allowNoSession) {
            pendingAction = action;
            handlePendingAction();
        }
    }
}
