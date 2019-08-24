package com.example.ndkgifapplication;

import android.graphics.Bitmap;

/**
 * Created by 海米 on 2018/4/27.
 */

public class GifHandler {
    static {
        System.loadLibrary("native-lib");
    }

    private long gifAddr;

    public GifHandler(String path) {
        this.gifAddr = loadPath(path);
    }

    private native long loadPath(String path);

    private native int getWidth(long ndkGif);

    private native int getHeight(long ndkGif);

    private native int getLength(long ndkGif);

    private native int updateFrame(Bitmap bitmap, int index, long ndkGif);

    private native void recycleGif(long ndkGif);

    public int getWidth() {
        return getWidth(gifAddr);
    }

    public int getHeight() {
        return getHeight(gifAddr);
    }

    public int getLength(){
        return getLength(gifAddr);
    }

    public int updateFrame(Bitmap bitmap, int index) {
        return updateFrame(bitmap,index, gifAddr);
    }
}
