#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

// x value sort parameter function
bool pointSortX(const Point& a, const Point& b) {
	return (a.x < b.x);
}

// y value sort parameter function 
bool pointSortY(const Point& a, const Point& b) {
	return (a.y < b.y);
}

/*
*@points the vertices of the shape
*@size the number of vertices in the shape
*@type the type of div, default to 'd' (changed later)
*/
struct myShape {
	vector<Point> points;
	size_t size;
	char type;
	myShape(vector<Point> points1) {
		this->points = points1;
		this->size = points1.size();
		this->type = 'd'; // d for default
	}

	//function used to sort the vertices first by x values, then by y values
	void pointSort() {
		std::sort(points.begin(), points.end(), pointSortX);
		std::sort(points.begin(), points.end(), pointSortY);
	}
};
vector<myShape> myShapes; // vector to hold shapes
vector<myShape> deletedShapes; // second vector hold deleted shapes, so you can undo the delete if you want

//check to make sure there are no duplicates in the shapesvector.
// returns true if it's a duplicate, false otherwise
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

// checks for the shape type, meaning it detects if there is a shape
// inside another shape, and if true, changes the type of the outside shape
// to an image or text div
void checkShapeType() {
	double xrange1, xrange2, yrange1, yrange2;
	int count = 0;
	for (int i = 0; i < myShapes.size(); i++) {
		if (myShapes[i].type == 'r') {
			xrange1 = myShapes[i].points[0].x;
			xrange2 = myShapes[i].points[3].x;
			yrange1 = myShapes[i].points[0].y;
			yrange2 = myShapes[i].points[3].y;
			for (int j = 0; j < myShapes.size(); j++) {
				if (myShapes[j].type == 't') {
					for (int y = 0; y < 3; y++) {
						if (myShapes[j].points[y].x < xrange2 && myShapes[j].points[y].x > xrange1 && myShapes[j].points[y].y < yrange2 && myShapes[j].points[y].y > yrange1)
							count += 1;
					}
					if (count == 3) {
						myShapes[i].type = 'T'; // big T indicating its a text object
						count = 0; // reset variables
						myShapes.erase(myShapes.begin() + j); // erase useless shape
						i -= 1; // reset i since myShapes is now smaller in size
						j -= 1; // reset j since myShapes is now smaller in size
						break; // break out of for loop
					}
				}
				else if (myShapes[j].type == 'c') {
					for (int y = 0; y < myShapes[j].size; y++) {
						if (myShapes[j].points[y].x < xrange2 && myShapes[j].points[y].x > xrange1 && myShapes[j].points[y].y < yrange2 && myShapes[j].points[y].y > yrange1)
							count += 1;
					}
					if (count == myShapes[j].size) {
						myShapes[i].type = 'I'; // big I indicating its an image
						count = 0; // reset variables
						myShapes.erase(myShapes.begin() + j); // erase useless shape
						i -= 1; // reset i since myShapes is now smaller in size
						j -= 1; // reset j since myShapes is now smaller in size
						break; // break out of for loop
					}
				}

			}

		}
	}
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
	Canny(src_gray, canny_output, 0, 50, 5);
	/// Find contours	
	findContours(canny_output, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
	/// Draw contours
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{		
		approxPolyDP(Mat(contours[i]), approxShape, arcLength(Mat(contours[i]), true)*0.02, true);
		if (checkShapes(approxShape)) {
			// do nothing because it's a duplicate
		}
		else {
			myShape newShape(approxShape);
			newShape.pointSort();
			if (approxShape.size() == 3)
				newShape.type = 't'; // t for triangle
			else if (approxShape.size() == 4)
				newShape.type = 'r'; // r for rectangle
			else {// it's a circle
				newShape.type = 'c'; // c for circle				
			}
			myShapes.push_back(newShape);
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}
	}

	checkShapeType();

	/// Show in a window
	namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	imshow("Contours", drawing);
}
