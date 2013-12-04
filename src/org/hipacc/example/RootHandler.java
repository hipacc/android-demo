package org.hipacc.example;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import android.content.Context;
import android.os.AsyncTask;
import android.widget.Toast;

class RootHandler {

    private final int POLL_INTERVAL = 100;
    private final int WAIT_OUTPUT = 1000;

    private Context mCtx;
    private Process mProc;
    private DataOutputStream mShellIn;
    private BufferedReader mShellOut;
    private List<String> mReturns;
    private AsyncTask<Void,Void,Void> mTask;
    private boolean mInitialized = false;
    private boolean mIsFinished = false;
    private boolean mSuccess = false;

    public RootHandler(Context ctx) {
        mCtx = ctx;
        try {
            mProc = Runtime.getRuntime().exec("su");
            mShellIn = new DataOutputStream(mProc.getOutputStream());
            mShellOut = new BufferedReader(new InputStreamReader(
                    mProc.getInputStream()));
            mReturns = new ArrayList<String>();
            mTask = new AsyncTask<Void,Void,Void>() {
                @Override
                protected Void doInBackground(Void... params) {
                    try {
                        mShellIn.writeBytes("exit\n");
                        mProc.waitFor();
                        if (mProc.exitValue() == 0) {
                            mSuccess = true;
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    return null;
                }
            };
            mInitialized = true;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public int addCommand(String cmd, boolean doReadLine) {
        if (mInitialized) {
            try {
                mShellIn.writeBytes(cmd + "\n");
                if (doReadLine) {
                    AsyncTask<Void, Void, Void> readLine =
                            new AsyncTask<Void, Void, Void>() {
                        @Override
                        protected Void doInBackground(Void... params) {
                            try {
                                mReturns.add(mShellOut.readLine());
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                            return null;
                        }
                    };
                    readLine.execute();
                    try {
                        readLine.get(WAIT_OUTPUT, TimeUnit.MILLISECONDS);
                    } catch (ExecutionException e) {
                        e.printStackTrace();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    } catch (TimeoutException e) {
                        readLine.cancel(true);
                    }
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            return doReadLine ? mReturns.size()-1 : -1;
        } else {
            return -1;
        }
    }

    public String getReturn(int id) {
        if (mInitialized && id >= 0 && id < mReturns.size()) {
            return mReturns.get(id);
        } else {
            return "";
        }
    }

    boolean startAndWait(long millis) {
        if (mInitialized && !mIsFinished) {
            //Toast.makeText(mCtx, "Requesting root permissions.",
            //        Toast.LENGTH_SHORT).show();

            mTask.execute();

            for (long i = 0; i < millis/POLL_INTERVAL; ++i) {
                try {
                    mTask.get(POLL_INTERVAL, TimeUnit.MILLISECONDS);
                    mIsFinished = true;
                    break;
                } catch (InterruptedException e) {
                    e.printStackTrace();
                } catch (ExecutionException e) {
                    e.printStackTrace();
                } catch (TimeoutException e) {
                }
            }

            if (!mIsFinished) {
                mTask.cancel(true);
                Toast.makeText(mCtx, "Requesting root permissions timed out.",
                        Toast.LENGTH_SHORT).show();
                mIsFinished = true;
            }

            return mSuccess;
        } else {
            return false;
        }
    }
}
