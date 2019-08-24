#include <jni.h>
#include <string>
#include <malloc.h>
#include <android/bitmap.h>
#include "gif/gif_lib.h"

extern "C"
JNIEXPORT jstring

JNICALL
Java_com_example_ndkgifapplication_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT GifFileType* JNICALL
Java_com_example_ndkgifapplication_GifHandler_loadPath(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);

    // TODO
    int err;
    GifFileType* gif = DGifOpenFileName(path,&err);

    err = DGifSlurp(gif);

    env->ReleaseStringUTFChars(path_, path);
    return gif;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkgifapplication_GifHandler_getWidth__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    // TODO
    return ((GifFileType *) ndkGif)->SWidth;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkgifapplication_GifHandler_getHeight__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    // TODO
    return ((GifFileType *) ndkGif)->SHeight;
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkgifapplication_GifHandler_getLength__J(JNIEnv *env, jobject instance, jlong ndkGif) {

    // TODO
    GifFileType *gif_handle = (GifFileType *) ndkGif;
    return gif_handle->ImageCount;
}

#define  argb(a,r,g,b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)
#define  dispose(ext) (((ext)->Bytes[0] & 0x1c) >> 2)
#define  trans_index(ext) ((ext)->Bytes[3])
#define  transparency(ext) ((ext)->Bytes[0] & 1)
#define  delay(ext) (10*((ext)->Bytes[2] << 8 | (ext)->Bytes[1]))
int drawFrame(GifFileType* gif, AndroidBitmapInfo*  info, int* pixels, int frame_no, bool force_dispose_1) {

GifColorType *bg;

GifColorType *color;

SavedImage * frame;

ExtensionBlock * ext = 0;

GifImageDesc * frameInfo;

ColorMapObject * colorMap;

int *line;

int width, height,x,y,j,loc,n,inc,p;

int* px;



width = gif->SWidth;

height = gif->SHeight;



frame = &(gif->SavedImages[frame_no]);

frameInfo = &(frame->ImageDesc);

if (frameInfo->ColorMap) {

colorMap = frameInfo->ColorMap;

} else {

colorMap = gif->SColorMap;

}



bg = &colorMap->Colors[gif->SBackGroundColor];



for (j=0; j<frame->ExtensionBlockCount; j++) {

if (frame->ExtensionBlocks[j].Function == GRAPHICS_EXT_FUNC_CODE) {

ext = &(frame->ExtensionBlocks[j]);

break;

}

}



// For dispose = 1, we assume its been drawn

px = pixels;

if (ext && dispose(ext) == 1 && force_dispose_1 && frame_no > 0) {

drawFrame(gif, info, pixels, frame_no-1, true);

}

else if (ext && dispose(ext) == 2 && bg) {

for (y=0; y<height; y++) {

line = (int*) px;

for (x=0; x<width; x++) {

line[x] = argb(255, bg->Red, bg->Green, bg->Blue);

}

px = (int *) ((char*)px + info->stride);

}

} else if (ext && dispose(ext) == 3 && frame_no > 1) {

drawFrame(gif, info, pixels, frame_no-2, true);

}

px = pixels;

if (frameInfo->Interlace) {

n = 0;

inc = 8;

p = 0;

px = (int *) ((char*)px + info->stride * frameInfo->Top);

for (y=frameInfo->Top; y<frameInfo->Top+frameInfo->Height; y++) {

for (x=frameInfo->Left; x<frameInfo->Left+frameInfo->Width; x++) {

loc = (y - frameInfo->Top)*frameInfo->Width + (x - frameInfo->Left);

if (ext && frame->RasterBits[loc] == trans_index(ext) && transparency(ext)) {

continue;

}



color = (ext && frame->RasterBits[loc] == trans_index(ext)) ? bg : &colorMap->Colors[frame->RasterBits[loc]];

if (color)

line[x] = argb(255, color->Red, color->Green, color->Blue);

}

px = (int *) ((char*)px + info->stride * inc);

n += inc;

if (n >= frameInfo->Height) {

n = 0;

switch(p) {

case 0:

px = (int *) ((char *)pixels + info->stride * (4 + frameInfo->Top));

inc = 8;

p++;

break;

case 1:

px = (int *) ((char *)pixels + info->stride * (2 + frameInfo->Top));

inc = 4;

p++;

break;

case 2:

px = (int *) ((char *)pixels + info->stride * (1 + frameInfo->Top));

inc = 2;

p++;

}

}

}

}

else {

px = (int *) ((char*)px + info->stride * frameInfo->Top);

for (y=frameInfo->Top; y<frameInfo->Top+frameInfo->Height; y++) {

line = (int*) px;

for (x=frameInfo->Left; x<frameInfo->Left+frameInfo->Width; x++) {

loc = (y - frameInfo->Top)*frameInfo->Width + (x - frameInfo->Left);

if (ext && frame->RasterBits[loc] == trans_index(ext) && transparency(ext)) {

continue;

}

color = (ext && frame->RasterBits[loc] == trans_index(ext)) ? bg : &colorMap->Colors[frame->RasterBits[loc]];

if (color)

line[x] = argb(255, color->Red, color->Green, color->Blue);

}

px = (int *) ((char*)px + info->stride);

}
}

return delay(ext);
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_example_ndkgifapplication_GifHandler_updateFrame__Landroid_graphics_Bitmap_2IJ(JNIEnv *env,
                                                                             jobject instance,
                                                                             jobject bitmap,
                                                                             jint index,
                                                                             jlong ndkGif) {

    // TODO
    GifFileType *gif_handle = (GifFileType *) ndkGif;

    AndroidBitmapInfo  info;

    void*              pixels;

    int                ret;



    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {

        return -1;
    }



    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {

        return -1;
    }


    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        return -1;
    }

    int delay_time = drawFrame(gif_handle, &info, (int*)pixels,index, false);

    AndroidBitmap_unlockPixels(env, bitmap);

    return delay_time;

}extern "C"
JNIEXPORT void JNICALL
Java_com_example_ndkgifapplication_GifHandler_recycleGif(JNIEnv *env, jobject instance, jlong ndkGif) {

// TODO
GifFileType* gif_handle = (GifFileType*)ndkGif;

free(gif_handle->UserData);

DGifCloseFile(gif_handle);
}