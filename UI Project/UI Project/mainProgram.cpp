#include "Div.hpp"
#include "myShape.hpp"

using namespace cv;
using namespace std;

int divId = 0;
RNG rng(12345);
const int HORIZONTAL = 0;
const int VERTICAL = 1;
vector<myShape> myShapes; // vector to hold shapes
vector<myShape> deletedShapes; // second vector hold deleted shapes, so you can undo the delete if you want

//non member functions 
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

void printDivs(vector<Div> divs){
	for (unsigned int i = 0; i < divs.size(); i++)
		cout << divs[i].printDiv() << endl;
}
//read in image file
void readImage(string imageName, Mat *img, Mat *gray){

	*img = imread(imageName);
	cvtColor(*img, *gray, CV_BGR2GRAY);
	blur(*gray, *gray, Size(3, 3));
}

bool compareDivsH(const Div &a, const Div &b){
	return a.col < b.col;
}
bool compareDivsV(const Div &a, const Div &b){
	return a.row < b.row;
}
Div createNewDiv(vector<Div> divList, bool horizontal){

	int minRow = -1, minCol = -1, maxCol = -1, maxRow = -1, maxW, maxH, divType;

	cout << "Creating new div from these divs " << endl;
	printDivs(divList);

	for (int i = 0; i < divList.size(); i++){

		Div d = divList[i];
		if (minRow == -1 || d.row < minRow){
			minRow = d.row;
		}
		if (maxRow == -1 || (d.row + d.height) > maxRow){
			maxRow = d.row + d.height;
		}
		if (minCol == -1 || d.col < minCol){
			minCol = d.col;
		}
		if (maxCol == -1 || (d.col + d.width) > maxCol){
			maxCol = d.col + d.width;
		}
	}
	if (horizontal){
		sort(divList.begin(), divList.end(), compareDivsH);
		divType = HORIZONTAL;
	}
	else{
		sort(divList.begin(), divList.end(), compareDivsV);
		divType = VERTICAL;
	}
	maxW = maxCol - minCol;
	maxH = maxRow - minRow;

	cout << "min row: " << minRow << endl;
	cout << "max row: " << maxRow << endl;
	cout << "min col: " << minCol << endl;
	cout << "max col: " << maxRow << endl;
	cout << " maxW: " << maxW << endl;
	cout << " maxH: " << maxH << endl;
	Div div = Div(minRow, minCol, maxW, maxH, divList, divType, divId);
	divId += 1;
	cout << "Created this div " << div.printDiv() << endl;
	return div;
}
vector<Div> findHorizontalMatch(int divIndex, vector<Div>divs){

	Div div1 = divs[divIndex];
	vector<int> divListIndex;
	divListIndex.push_back(divIndex);

	for (unsigned int i = 0; i < divs.size(); i++){

		Div div2 = divs[i];
		if (!div1.match(div2)){

			if (abs(div1.row - div2.row) < 100 &&
				abs(div1.height - div2.height)<100){
				divListIndex.push_back(i);
			}
		}
	}

	vector<Div> newDivs;

	if (divListIndex.size() > 1){
		vector<Div>divList;
		for (unsigned int i = 0; i < divListIndex.size(); i++){
			divList.push_back(divs[divListIndex[i]]);
		}

		Div div = createNewDiv(divList, true);

		for (unsigned int i = 0; i < divs.size(); i++){
			bool match = false;
			for (unsigned int j = 0; j < divListIndex.size(); j++){
				if (divs[i].match(divs[divListIndex[j]])){
					match = true;
				}
			}
			if (!match)
				newDivs.push_back(divs[i]);
		}
		newDivs.push_back(div);
		return newDivs;
	}
	else{
		//printDivs(divs);
		return divs;
	}

}
vector<Div> findVerticalMatch(int divIndex, vector<Div>divs){

	Div div1 = divs[divIndex];
	vector<int> divListIndex;
	divListIndex.push_back(divIndex);

	for (unsigned int i = 0; i < divs.size(); i++){

		Div div2 = divs[i];
		if (!div1.match(div2)){

			if (abs(div1.col - div2.col) < 100 &&
				abs(div1.width - div2.width)<100){
				divListIndex.push_back(i);
			}
		}
	}

	vector<Div> newDivs;

	if (divListIndex.size() > 1){
		vector<Div>divList;
		for (unsigned int i = 0; i < divListIndex.size(); i++){
			divList.push_back(divs[divListIndex[i]]);
		}

		Div div = createNewDiv(divList, false);

		printDivs(divs);
		for (unsigned int i = 0; i < divs.size(); i++){
			bool match = false;
			for (unsigned int j = 0; j < divListIndex.size(); j++){
				if (divs[i].match(divs[divListIndex[j]])){
					match = true;
				}
			}
			if (!match){
				newDivs.push_back(divs[i]);
			}
		}
		newDivs.push_back(div);
		return newDivs;
	}
	else{
		//printDivs(divs);
		return divs;
	}

}
Div setupHierarchy(vector<Div> divs, int count){

	if (divs.size() > 1){
		for (unsigned int i = 0; i<divs.size(); i++){
			divs = findHorizontalMatch(i, divs);
			//cout << "Divs after horizontal matching " << i <<" size: "<<divs.size() << endl;
			//printDivs(divs);
		}
	}
	if (divs.size()>1){
		for (unsigned int i = 0; i<divs.size(); i++){
			divs = findVerticalMatch(i, divs);
			//cout << "Divs after vertical matching " << i <<" size: "<<divs.size() << endl;
			//printDivs(divs);
		}
	}

	if (divs.size() > 1)
		return setupHierarchy(divs, count + 1);

	else{
		return divs[0];
	}
}

//remove duplicates and setup hierarchy
Div setUpDivs(){
	vector<Div> tempDiv;
	for (int i = 0; i < myShapes.size(); i++) {
		Div newDiv(&myShapes[i]);
		newDiv.setId(divId);
		divId += 1;
		tempDiv.push_back(newDiv);
	}
	printDivs(tempDiv);
	Div mainDiv = setupHierarchy(tempDiv, 0);
	return mainDiv;
}

//resize divs to fit in webpage
void resizeAllDivs(){
}
//create the HTML for the page
void createHTML(){
}
//write the HTML to file
void writeHTML(){
}

/** @function thresh_callback */
void doWork(Mat &src_gray, int, void*)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> approxShape;

	/// Detect edges using canny
	Canny(src_gray, canny_output, 100, 200, 3);
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
			newShape.pointSortX();
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
}


int main(int argc, char** argv) {
	string file = "C:/Users/Nicholas/Desktop/test1.png";
	Mat img, gray;
	readImage(file, &img, &gray);
	doWork(gray, 0, 0);
	Div mainDiv = setUpDivs();
	imshow("Main image", img);
	cout << "printing hierarchy" << mainDiv.children.size() << endl;
	mainDiv.printDivWithChildren(0);
	waitKey(0);
	return 0;
}