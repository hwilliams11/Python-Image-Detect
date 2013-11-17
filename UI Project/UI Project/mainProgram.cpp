#include "Div.hpp"
#include "myShape.hpp"
#include <fstream>
using namespace cv;
using namespace std;

int divId = 0;
RNG rng(12345);
const int MAX_PAGE_WIDTH = 700;
vector<myShape> myShapes; // vector to hold shapes
vector<myShape> deletedShapes; // second vector hold deleted shapes, so you can undo the delete if you want
string dummyText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse volutpat ipsum sed feugiat facilisis. In auctor nulla vitae velit cursus, volutpat vulputate massa volutpat. Nullam ornare, purus et consectetur suscipit, felis velit lacinia nulla, vitae ornare felis erat vel augue. Donec sed tempor urna. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Cras interdum justo et faucibus malesuada. Ut ut eros magna.\
		Pellentesque ac nibh ut est porttitor lobortis. In nec justo sed metus convallis aliquet faucibus eu ante. Aenean tristique, felis quis lobortis volutpat, neque enim bibendum orci, eu fermentum nunc ligula nec est. In at sagittis felis. Cras sit amet justo ac orci viverra porta. Maecenas lobortis semper dolor quis tempus. Duis in tincidunt lectus. Donec vitae orci dolor. Aliquam pellentesque, mauris id iaculis suscipit, mi lacus elementum nisl, vitae feugiat velit lectus vitae nibh. Interdum et malesuada fames ac ante ipsum primis in faucibus. Proin hendrerit diam et eleifend laoreet. Nam turpis sapien, sollicitudin non consectetur eu, ornare at justo. Quisque arcu enim, sollicitudin vitae tortor nec, eleifend ornare turpis. Vestibulum ultricies tempor nunc at gravida. Vestibulum non dictum odio.";
string dummy_image = "Sample Pictures/Desert.jpg";

//non member functions 
//check to make sure there are no duplicates in the shapesvector.
// returns true if it's a duplicate, false otherwise
bool checkShapes(vector<Point> shapeVector) {
	int count = 0;
	size_t shapeSize = shapeVector.size();
	myShape newShape(shapeVector);
	newShape.pointSortX();
	shapeVector = newShape.points;

	double xrange1, yrange1, xrange2, yrange2,msxrange1,msxrange2,msyrange1,msyrange2;
	
	for (int y = 0; y < myShapes.size(); y++) {
		if (myShapes[y].size == shapeSize) {
			size_t size = myShapes.size();
			count = 0;
			for (int i = 0; i < shapeSize; i++) {

				xrange1 = shapeVector[i].x * .80;
				xrange2 = shapeVector[i].x * 1.20;
				yrange1 = shapeVector[i].y * .80;
				yrange2 = shapeVector[i].y * 1.20;
			
				msxrange1 = myShapes[y].points[i].x * .80;
				msxrange2 = myShapes[y].points[i].x * 1.20;
				msyrange1 = myShapes[y].points[i].y * .80;
				msyrange2 = myShapes[y].points[i].y * 1.20;

				bool myShapeInNewShape=false;
				bool newShapeInMyShape=false;
				/*
				if( myShapes[y].points[i].x <= xrange2 && myShapes[y].points[i].x >= xrange1 && myShapes[y].points[i].y <= yrange2 && myShapes[y].points[i].y >= yrange1 ){
				myShapeInNewShape = true;
				}
				if( shapeVector[i].x <= msxrange2 && shapeVector[i].x >= msxrange1 && shapeVector[i].y <= msyrange2 && shapeVector[i].y >= msyrange1 ){
				newShapeInMyShape = true;
				}
				if ( myShapeInNewShape || newShapeInMyShape ) {
					count += 1;
				}*/
				if( ( abs( myShapes[y].points[i].x-shapeVector[i].x ) < 50 ) && ( abs( myShapes[y].points[i].y-shapeVector[i].y ) < 50 ) ){
					count+=1;
				}
			}
			if( count == shapeSize )
			return true;
		}
	}
	return false;
}
void checkShapeType() {
        double xrange1, xrange2, yrange1, yrange2;
        int count = 0;
        for (int i = 0; i < myShapes.size(); i++) {
                if (myShapes[i].type == 'r') {
                        myShapes[i].pointSortX();
                        xrange1 = myShapes[i].points[0].x;
                        xrange2 = myShapes[i].points[3].x;
                        myShapes[i].pointSortY();
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
                                else if (myShapes[j].type == 'c' && myShapes[j].size>10 ) {
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
void writeHTML(string html){


	ofstream html_file("output.html");
	
	if( html_file.is_open() ){
		html_file << html << endl;
	}
	html_file.close();
}
string createHead(){

	string head = "<html>\n";
	head += "\t<head>\n";
	head += "<style type=\"text/css\">\n";
	head += "\tbody{\n";
	head += "\t\tmargin:auto;\n";
	head += "\t\twidth:"+std::to_string(MAX_PAGE_WIDTH)+"px;\n";
	head += "\t}\n";
	head += "\ttable{\n";
	head += "\t\tmargin: 0 auto;\n";
	head += "\t}\n";
	head += "\timg{\n";
	head += "\t\tdisplay:block;\n";
	head += "\t\tmargin: 0 auto;\n";
	head += "\t}\n</style>\n";
	head += "\t</head>\n";
	head += "\t<body>\n";
	return head;
}
string createEnd(){

	string end = "\t</body>\n";
	end += "<html>\n";
	return end;
}
string createBody(Div div,int divOrientation,string body,int tabLevel){

	string tabs;
	char info[200];
	for(int i=0;i<tabLevel;i++)
		tabs+="\t";
				
	if( div.children.size()>0 ){
		if( divOrientation == Div::HORIZONTAL ){
			sprintf(info,"id=\"%d\" style=\"float:left;width:%dpx;\"",div.id,div.width);
		}else{
			sprintf(info,"id=\"%d\" style=\"width:%dpx;\"",div.id,div.width);
		}
		body += tabs+"<div "+string(info)+">\n";
		for( int i=0; i < div.children.size(); i++ ){
			body = createBody(div.children[i],div.divOrientation,body,tabLevel+1);
		}
		body += tabs+"</div><!-- end"+std::to_string(div.id)+"!-->\n";
		return body;
	}
	else if( div.children.size() == 0 ){
		
		if( div.divContent == Div::TEXT ){
			if( divOrientation == Div::HORIZONTAL ){
				sprintf(info,"id=\"%d\" style=\"float:left;width:%dpx;\"",div.id,div.width);
			}else{
				sprintf(info,"id=\"%d\" style=\"width:%dpx;\"",div.id,div.width);
			}
			body += tabs+"<div "+string(info)+">\n";
			body += tabs+"\t<p>\n"+tabs+"\t\t"+dummyText+"\n"+tabs+"\t</p>\n";
			body += tabs+"</div><!-- end"+std::to_string(div.id)+"!-->\n";
			return body;
		}
		if( div.divContent == Div::IMAGE ){
			if( divOrientation == Div::HORIZONTAL ){
				body += tabs+"<div style=\"float:left\">\n";
			}else{
				body += tabs+"<div>\n";
			}
			sprintf(info,"id=\"%d\" style=\"width:%dpx;height:400px;display:table-cell;vertical-align:middle;\"",div.id,div.width);
			body += tabs+"\t<div "+string(info)+">\n";
			body += tabs+"\t\t<img src=\""+dummy_image+"\"/>\n";
			body += tabs+"\t</div><!-- end"+std::to_string(div.id)+"!-->\n";
			body += tabs+"</div>\n";
			return body;
		}
		if( div.divContent == Div::TABLE ){
			if( divOrientation == Div::HORIZONTAL ){
				body += tabs+"<div style=\"float:left\">\n";
			}else{
				body += tabs+"<div>\n";
			}
			sprintf(info,"id=\"%d\" style=\"width:%dpx;height:400px;display:table-cell;vertical-align:middle;\"",div.id,div.width);
			body += tabs+"\t<div "+string(info)+">\n";
			body += tabs+"\t\t<table>\n";
				body += tabs+"\t\t\t<tr>\n";
					body += tabs+"\t\t\t<td>1</td>\n";
					body += tabs+"\t\t\t<td>2</td>\n";
					body += tabs+"\t\t\t<td>3</td>\n";
				body += tabs+"\t\t\t</tr>\n";
				body += tabs+"\t\t\t<tr>\n";
					body += tabs+"\t\t\t<td>4</td>\n";
					body += tabs+"\t\t\t<td>5</td>\n";
					body += tabs+"\t\t\t<td>6</td>\n";
				body += tabs+"\t\t\t</tr>\n";
				body += tabs+"\t\t\t<tr>\n";
					body += tabs+"\t\t\t<td>7</td>\n";
					body += tabs+"\t\t\t<td>8</td>\n";
					body += tabs+"\t\t\t<td>9</td>\n";
				body += tabs+"\t\t\t</tr>\n";
				body += tabs+"\t\t\t<tr>\n";
					body += tabs+"\t\t\t<td>10</td>\n";
					body += tabs+"\t\t\t<td>11</td>\n";
					body += tabs+"\t\t\t<td>12</td>\n";
				body += tabs+"\t\t\t</tr>\n";
			body += tabs+"\t\t</table>\n";
			body += tabs+"\t</div><!-- end"+std::to_string(div.id)+"!-->\n";
			body += tabs+"</div>\n";
			return body;
		}
	}
	
}
string createHTML(Div mainDiv){

	string head = createHead();
	string end = createEnd();
	string body = "";
	body = createBody(mainDiv,mainDiv.divOrientation,body,2);

	string html = head + body + end;

	return html;
}
void resizeAllDivs(Div &div,int width){

	cout << "Width: "<<width<<endl;
	div.width = width;
	if( div.children.size() > 0 ){

		if( div.divOrientation == Div::VERTICAL ){

			for(int i = 0; i < div.children.size(); i++){

				resizeAllDivs(div.children[i],width);
			}
		}
		else{
			size_t childWidth = width/div.children.size();
			for (int i = 0; i < div.children.size(); i++){

				resizeAllDivs(div.children[i],childWidth);
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

	int minRow = -1, minCol = -1, maxCol = -1, maxRow = -1, maxW, maxH;
	Div::DivOrientation divOrientation;

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
		divOrientation = Div::HORIZONTAL;
	}
	else{
		sort(divList.begin(), divList.end(), compareDivsV);
		divOrientation = Div::VERTICAL;
	}
	maxW = maxCol - minCol;
	maxH = maxRow - minRow;

	cout << "min row: " << minRow << endl;
	cout << "max row: " << maxRow << endl;
	cout << "min col: " << minCol << endl;
	cout << "max col: " << maxRow << endl;
	cout << " maxW: " << maxW << endl;
	cout << " maxH: " << maxH << endl;
	Div div = Div(minRow, minCol, maxW, maxH, divList, divOrientation, divId);
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

			if (abs(div1.row - div2.row) < 80 &&
				abs(div1.height - div2.height)<80){
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

			if (abs(div1.col - div2.col) < 80 &&
				abs(div1.width - div2.width)<80){
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

	if( count == 10 )
		return divs[0];

	if (divs.size() > 1){
		for (unsigned int i = 0; i<divs.size(); i++){
			divs = findHorizontalMatch(i, divs);
			cout << "Divs after horizontal matching " << i <<" size: "<<divs.size() << endl;
			printDivs(divs);
		}
	}
	if (divs.size()>1){
		for (unsigned int i = 0; i<divs.size(); i++){
			divs = findVerticalMatch(i, divs);
			cout << "Divs after vertical matching " << i <<" size: "<<divs.size() << endl;
			printDivs(divs);
		}
	}

	if (divs.size() > 1)
		return setupHierarchy(divs, count + 1);

	else{
		return divs[0];
	}
}
vector<Div> removeDupDivs(vector<Div> divs){

	bool matched;
	vector<Div> nonDup;

	for(unsigned int i=0;i<divs.size();i++){
		matched=false;
		for(unsigned int j=0;j<nonDup.size();j++){

			if( !divs[i].match(nonDup[j]) ){

				if( divs[i].similar(nonDup[j])){

					matched = true;
				}
			}
		}
		if( !matched )
			nonDup.push_back(Div(divs[i]));
	}
	return nonDup;
}
//remove duplicates and setup hierarchy
Div setUpDivs(){
	vector<Div> tempDiv;
	Div mainDiv;

	if( myShapes.size() > 0 ){
		for (int i = 0; i < myShapes.size(); i++) {
			if( myShapes[i].type=='r' || myShapes[i].type=='T' || myShapes[i].type=='I'){
				cout <<"i: "<<i<<endl;
				Div newDiv(&myShapes[i]);
				newDiv.setId(divId);
				divId += 1;
				tempDiv.push_back(newDiv);
			}
		}
		tempDiv = removeDupDivs(tempDiv);
		printDivs(tempDiv);
		cout<<"setup hierarchy"<<endl;
		mainDiv = setupHierarchy(tempDiv, 0);
	}
	return mainDiv;
}

/** @function thresh_callback */
void doWork(Mat &img,Mat &src_gray, int, void*)
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
		approxPolyDP(Mat(contours[i]), approxShape, 0.01*arcLength(Mat(contours[i]), true), true);
		if (checkShapes(approxShape)) {
			// do nothing because it's a duplicate
		}
		else {
			Scalar color;
			myShape newShape(approxShape);
			newShape.pointSortX();
			if (approxShape.size() == 3){
				newShape.type = 't'; // t for triangle
				color = Scalar(0,255,0);
			}
			else if (approxShape.size() == 4){
				newShape.type = 'r'; // r for rectangle
				color = Scalar(255,0,0);
			}
			else {// it's a circle
				newShape.type = 'c'; // c for circle
				color = Scalar(0,0,255);
			}
			myShapes.push_back(newShape);
			drawContours(img, contours, i, color, 2);
		}
	}
	cout << "Checking shape type"<<endl;
	checkShapeType();
}
/** @function thresh_callback */
void doWork2(Mat &img,Mat &src_gray, int, void*)
{
	Mat threshImage;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Point> approxShape;

	double ret = threshold(src_gray,threshImage,127,255,1);
	findContours(threshImage,contours,1,2);

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], approxShape, 0.01*arcLength(contours[i], true), true);
		Rect rect = boundingRect( contours[i] );

		if( rect.width <20 || rect.height < 20 ){
			continue;
		}
		if (checkShapes(approxShape)) {
			// do nothing because it's a duplicate
		}
		else {
			Scalar color;
			myShape newShape(approxShape);
			newShape.pointSortX();
			if (approxShape.size() == 3){
				newShape.type = 't'; // t for triangle
				color = Scalar(0,255,0);
			}
			else if (approxShape.size() == 4){
				newShape.type = 'r'; // r for rectangle
				color = Scalar(255,0,0);
			}
			else {// it's a circle
				newShape.type = 'c'; // c for circle
				color = Scalar(0,0,255);
			}
			myShapes.push_back(newShape);
			drawContours(img, contours, i, color, 2);
		}
	}
	cout << "Checking shape type"<<endl;
	checkShapeType();
}
int main(int argc, char** argv) {

	cout << "Choose an image file"<<endl;
	cout << " - 1 for rects2.png"<<endl;
	cout << " - 2 for shapes6.png"<<endl;
	cout << " - 3 for test.png'"<<endl;
	cout << " - 4 for test2.png'"<<endl;
	cout << " - 5 for htmlPhoto1.png'"<<endl;
	cout << " - 6 for shapes7.png'"<<endl;
	cout << " - 7 for shapes8.png'"<<endl;

	int choice = getchar()-'0';

	string file = "HTML Drawings/";

	switch( choice ){

		case 1:{file+="rects2.png";break;}
		case 2:{file+="shapes6.png";break;}
		case 3:{file+="test.png";break;}
		case 4:{file+="test2.png";break;}
		case 5:{file+="htmlPhoto1.png";break;}
		case 6:{file+="shapes7.png";break;}
		case 7:{file+="shapes8.png";break;}
		default:{file+="test.png";break;}
	}
	
	Mat img, gray;
	cout << "Read image" << endl;
	readImage(file, &img, &gray);
	cout << "Do work" << endl;
	doWork2(img,gray, 0, 0);
	cout << "Setup divs" << endl;
	Div mainDiv = setUpDivs();
	imshow("Main image", img);
	cout << "printing hierarchy" << mainDiv.children.size() << endl;
	resizeAllDivs(mainDiv,MAX_PAGE_WIDTH);
	mainDiv.printDivWithChildren(0);
	string html = createHTML(mainDiv);
	writeHTML(html);
	waitKey(0);
	return 0;
}