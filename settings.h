#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"
#include "capturethread.h"
#include "processingthread.h"
#include <QDateTime>
#include <QTimer>
#include <QButtonGroup>

class RenderWidget;
class IPCameraForm;

class SettingsWidget : public QWidget {
Q_OBJECT;
public:
  SettingsWidget(QWidget* parent);
  CvSize getSelectedResolution();
  void RecordAVI(bool state);

  void setThreads(CaptureThread *capthread, ProcessingThread *procthread);
  void setRenderWidget(RenderWidget *rw) { this->renderWidget = rw; }

public slots:
  void on640ResToggled(bool on);
  void on320ResToggled(bool on);
  void onThresholdChanged(int value);
  void onAverageChanged(int value);
  void onErodeChanged(int value);
  void onDilateChanged(int value);  
  void onBrowseClicked();
  void onAddCameraClicked();
  void onWriteAVIToggled(bool state);
  void onTimer();
  void onImageSelection(int btn);
  void onRotateSelection(int btn);
  void onBlendChanged(int value);
  void onCameraSelection(int index);
  //
  void loadSettings();
  void saveSettings();

signals:
  void resolutionSelected(CvSize);
  void rotationChanged(int);

  void CameraIndexChanged(int, QString);

protected:
  Ui::SettingsWidget  ui;
  CaptureThread      *capturethread;
  ProcessingThread   *processingthread;
  QTime               AVI_StartTime;
  QTime               AVI_EndTime;
  QTimer              clock;
  QButtonGroup        ImageButtonGroup;
  QButtonGroup        RotateButtonGroup;
  RenderWidget       *renderWidget;
  QString             settingsFileName;
  IPCameraForm       *cameraForm;
};

#endif
