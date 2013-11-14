#include "myShape.hpp"

myShape::myShape(vector<Point> points1) {
	points = points1;
	size = points1.size();
	type = 'd'; // d for default
}

void myShape::pointSort() {
	std::sort(points.begin(), points.end(), pointSortX());
	std::sort(points.begin(), points.end(), pointSortY());
}
