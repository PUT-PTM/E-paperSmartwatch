package com.example.ivan.ewatch;
import android.Manifest;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;
import android.widget.EditText;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity
{
    private static MainActivity ins;
    String battery,phoneNumber;
    TextView myLabel;
    TextView textView,textView_wychodzacy;
    TextView textView2,textView3;
    EditText myTextbox;
    BluetoothAdapter mBluetoothAdapter;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    OutputStream mmOutputStream;
    InputStream mmInputStream;
    Thread workerThread;
    boolean call=false;
    boolean notification=false;
    byte[] readBuffer;
    int readBufferPosition;
    volatile boolean stopWorker;


    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ins=this;

        this.registerReceiver(this.mBatInfoReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));

        if(ContextCompat.checkSelfPermission(MainActivity.this,
                Manifest.permission.READ_PHONE_STATE)!=PackageManager.PERMISSION_GRANTED)
        {
            if(ActivityCompat.shouldShowRequestPermissionRationale(MainActivity.this,
                    Manifest.permission.READ_PHONE_STATE)){
                ActivityCompat.requestPermissions(MainActivity.this,
                        new String[]{Manifest.permission.READ_PHONE_STATE},1);
            } else {
                ActivityCompat.requestPermissions(MainActivity.this,
                        new String[]{Manifest.permission.READ_PHONE_STATE},1);
            }
        } else {
        }




        Button openButton = (Button)findViewById(R.id.open);
        Button sendButton = (Button)findViewById(R.id.send);
        Button closeButton = (Button)findViewById(R.id.close);
        myLabel = (TextView)findViewById(R.id.label);
        myTextbox = (EditText)findViewById(R.id.entry);
        textView = (TextView) findViewById(R.id.textView1);
        textView2 = (TextView) findViewById(R.id.textView2);
        textView3 = (TextView) findViewById(R.id.textView3);
        textView_wychodzacy=(TextView) findViewById(R.id.textView4);

        textView2.setText("Przychodzace");
        textView3.setText("Wychodzace");


        //Open Button
        openButton.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                try
                {
                    findBT();
                    openBT();
                }
                catch (IOException ex) { }
            }
        });

        //Send Button
        sendButton.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                try
                {
                    sendData();
                }
                catch (IOException ex) { }
            }
        });

        //Close button
        closeButton.setOnClickListener(new View.OnClickListener()
        {
            public void onClick(View v)
            {
                try
                {
                    closeBT();
                }
                catch (IOException ex) { }
            }
        });
    }
    public static MainActivity getIns()
    {
        return ins;
    }
    protected void onDestroy() {
        super.onDestroy();
    }

    public void updateUI(final String s)
    {
        MainActivity.this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                phoneNumber=s;
                if(s.isEmpty())call=false;
                if(!s.isEmpty())call=true;
            }
        });
    }

    void findBT()
    {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter == null)
        {
            myLabel.setText("No bluetooth adapter available");
        }

        if(!mBluetoothAdapter.isEnabled())
        {
            Intent enableBluetooth = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBluetooth, 0);
        }

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if(pairedDevices.size() > 0)
        {
            for(BluetoothDevice device : pairedDevices)
            {
                if(device.getName().equals("HC-05"))
                {
                    mmDevice = device;
                    break;
                }
            }
        }
        myLabel.setText("Bluetooth Device Found");
    }

    void openBT() throws IOException
    {
        UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //Standard SerialPortService ID
        mmSocket = mmDevice.createRfcommSocketToServiceRecord(uuid);
        mmSocket.connect();
        mmOutputStream = mmSocket.getOutputStream();
        mmInputStream = mmSocket.getInputStream();

        beginListenForData();

        myLabel.setText("Bluetooth Opened");
    }

    void beginListenForData()
    {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character


        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        workerThread = new Thread(new Runnable()
        {
            public void run()
            {
                while(!Thread.currentThread().isInterrupted() && !stopWorker)
                {
                    try
                    {
                        int bytesAvailable = mmInputStream.available();
                        if(bytesAvailable > 0)
                        {
                            byte[] packetBytes = new byte[bytesAvailable];
                            mmInputStream.read(packetBytes);
                            for(int i=0;i<bytesAvailable;i++)
                            {
                                byte b = packetBytes[i];
                                if(b == delimiter)
                                {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "UTF-8");
                                    final AudioManager audioManager = (AudioManager) getBaseContext().getApplicationContext().getSystemService(Context.AUDIO_SERVICE);
                                    readBufferPosition = 0;
                                        handler.post(new Runnable() {
                                            public void run() {
                                                textView.setText(data);
                                                if(call)
                                                {
                                                    try
                                                    {
                                                        sendData("2!"+phoneNumber);
                                                    }
                                                    catch (IOException e)
                                                    {
                                                        e.printStackTrace();
                                                    }
                                                }
                                                else if(data.equals("battery")&&!call)
                                                {
                                                    try
                                                    {
                                                        sendData("1!"+battery);
                                                    }
                                                    catch(IOException e)
                                                    {
                                                        e.printStackTrace();
                                                    }
                                                }
                                                else if(data.equals("time")&&!call)
                                                {
                                                    Date date=new Date();
                                                    SimpleDateFormat sdf=new SimpleDateFormat("HH:mm:ss");
                                                    String currentDate = sdf.format(date);
                                                    try {
                                                        sendData("0!" + currentDate);
                                                    } catch (IOException e) {
                                                        e.printStackTrace();
                                                    }
                                                }
                                                else if(data.equals("nots")&&!call)
                                                {
                                                    try {
                                                        sendData("3!" + "nots");
                                                    } catch (IOException e) {
                                                        e.printStackTrace();
                                                    }
                                                }
                                                else if(data.equals("sound")&&!call)
                                                {

                                                    AudioManager am = (AudioManager)getSystemService(Context.AUDIO_SERVICE);

                                                    switch (am.getRingerMode()) {
                                                        case AudioManager.RINGER_MODE_SILENT:
                                                            try {
                                                                sendData("4!" + "Wyciszony.");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                        case AudioManager.RINGER_MODE_VIBRATE:
                                                            try {
                                                                sendData("4!" + "Wibracje.");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                        case AudioManager.RINGER_MODE_NORMAL:
                                                            try {
                                                                sendData("4!" + "Dzwiek.");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                    }
                                                }
                                                else if(data.equals("chsound")&&!call)
                                                {
                                                    int maxVolume = audioManager.getStreamMaxVolume(AudioManager.STREAM_RING);
                                                    AudioManager am = (AudioManager)getSystemService(Context.AUDIO_SERVICE);
                                                    switch (am.getRingerMode()) {
                                                        case AudioManager.RINGER_MODE_SILENT:
                                                            try {
                                                                am.setRingerMode(AudioManager.RINGER_MODE_VIBRATE);
                                                                sendData("4!" + "sound");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                        case AudioManager.RINGER_MODE_VIBRATE:
                                                            try {
                                                                audioManager.setStreamVolume(AudioManager.STREAM_RING, maxVolume, AudioManager.FLAG_SHOW_UI + AudioManager.FLAG_PLAY_SOUND);
                                                                sendData("4!" + "sound");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                        case AudioManager.RINGER_MODE_NORMAL:
                                                            try {
                                                                am.setRingerMode(AudioManager.RINGER_MODE_SILENT);
                                                                sendData("4!" + "sound");
                                                            } catch (IOException e) {
                                                                e.printStackTrace();
                                                            }
                                                            break;
                                                    }
                                                }

                                                else if(data.equals("chsound2")&&!call)
                                                {
                                                    try {
                                                        audioManager.setRingerMode(AudioManager.RINGER_MODE_VIBRATE);
                                                        sendData("4!" + "sound");
                                                    } catch (IOException e) {
                                                        e.printStackTrace();
                                                    }
                                                }
                                            }
                                        });
                                }
                                else
                                {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        }

                    }
                    catch (IOException ex)
                    {
                        stopWorker = true;
                    }
                }
            }
        });
        workerThread.start();
    }

    void sendData() throws IOException
    {
        String msg = myTextbox.getText().toString();
        msg += "\n";
        mmOutputStream.write(msg.getBytes());
        myLabel.setText("Data Sent");
    }

    void sendData(String msg) throws IOException
    {
        msg += "\n";
        mmOutputStream.write(msg.getBytes());
        myLabel.setText("Data Sent");
    }

    void closeBT() throws IOException
    {
        stopWorker = true;
        mmOutputStream.close();
        mmInputStream.close();
        mmSocket.close();
        myLabel.setText("Bluetooth Closed");
    }

         BroadcastReceiver mBatInfoReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
                battery=String.valueOf(level) + "%";
                textView.setText(battery);
            }
        };
    public void onRequestPermissionResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults){
        switch(requestCode)
        {
            case 1:
            {
                if(grantResults.length>0&&grantResults[0]==PackageManager.PERMISSION_GRANTED) {
                    if(ContextCompat.checkSelfPermission(MainActivity.this,
                            Manifest.permission.READ_PHONE_STATE)==PackageManager.PERMISSION_GRANTED)
                    {
                        Toast.makeText(this,"Permission granted", Toast.LENGTH_SHORT).show();
                    }
                }
                else
                {
                    Toast.makeText(this, "No permission grated",Toast.LENGTH_SHORT).show();
                }
            }
        }
    }
}