package com.example.maniu015;

import androidx.appcompat.app.AppCompatActivity;

import android.hardware.Camera;
import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import java.io.File;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback,Camera.PreviewCallback {
    OpenCVJni openCVJni;
    CameraHelper helper;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        openCVJni = new OpenCVJni();
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        surfaceView.getHolder().addCallback(this);
        helper = new CameraHelper(Camera.CameraInfo.CAMERA_FACING_FRONT);
        helper.setPreviewCallback(this);
        //Utils.copyAssets(this,"lbpcascade_frontalface.xml");
    }

    @Override
    protected void onResume() {
        super.onResume();
        String path = new File(Environment.getExternalStorageDirectory(),"lbpcascade_frontalface.xml").getAbsolutePath();
        helper.startPreview();
        openCVJni.init(path);
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        openCVJni.postdata(bytes,helper.WIDTH,helper.HEIGHT,helper.getCameraId());
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        openCVJni.setSurface(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
