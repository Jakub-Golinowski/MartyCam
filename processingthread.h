#ifndef PROCESSING_THREAD_H
#define PROCESSING_THREAD_H

#include <QThread>
#include "cv.h"
#include "highgui.h"
#include "opencv2/core/core.hpp"

class ImageBuffer;
class Filter;
class CvVideoWriter;

class ProcessingThread : public QThread {
public: 
	 ProcessingThread(ImageBuffer* buffer);
  ~ProcessingThread();

  void countPixels(IplImage *image);

  double getMotionPercent() { return this->motionPercent; }
  //
	void setRootFilter(Filter* filter) { rootFilter = filter; }
	void setFlipVertical(bool fv) { flipVertical = fv; }
	void setMotionDetecting(bool md) { this->MotionDetecting = md; }
	void setThreshold(int val) { threshold = val; }
	void setAveraging(double val) { average = val; }
	void setErodeBlockSize(int val) { erodeBlockSize = val; }
	void setDisplayImage(int image) { displayImage = image; }
	void run();

private:
	ImageBuffer *imageBuffer;
	Filter      *rootFilter;
	bool         flipVertical;
  bool         MotionDetecting;
  int          threshold;
  double       average;
	int          erodeBlockSize;
  double       motionPercent;
  int          displayImage;

  //Images to use in the program.
  CvSize    imageSize;
  IplImage* currentImage;
  IplImage* thresholdImage;
  IplImage* blendImage;
  IplImage* movingAverage;
  IplImage* difference;
  IplImage* tempImage;

};

#endif
