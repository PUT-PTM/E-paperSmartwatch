package com.example.ivan.ewatch;
import android.accessibilityservice.AccessibilityService;
import android.accessibilityservice.AccessibilityServiceInfo;
import android.app.Notification;
import android.content.Context;
import android.os.Parcelable;
import android.view.accessibility.AccessibilityEvent;

public class NotificationAccessibilityService extends AccessibilityService{
    String s="New notification";
    Context context;
    protected void onServiceConnected() {
        context=this;
        AccessibilityServiceInfo info = new AccessibilityServiceInfo();
        info.eventTypes = AccessibilityEvent.TYPE_NOTIFICATION_STATE_CHANGED;
        info.feedbackType = AccessibilityServiceInfo.FEEDBACK_ALL_MASK;
        info.notificationTimeout = 100;
        setServiceInfo(info);
    }

    @Override
    public void onAccessibilityEvent(AccessibilityEvent e) {
        if (e.getEventType() == AccessibilityEvent.TYPE_NOTIFICATION_STATE_CHANGED) {
            Parcelable data = e.getParcelableData();
            if (data instanceof Notification) {
              // MainActivity.getIns().updateNotifications(s);
            }
        }
    }
    @Override
    public void onInterrupt() {

    }
}