#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"


#include <iostream>
#include "image.h"


using namespace std;
using namespace cv;

bool overlap(Rect new_rectangle, vector<Rect> tracking_rectangles);