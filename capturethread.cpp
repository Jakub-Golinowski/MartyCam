#include <iostream>
#include "capturethread.h"
#include "imagebuffer.h"
#include <QDebug>
#include <QTime>
//----------------------------------------------------------------------------
CaptureThread::CaptureThread(ImageBuffer* buffer, int device) : QThread()
{
  this->abort             = false;
  this->captureActive     = false;
  this->fps               = 0.0 ;
  this->deviceIndex       = -1;
  this->imageSize.width   = 640;
  this->imageSize.height  = 480;
  this->AVI_Writing       = false;
  this->AVI_Writer        = NULL; 
  this->capture           = NULL;
  this->imageBuffer       = buffer;
  this->setDeviceIndex(device);
}
//----------------------------------------------------------------------------
CaptureThread::~CaptureThread() 
{  
  this->closeAVI();
  // Release our stream capture object
  cvReleaseCapture(&capture);
}
//----------------------------------------------------------------------------
void CaptureThread::setDeviceIndex(int index)
{
  if (this->deviceIndex!=index) {
    bool active = this->captureActive;
    this->stopCapture();
    if (this->capture) {
      cvReleaseCapture(&capture);
    }
    this->deviceIndex = index;
    capture = cvCaptureFromCAM(CV_CAP_DSHOW + this->deviceIndex );
    if (!capture) {
      throw std::exception("Camera capture failed");
    }
    if (active) {
      this->startCapture(30, Size640);
    }
  }
}
//----------------------------------------------------------------------------
void CaptureThread::run() {  
  QTime time;
  time.start();
  while (!this->abort) {
    if (!captureActive) {
      captureLock.lock();
      fps = 0;
      frameTimes.clear();
      captureWait.wait(&captureLock);
      time.restart();
      updateFPS(time.elapsed());
      captureLock.unlock();
    }
    // get latest frame from webcam
    IplImage *frame = cvQueryFrame(capture);
    updateFPS(time.elapsed());
    // always write the frame out if saving movie
    if (this->AVI_Writing) {
      this->saveAVI(frame);
    }
    // add to queue if space is available, 
    // otherwise drop the frame from further processing
    if (!imageBuffer->isFull()) {
      imageBuffer->addFrame(frame);
    }
  }
}
//----------------------------------------------------------------------------
void CaptureThread::updateFPS(int time) {
  frameTimes.enqueue(time);
  if (frameTimes.size() > 15) {
    frameTimes.dequeue();
  }
  if (frameTimes.size() > 1) {
    fps = frameTimes.size()/((double)time-frameTimes.head())*1000.0;
  }
  else {
    fps = 0;
  }
}
//----------------------------------------------------------------------------
bool CaptureThread::startCapture(int framerate, FrameSize size) {
  if (!captureActive) {
/*
    if (size == Size640) {
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 640);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 480);
    }
    else if (size == Size320) {
*/
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
//    }
    // qDebug() << "Listing capture properties...";
    // qDebug() << "CV_CAP_PROP_FRAME_WIDTH" << cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
    // qDebug() << "CV_CAP_PROP_FRAME_HEIGHT" << cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
    // qDebug() << "CV_CAP_PROP_FPS" << cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
    // qDebug() << "CV_CAP_PROP_FOURCC" << cvGetCaptureProperty(capture, CV_CAP_PROP_FOURCC);
    // qDebug() << "CV_CAP_PROP_BRIGHTNESS" << cvGetCaptureProperty(capture, CV_CAP_PROP_BRIGHTNESS);
    // qDebug() << "CV_CAP_PROP_CONTRAST" << cvGetCaptureProperty(capture, CV_CAP_PROP_CONTRAST);
    // qDebug() << "CV_CAP_PROP_SATURATION" << cvGetCaptureProperty(capture, CV_CAP_PROP_SATURATION);
    // qDebug() << "CV_CAP_PROP_HUE" << cvGetCaptureProperty(capture, CV_CAP_PROP_HUE);
    // qDebug() << "Done";
    // qDebug() << "Attempting to set frame rate...";
    // qDebug() << "Error:" << cvSetCaptureProperty(capture, CV_CAP_PROP_FPS, framerate);
    // qDebug() << "Starting to track";
    captureActive = true;
    captureWait.wakeAll();
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------
void CaptureThread::stopCapture() {
  captureActive = false;
}
//----------------------------------------------------------------------------
void CaptureThread::saveAVI(IplImage *image) 
{
  //CV_FOURCC('M', 'J', 'P', 'G'),
  //CV_FOURCC('M', 'P', '4', '2') = MPEG-4.2 codec
  //CV_FOURCC('D', 'I', 'V', '3') = MPEG-4.3 codec
  //CV_FOURCC('D', 'I', 'V', 'X') = MPEG-4 codec
  if (!this->AVI_Writer) {
    std::string path = this->AVI_Directory + "/" + this->AVI_Name + std::string(".avi");
    this->AVI_Writer = cvCreateVideoWriter(
      path.c_str(),
      0,  
      // 15.0,  
      this->getFPS(),
      imageSize
    );
    emit(RecordingState(true));
  }
  cvWriteFrame(this->AVI_Writer, image);
}
//----------------------------------------------------------------------------
void CaptureThread::closeAVI() 
{
  this->AVI_Writing = false;
  if (this->AVI_Writer) {
    cvReleaseVideoWriter(&this->AVI_Writer);
    emit(RecordingState(false));
  }
  this->AVI_Writer = NULL;
}
//----------------------------------------------------------------------------
void CaptureThread::setWriteAVIDir(const char *dir)
{
  this->AVI_Directory = dir;
}
//----------------------------------------------------------------------------
void CaptureThread::setWriteAVIName(const char *name)
{
  this->AVI_Name = name;
}
//----------------------------------------------------------------------------
