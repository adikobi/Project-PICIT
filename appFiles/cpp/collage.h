#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#include "maxflow/graph.h"
#include "data.h"


using namespace std;
using namespace cv;

enum INSIDE_MODE {DESIGNATED_SOURCE};
enum OUTSIDE_MODE {COLORS, GRADIENTS, COLORS_AND_GRADIENTS};
extern "C" {

class Collage {

private:
	int nImages;
	Data *D;


public:
	Collage(Data *Dat);

	Mat computePhotomontage(INSIDE_MODE insideMode, OUTSIDE_MODE outsideMode);

	static bool isInImage(int x, int y, int offsetX, int offsetY, const Mat &I) {
		return (x - offsetX >= 0 && y - offsetY >= 0 && x - offsetX < I.rows &&
				y - offsetY < I.cols);
	}
};
}