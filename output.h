#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>

#include <iostream>
#include "image.h"
#include "calibration.h"


using namespace std;
using namespace cv;

Image<Vec3b> video_homography(string video_file_path, vector<vector<Rect>> &tracking_rectangles, void *data, DetectionParam param);