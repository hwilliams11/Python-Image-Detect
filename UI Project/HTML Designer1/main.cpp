#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;


int divId = 0;
RNG rng(12345);
const int HORIZONTAL = 0;
const int VERTICAL = 1;

struct Div {

	int row;
	int col;
	int id;
	int width;
	int height;
	int divType;
	vector<Div> children;
	Div(){
	}
	Div(int row,int col,int width,int height,vector<Div>children,int divType){
		this->row = row;
		this->col = col;
		this->width = width;
		this->height = height;
		this->divType = divType;
		this->children = vector<Div>(children);
		setId(this);
	}
	Div(Div *div){

		this->row = div->row;
		this->col = div->col;
		this->width = div->width;
		this->height = div->height;
		this->divType = div->divType;
		this->children = div->children;
		
		setId(this);
	}
	void setId(Div *div){
		div->id = divId;
		divId += 1;
	}
	string printDiv(){
		
		char buffer[100];
		sprintf(buffer,"(%d,%d) %d x %d",row,col,width,height);
		return string(buffer);
	}
	bool match(Div div2 ){
		if( this->row == div2.row && this->col == div2.col &&
			this->width == div2.width && this->height == div2.height ){
				return true;
		}
		return false;
	}
	bool similar(Div div2 ){
		if( abs( this->row - div2.row ) < 10 &&
			abs( this->col - div2.col ) < 10 &&
			abs( this->width - div2.width) < 20 &&
			abs( this->height - div2.height) < 20 ){
				return true;
		}
		return false;
	}
	void printDivWithChildren(int level){
		
		string indent = "";
		for(int i=0;i<5*level;i++)
			indent += " ";
		cout << indent + this->printDiv() << endl;
		cout << this->children.size() << endl;
		for(int i=0;i<this->children.size();i++){
			this->children[i].printDivWithChildren(level+1);
		}
	}
};
void printDivs( vector<Div> divs ){

	for(unsigned int i=0;i<divs.size();i++)
			cout << divs[i].printDiv() << endl;

}
//read in image file
void readImage(string imageName, Mat *img,Mat *gray){

	*img = imread(imageName);
	cvtColor(*img,*gray,CV_BGR2GRAY);
}
//locate rectangles drawn
vector<Div> findRectangles(Mat image,Mat gray){
	Mat threshImage;
	vector<vector<Point> > contours;
	Point p = Point(0,0);
	vector<Div> rectangles;

	double ret = threshold(gray,threshImage,127,255,1);
	findContours(threshImage,contours,1,2);

	for(unsigned int i=0;i<contours.size();i++){

		vector<Point> approx;
		vector<Point> contour = contours[i];
		approxPolyDP(contour,approx,0.01*arcLength(contour,true),true);

		if( approx.size() == 4 ){

			Rect rect = boundingRect(contour);
			int row = rect.y;
			int col = rect.x;
			int w = rect.width;
			int h = rect.height;

			if( rect.width * rect.height > 20 ){

				InputArrayOfArrays ac = contour;
				//printf("square: (%d, %d) %d x %d\n",row,col,w,h);
				rectangle(image,Point(col,row),Point(col+w,row+h),Scalar(0,255,0),2);
				drawContours(image,contours,i,Scalar(0,0,255),2);
				Div div = Div(row,col,w,h,(vector<Div>)NULL,-1);
				rectangles.push_back( div );
			}

		}
	}
	return rectangles;
}
bool compareDivsH(const Div &a, const Div &b){
	return a.col < b.col;
}
bool compareDivsV(const Div &a, const Div &b){
	return a.row < b.row;
}
Div createNewDiv(vector<Div> divList,bool horizontal){

	int minRow=-1,minCol=-1,maxCol=-1,maxRow=-1,maxW,maxH,divType;

	cout << "Creating new div from these divs "<<endl;
	printDivs(divList);

	for(int i=0;i<divList.size();i++){

		Div d = divList[i];
		if( minRow == -1 || d.row < minRow ){
			minRow = d.row;
		}
		if( maxRow == -1 || (d.row+d.height) > maxRow ){
			maxRow = d.row+d.height;
		}
		if( minCol == -1 || d.col < minCol ){
			minCol = d.col;
		}
		if( maxCol == -1 || (d.col+d.width) > maxCol ){
			maxCol = d.col+d.width;
		}
	}
	if( horizontal ){
		sort(divList.begin(),divList.end(),compareDivsH);
		divType = HORIZONTAL;
	}else{
		sort(divList.begin(),divList.end(),compareDivsV);
		divType = VERTICAL;
	}
	maxW = maxCol - minCol;
	maxH = maxRow - minRow;
	
	cout << "min row: "<<minRow<<endl;
	cout << "max row: "<<maxRow<<endl;
	cout << "min col: "<<minCol<<endl;
	cout << "max col: "<<maxRow<<endl;
	cout <<" maxW: "<<maxW<<endl;
	cout <<" maxH: "<<maxH<<endl;
	Div div = Div(minRow,minCol,maxW,maxH,divList,divType);
	cout <<"Created this div "<<div.printDiv()<<endl;
	return div;
}
vector<Div> findHorizontalMatch(int divIndex,vector<Div>divs){

	Div div1 = divs[divIndex];
	vector<int> divListIndex;
	divListIndex.push_back(divIndex);
	
	for(unsigned int i=0;i<divs.size();i++){
		
		Div div2 = divs[i];
		if( !div1.match(div2) ){

			if( abs(div1.row-div2.row)<100 && 
				abs(div1.height-div2.height)<100 ){
					divListIndex.push_back(i);
			}
		}
	}
	
	vector<Div> newDivs;
	
	if( divListIndex.size() > 1 ){
		vector<Div>divList;
		for(unsigned int i=0;i<divListIndex.size();i++){
			divList.push_back( divs[divListIndex[i]] );
		}
		
		Div div = createNewDiv(divList,true);
		
		for(unsigned int i=0;i<divs.size();i++){
			bool match = false;
			for(unsigned int j=0;j<divListIndex.size();j++){
				if( divs[i].match(divs[divListIndex[j]]) ){
					match=true;
				}
			}
			if(!match)
				newDivs.push_back(divs[i]);
		}
		newDivs.push_back(div);
		return newDivs;
	}else{
		//printDivs(divs);
		return divs;
	}
	
}
vector<Div> findVerticalMatch(int divIndex,vector<Div>divs){

	Div div1 = divs[divIndex];
	vector<int> divListIndex;
	divListIndex.push_back(divIndex);
	
	for(unsigned int i=0;i<divs.size();i++){
		
		Div div2 = divs[i];
		if( !div1.match(div2) ){

			if( abs(div1.col-div2.col)<100 && 
				abs(div1.width-div2.width)<100 ){
					divListIndex.push_back(i);
			}
		}
	}
	
	vector<Div> newDivs;
	
	if( divListIndex.size() > 1 ){
		vector<Div>divList;
		for(unsigned int i=0;i<divListIndex.size();i++){
			divList.push_back( divs[divListIndex[i]] );
		}
		
		Div div = createNewDiv(divList,false);
		
		printDivs(divs);
		for(unsigned int i=0;i<divs.size();i++){
			bool match = false;
			for(unsigned int j=0;j<divListIndex.size();j++){
				if( divs[i].match(divs[divListIndex[j]]) ){
					match=true;
				}
			}
			if(!match){
				newDivs.push_back(divs[i]);
			}
		}
		newDivs.push_back(div);
		return newDivs;
	}else{
		//printDivs(divs);
		return divs;
	}
	
}
Div setupHierarchy(vector<Div> divs,int count){

	if( divs.size()>1){
		for(unsigned int i=0;i<divs.size();i++){
			divs = findHorizontalMatch(i,divs);
			//cout << "Divs after horizontal matching " << i <<" size: "<<divs.size() << endl;
			//printDivs(divs);
		}
	}
	if( divs.size()>1 ){
		for(unsigned int i=0;i<divs.size();i++){
			divs = findVerticalMatch(i,divs);
			//cout << "Divs after vertical matching " << i <<" size: "<<divs.size() << endl;
			//printDivs(divs);
		}
	}

	if( divs.size() > 1 )
		return setupHierarchy(divs,count+1);

	else{
		return divs[0];
	}
}
vector<Div> removeDupRectangles(vector<Div> rects){

	bool matched;
	vector<Div> nonDup;

	for(unsigned int i=0;i<rects.size();i++){
		matched=false;
		for(unsigned int j=0;j<nonDup.size();j++){

			if( !rects[i].match(nonDup[j]) ){

				if( rects[i].similar(nonDup[j])){

					matched = true;
				}
			}
		}
		if( !matched )
			nonDup.push_back(Div(rects[i]));
	}
	return nonDup;
}
//remove duplicates and setup hierarchy
Div setupRectangles(vector<Div> rects){

	cout << "Before" <<endl;
	printDivs(rects);
	vector<Div> nonDup = removeDupRectangles(rects);
	cout << "After" <<endl;
	printDivs(nonDup);
	Div mainDiv = setupHierarchy(nonDup,0);
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
int main(){

	Mat img;
	Mat gray;
	string imageName;
	ifstream imNameFile;

	imNameFile.open("imageName.txt");
	imNameFile >> imageName;
	imNameFile.close();
	
	readImage(imageName,&img,&gray);
	vector<Div> rects = findRectangles(img,gray);
	Div mainDiv = setupRectangles(rects);
	imshow("Main image",img);

	cout << "printing hierarchy"<<mainDiv.children.size()<<endl;
	mainDiv.printDivWithChildren(0);
	
	waitKey(0);
	return 0;
}