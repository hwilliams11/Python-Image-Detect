#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
using namespace std;
using namespace cv;

class myShape {
public:
	vector<Point> points;
	size_t size;
	char type;

	myShape(vector<Point> points1);	
	struct pointSortX {
		bool operator()(const Point& a, const Point& b) const {
			return (a.x < b.x);
		}
	};
	struct pointSortY {
		bool operator()(const Point& a, const Point& b) const {
			return (a.y < b.y);
		}
	};

	void pointSort();
};