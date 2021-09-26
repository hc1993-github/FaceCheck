#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "android/native_window_jni.h"
using namespace cv;
ANativeWindow * nwindow = 0;
DetectionBasedTracker * tracker = 0;
int index=0;
class CascadeClassifierAdapter:public DetectionBasedTracker::IDetector{
public:
    CascadeClassifierAdapter(cv::Ptr<CascadeClassifier> detector):IDetector(),Detector(detector){

    }
    void detect(const cv::Mat &image, std::vector<cv::Rect> &objects){
        Detector->detectMultiScale(image,objects,scaleFactor,minNeighbours,0,minObjSize,maxObjSize);
    };
    virtual ~CascadeClassifierAdapter() {
    }
private:
    CascadeClassifierAdapter();
    cv::Ptr<CascadeClassifier> Detector;
};
extern "C"
JNIEXPORT void JNICALL
Java_com_example_maniu015_OpenCVJni_init(JNIEnv *env, jobject thiz, jstring javapath) {
    const char * path = env->GetStringUTFChars(javapath,0);
    //创建检测器
    Ptr<CascadeClassifier> jiance = makePtr<CascadeClassifier>(path);
    Ptr<CascadeClassifierAdapter> jianceadapter = makePtr<CascadeClassifierAdapter>(jiance);
    //创建追踪器
    Ptr<CascadeClassifier> zhuizong = makePtr<CascadeClassifier>(path);
    Ptr<CascadeClassifierAdapter> zhuizongadapter = makePtr<CascadeClassifierAdapter>(zhuizong);
    DetectionBasedTracker::Parameters parameters;
    //开始识别
    tracker = new DetectionBasedTracker(jianceadapter,zhuizongadapter,parameters);
    tracker->run();
    env->ReleaseStringUTFChars(javapath,path);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_maniu015_OpenCVJni_postdata(JNIEnv *env, jobject thiz, jbyteArray javadata, jint width,
                                             jint height, jint cameraid) {
    jbyte * data = env->GetByteArrayElements(javadata,0);
    Mat srcdata(height*3/2,width,CV_8UC1,data);
    //yuv转rgba
    cvtColor(srcdata,srcdata,COLOR_YUV2RGBA_NV21);
    if(cameraid==1){
        //旋转
        rotate(srcdata,srcdata,ROTATE_90_COUNTERCLOCKWISE);
        //翻转
        flip(srcdata,srcdata,1);
    } else{
        rotate(srcdata,srcdata,ROTATE_90_CLOCKWISE);
    }
    Mat graydata;
    //灰度
    cvtColor(srcdata,graydata,COLOR_RGBA2GRAY);
    //二值化
    equalizeHist(graydata,graydata);
    //imwrite("/sdcard/src.jpg",graydata);
    std::vector<Rect> faces;
    //开始检查
    tracker->process(graydata);
    //获取结果
    tracker->getObjects(faces);
    for(Rect face:faces){
        rectangle(srcdata,face,Scalar(255,0,0));
    }
    if (nwindow) {
        ANativeWindow_setBuffersGeometry(nwindow, srcdata.cols, srcdata.rows, WINDOW_FORMAT_RGBA_8888);
        ANativeWindow_Buffer window_buffer;
        do {
            if (ANativeWindow_lock(nwindow, &window_buffer, 0)) {
                ANativeWindow_release(nwindow);
                nwindow = 0;
                break;
            }
            uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);
            int dst_linesize = window_buffer.stride * 4;

            for (int i = 0; i < window_buffer.height; ++i) {
                memcpy(dst_data + i * dst_linesize, srcdata.data + i * srcdata.cols * 4, dst_linesize);
            }
            ANativeWindow_unlockAndPost(nwindow);
        } while (0);
    }
    srcdata.release();
    graydata.release();
    env->ReleaseByteArrayElements(javadata,data,0);
}extern "C"
JNIEXPORT void JNICALL
Java_com_example_maniu015_OpenCVJni_setSurface(JNIEnv *env, jobject thiz, jobject surface) {
    if (nwindow) {
        ANativeWindow_release(nwindow);
        nwindow = 0;
    }
    nwindow = ANativeWindow_fromSurface(env, surface);
}