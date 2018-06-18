package com.example.ivan.ewatch;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;

public class InterceptCall extends BroadcastReceiver {
    public String number="";
    @Override
    public void onReceive(final Context context, Intent intent)
    {
        try {
            TelephonyManager telephony = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
            telephony.listen(new PhoneStateListener() {
                @Override
                public void onCallStateChanged(int state, String incomingNumber) {
                    super.onCallStateChanged(state, incomingNumber);
                    number=incomingNumber;
                    MainActivity.getIns().updateUI(number);
                }
            }, PhoneStateListener.LISTEN_CALL_STATE);
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
    }
}