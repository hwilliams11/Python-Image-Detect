#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

struct myShape {
	vector<Point> points;
	size_t size;
	myShape(vector<Point> points) {
		this->points = points;
		this->size = this->points.size();
	}
};

vector<myShape> myShapes;

// check for duplicates function, returns true if duplicate, otherwise returns false
bool checkShapes(vector<Point> shapeVector) {
	int count = 0;
	size_t shapeSize = shapeVector.size();
	bool * alreadyUsed = new bool[shapeSize];
	for (int z = 0; z < shapeSize; z++)
		alreadyUsed[z] = false;
	double xrange1, yrange1, xrange2, yrange2;
	for (int y = 0; y < myShapes.size(); y++) {
		if (myShapes[y].size == shapeSize) {
			for (int i = 0; i < shapeSize; i++) {
				xrange1 = shapeVector[i].x * .80;
				xrange2 = shapeVector[i].x * 1.20;
				yrange1 = shapeVector[i].y * .80;
				yrange2 = shapeVector[i].y * 1.20;
				for (int g = 0; g < shapeSize; g++) {
					if (myShapes[y].points[g].x <= xrange2 && myShapes[y].points[g].x >= xrange1 && myShapes[y].points[g].y <= yrange2 && myShapes[y].points[g].y >= yrange1 && !alreadyUsed[g]) {
						count += 1;
						alreadyUsed[g] = true;
						break;
					}					
				}
			}
			if (count == shapeSize) {
				delete[] alreadyUsed;
				return true;
			}
			count = 0;
			for (int z = 0; z < shapeSize; z++)
				alreadyUsed[z] = false;
		}
	}
	delete[] alreadyUsed;
	return false; 
}

/// Function header
void thresh_callback(int, void*);

/** @function main */
int main(int argc, char** argv)
{
	/// Load source image and convert it to gray
	src = imread("C:/Users/Nicholas/Desktop/test.png");

	/// Convert image to gray and blur it
	cvtColor(src, src_gray, CV_BGR2GRAY);
	blur(src_gray, src_gray, Size(3, 3));

	/// Create Window
	char* source_window = "Source";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, src);

	createTrackbar(" Canny thresh:", "Source", &thresh, max_thresh, thresh_callback);
	thresh_callback(0, 0);

	waitKey(0);
	return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> approxShape;

	/// Detect edges using canny
	Canny(src_gray, canny_output, thresh, thresh * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], approxShape, arcLength(Mat(contours[i]), true)*0.05, true); // puts the vertexes of the contour into approxShape
			if (checkShapes(approxShape)) { // shape is already in the display shape list, meaning it's a duplicate
				// do nothing because it's a duplicate
			}
			else {
				myShape newShape(approxShape); // create te new shape given the vertices
				myShapes.push_back(newShape); // add to shape list
				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); 
				drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());	// draws it... not really needed other then to show it worked			
			}			
	}

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
}