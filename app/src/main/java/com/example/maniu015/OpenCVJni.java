package com.example.maniu015;

import android.view.Surface;

public class OpenCVJni {
    static {
        System.loadLibrary("native-lib");
    }
    public native void init(String path);
    public native void postdata(byte[] dat,int width,int height,int cameraid);

    public native void setSurface(Surface surface);
}
