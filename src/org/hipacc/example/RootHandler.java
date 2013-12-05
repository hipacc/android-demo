package org.hipacc.example;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import android.content.Context;
import android.os.AsyncTask;
import android.widget.Toast;

class RootHandler {

    private final int POLL_INTERVAL = 100;

    private Context mCtx;
    private Process mProc;
    private DataOutputStream mShellIn;
    private BufferedReader mShellOut;
    private boolean mInitialized = false;
    private boolean mIsFinished = false;

    public RootHandler(Context ctx) {
        mCtx = ctx;
        try {
            mProc = Runtime.getRuntime().exec("su");
            mShellIn = new DataOutputStream(mProc.getOutputStream());
            mShellOut = new BufferedReader(new InputStreamReader(
                    mProc.getInputStream()));
            mInitialized = true;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public String runCommand(String cmd, long millis) {
        String response = null;

        if (mInitialized && !mIsFinished) {
            try {
                mShellIn.writeBytes(cmd + "\n");
                if (millis > 0) {
                    AsyncTask<Void, Void, String> readTask =
                            new AsyncTask<Void, Void, String>() {
                        @Override
                        protected String doInBackground(Void... params) {
                            try {
                                return mShellOut.readLine();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                            return null;
                        }
                    };
                    readTask.execute();
                    try {
                        response = readTask.get(millis, TimeUnit.MILLISECONDS);
                    } catch (ExecutionException e) {
                        e.printStackTrace();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    } catch (TimeoutException e) {
                        readTask.cancel(true);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        return response;
    }

    boolean close(long millis) {
        boolean success = false;

        if (mInitialized && !mIsFinished) {
            try {
                mShellIn.writeBytes("exit\n");
            } catch (IOException e) {
                e.printStackTrace();
            }

            AsyncTask<Void,Void,Integer> waitTask =
                    new AsyncTask<Void,Void,Integer>() {
                @Override
                protected Integer doInBackground(Void... params) {
                    try {
                        mProc.waitFor();
                        return mProc.exitValue();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    return null;
                }
            };
            waitTask.execute();

            long count = 0;
            do {
                try {
                    success = (waitTask.get(POLL_INTERVAL,
                            TimeUnit.MILLISECONDS) == 0);
                    mIsFinished = true;
                    break;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                    break;
                } catch (ExecutionException e) {
                    e.printStackTrace();
                    break;
                } catch (TimeoutException e) {
                    ++count;
                }
            } while (count < millis/POLL_INTERVAL);

            if (!mIsFinished) {
                waitTask.cancel(true);
                Toast.makeText(mCtx, "Requesting root permissions timed out.",
                        Toast.LENGTH_SHORT).show();
                mIsFinished = true;
            }
        }

        return success;
    }
}
