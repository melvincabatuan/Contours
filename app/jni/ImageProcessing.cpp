#include "com_cabatuan_contours_MainActivity.h"
#include <android/log.h>
#include <android/bitmap.h>

#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

#define  LOG_TAG    "Contours"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


  double t; // for measuring time duration

  RNG rng(12345); // for random color

  Mat canny_output;
  Mat drawing;
  std::vector<std::vector<Point> > contours;
  std::vector<Vec4i> hierarchy;


/*
 * Class:     com_cabatuan_contours_MainActivity
 * Method:    process
 * Signature: (Landroid/graphics/Bitmap;[B)V
 */
JNIEXPORT void JNICALL Java_com_cabatuan_contours_MainActivity_process
  (JNIEnv *pEnv, jobject clazz, jobject pTarget, jbyteArray pSource){

   AndroidBitmapInfo bitmapInfo;
   uint32_t* bitmapContent; // Links to Bitmap content

   if(AndroidBitmap_getInfo(pEnv, pTarget, &bitmapInfo) < 0) abort();
   if(bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888) abort();
   if(AndroidBitmap_lockPixels(pEnv, pTarget, (void**)&bitmapContent) < 0) abort();

   /// Access source array data... OK
   jbyte* source = (jbyte*)pEnv->GetPrimitiveArrayCritical(pSource, 0);
   if (source == NULL) abort();

   /// cv::Mat for YUV420sp source and output BGRA 
    Mat srcGray(bitmapInfo.height, bitmapInfo.width, CV_8UC1, (unsigned char *)source);
    Mat mbgra(bitmapInfo.height, bitmapInfo.width, CV_8UC4, (unsigned char *)bitmapContent);

/***********************************************************************************************/
    /// Native Image Processing HERE... 
    LOGI("Starting native image processing...");
    t = (double)getTickCount(); 

    if(canny_output.empty())
         canny_output = Mat(srcGray.size(), srcGray.type());
    
    Canny(srcGray, canny_output, 65, 100);
    
// void findContours(InputOutputArray image, OutputArrayOfArrays contours, OutputArray hierarchy, int mode, int method, Point offset=Point())¶
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  
// void findContours(InputOutputArray image, OutputArrayOfArrays contours, int mode, int method, Point offset=Point())¶
    //findContours( canny_output, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) );

    t = 1000*((double)getTickCount() - t)/getTickFrequency();
    LOGI("Processing took %0.2f ms.", t);

   
    drawing = Mat::zeros( canny_output.size(), CV_8UC3 );

    for( int i = 0; i< contours.size(); i++ )
     {
       Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
       drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
     }


    cvtColor(drawing, mbgra, CV_BGR2BGRA);

    LOGI("Successfully finished native image processing..."); 
/************************************************************************************************/ 
   
   /// Release Java byte buffer and unlock backing bitmap
   pEnv-> ReleasePrimitiveArrayCritical(pSource,source,0);
   if (AndroidBitmap_unlockPixels(pEnv, pTarget) < 0) abort();

}
