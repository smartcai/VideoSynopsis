#ifndef VEHICLECOLORRECOGNITION_H
#define VEHICLECOLORRECOGNITION_H

/************************************************************************/
/* ���루cv::Mat���� ������ɫͼƬ;
   �����std::string���� ������*/
/************************************************************************/

#include <string>
#include <opencv2/highgui/highgui.hpp>

std::string recognizeVehicleColor(const cv::Mat& bgrImg);

#endif // VEHICLECOLORRECOGNITION_H