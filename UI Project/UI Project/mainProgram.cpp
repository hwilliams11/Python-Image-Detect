#include "Div.hpp"
#include "myShape.hpp"
#include <fstream>
#include <time.h>
using namespace cv;
using namespace std;

int divId = 0;
const int MAX_PAGE_WIDTH = 700;
const int MAX_PAGE_HEIGHT = 700;
vector<myShape> myShapes; // vector to hold shapes
vector<myShape> deletedShapes; // second vector hold deleted shapes, so you can undo the delete if you want
string dummyText = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse volutpat ipsum sed feugiat facilisis. In auctor nulla vitae velit cursus, volutpat vulputate massa volutpat. Nullam ornare, purus et consectetur suscipit, felis velit lacinia nulla, vitae ornare felis erat vel augue. Donec sed tempor urna. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Cras interdum justo et faucibus malesuada. Ut ut eros magna.\
		Pellentesque ac nibh ut est porttitor lobortis. In nec justo sed metus convallis aliquet faucibus eu ante. Aenean tristique, felis quis lobortis volutpat, neque enim bibendum orci, eu fermentum nunc ligula nec est. In at sagittis felis. Cras sit amet justo ac orci viverra porta. Maecenas lobortis semper dolor quis tempus. Duis in tincidunt lectus. Donec vitae orci dolor. Aliquam pellentesque, mauris id iaculis suscipit, mi lacus elementum nisl, vitae feugiat velit lectus vitae nibh. Interdum et malesuada fames ac ante ipsum primis in faucibus. Proin hendrerit diam et eleifend laoreet. Nam turpis sapien, sollicitudin non consectetur eu, ornare at justo. Quisque arcu enim, sollicitudin vitae tortor nec, eleifend ornare turpis. Vestibulum ultricies tempor nunc at gravida. Vestibulum non dictum odio.";

string dummy_image = "Sample Pictures/Desert.jpg";
const int PADDING=5;
int drawnDivWidth = 0;
int divComparison = 0;


struct Website{
	string name;
	string address;

	Website(string name,string address){
		this->name = name;
		this->address = address;
	}
};
vector<Website> websites;

//non member functions 
//check to make sure there are no duplicates in the shapesvector.
// returns true if it's a duplicate, false otherwise
bool checkShapes(vector<Point> shapeVector) {
	int count = 0;
	size_t shapeSize = shapeVector.size();
	myShape newShape(shapeVector);
	newShape.pointSortX();
	shapeVector = newShape.points;
	double x0 = 0, x1 = 0, y0 = 0, y1 = 0, X0 = 0, X1 = 0, Y0 = 0, Y1 = 0;

	for (int y = 0; y < myShapes.size(); y++) {
		if (myShapes[y].size == shapeSize) {
			size_t size = myShapes.size();
			count = 0;
			myShapes[y].pointSortX();
			x0 = myShapes[y].points[0].x;
			x1 = myShapes[y].points.back().x;
			X0 = newShape.points[0].x;
			X1 = newShape.points.back().x;
			newShape.pointSortY();
			myShapes[y].pointSortY();
			Y0 = newShape.points[0].y;
			Y1 = newShape.points.back().y;
			y0 = myShapes[y].points[0].y;
			y1 = myShapes[y].points.back().y;
			newShape.pointSortX();
			if ((abs(x0 - X0) < 50) && (abs(y0 - Y0) < 50) && (abs(x1 - X1) < 50) && (abs(y1 - Y1) < 50))
				return true;
		}
	}
	return false;
}

void checkShapeType() {
        double xrange1, xrange2, yrange1, yrange2;
        int count = 0;
        for (int i = 0; i < myShapes.size(); i++) {
                if (myShapes[i].type == 'T') {
					/*
                        myShapes[i].pointSortX();
                        xrange1 = myShapes[i].points[0].x;
                        xrange2 = myShapes[i].points[3].x;
                        myShapes[i].pointSortY();
                        yrange1 = myShapes[i].points[0].y;
                        yrange2 = myShapes[i].points[3].y;
						*/
						Rect rect = boundingRect( myShapes[i].points );
						xrange1 = rect.x;
						xrange2 = rect.x+rect.width;
						yrange1 = rect.y;
						yrange2 = rect.y+rect.height;

                        for (int j = 0; j < myShapes.size(); j++) {
                                if (myShapes[j].type == 'l') {
									for (int y = 0; y < myShapes[j].size; y++) {
                                                if (myShapes[j].points[y].x < xrange2 && myShapes[j].points[y].x > xrange1 && myShapes[j].points[y].y < yrange2 && myShapes[j].points[y].y > yrange1)
                                                        count += 1;
                                        }
									if (count == myShapes[j].size) {
                                                myShapes[i].type = 'L'; // big L indicating links                                                
                                                count = 0; // reset variables
                                                myShapes.erase(myShapes.begin() + j); // erase useless shape
                                                i -= 1; // reset i since myShapes is now smaller in size
                                                j -= 1; // reset j since myShapes is now smaller in size
                                                break; // break out of for loop
                                        }
                                }
								if (myShapes[j].type == 't') {
									for (int y = 0; y < myShapes[j].size; y++) {
										if (myShapes[j].points[y].x < xrange2 && myShapes[j].points[y].x > xrange1 && myShapes[j].points[y].y < yrange2 && myShapes[j].points[y].y > yrange1)
											count += 1;
									}
									if (count == myShapes[j].size) {
										myShapes[i].type = 'B'; // big B indicating its a table                                               
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
void writeHTML(string html){


	ofstream html_file("output.html");
	
	if( html_file.is_open() ){
		html_file << html << endl;
	}
	html_file.close();
}
string getPreparedText(int width,int height){

	const int CHARWIDTH = 5;
	const int LINEHEIGHT = 30;
	string text = dummyText;
	int len = text.length()*CHARWIDTH;
	int preferredNumLines = height/LINEHEIGHT;
	int numLinesInText = len/width;
	

	//if too short
	while( numLinesInText < preferredNumLines - 2 ){
		text+="\n"+dummyText;
		len = text.length()*CHARWIDTH;
		numLinesInText = len/width;
	}
	//if too long
	if( numLinesInText > preferredNumLines ){

		int linesToLose = numLinesInText-preferredNumLines;
		int numCharsToLose = (width*linesToLose)/CHARWIDTH;
		int end = text.length()-numCharsToLose;
		end =  text.find('.',end-10);
		text = text.substr(0,end+1);
	}
	return text;
	
}
vector<Website> getLinks(int height){

	const int LINEHEIGHT = 25;
	int preferredNumLines = height/LINEHEIGHT;
	vector<Website> links;
	srand(time(NULL));
	int i= rand()%websites.size();

	while( links.size() < preferredNumLines ){

		links.push_back(  websites[i++] );
		if( i==websites.size() ){
			i=0;
		}
	}
	return links;
}
string getTable(int width,int height,int tabLevel){

	const int CHARWIDTH = 5;
	const int LINEHEIGHT = 40;
	int numCols = width/(CHARWIDTH*15);
	int numRows = height/LINEHEIGHT;
	string tabs = "";

	for(int i=0;i<tabLevel;i++)
		tabs+="\t";

	string table = tabs+"<table class=\"table\">\r\n";

	for(int i=0;i<numRows;i++){
		table+=tabs+"\t<tr>\n";
		for(int j=0;j<numCols;j++ ){

			string num = std::to_string((i*numCols)+j+1);
			table+=tabs+"\t\t<td>"+num+"</td>\r\n";

		}
		table+=tabs+"\t</tr>\r\n";
	}
	table+=tabs+"</table>\r\n";
	return table;
}
void getDummyLinks(){

	ifstream linksFile("dummy_links.csv");
	string line="";
	if( !linksFile.is_open() ){
		cout << "Problem opening links file"<<endl;
	}
	while( getline(linksFile,line) ){
		int comma = line.find(',');
		string name = line.substr(0,comma);
		string address = line.substr(comma+1,line.length());
		websites.push_back( Website(name,address) );
	}
	linksFile.close();
}
string createHead(){


	string head = "<html>\r\n";
	head += "\t<head>\r\n";
	head += "\t\t<link rel=\"stylesheet\" href=\"css/bootstrap.css\"  type=\"text/css\"/>\r\n";
	head += "\t\t<link rel=\"stylesheet\" href=\"style.css\"  type=\"text/css\"/>\r\n";
	head += "\t</head>\r\n";
	head += "\t<body>\r\n";
	head += "\t\t<script src=\"http://code.jquery.com/jquery-1.10.1.min.js\"></script>\r\n";
	head += "\t\t<script src=\"js/bootstrap.js\"></script>\r\n";
	return head;
}
string createEnd(){

	string end = "\t</body>\r\n";
	end += "<html>\r\n";
	return end;
}
string createBody(Div div,int divOrientation,string body,int tabLevel,bool floatLeft){

	string tabs;
	char info[200];
	bool left=false;
	for(int i=0;i<tabLevel;i++)
		tabs+="\t";
				
	left = true;

	if( div.children.size()>0 ){
		if( divOrientation == Div::HORIZONTAL && tabLevel != 2){
			sprintf(info,"id=\"%d\" class=\"container\" style=\"float:left;width:%dpx;\"",div.id,div.width);
		}else{
			if( tabLevel==2 )//outermost div
				sprintf(info,"id=\"%d\" class=\"container\" style=\"width:%dpx;\"",div.id,div.width);
			else
				sprintf(info,"id=\"%d\" style=\"width:%dpx;float:left\"",div.id,div.width);
		}
		body += tabs+"<div "+string(info)+">\r\n";
		for( int i=0; i < div.children.size(); i++ ){
			body = createBody(div.children[i],div.divOrientation,body,tabLevel+1,left);
		}
		body += tabs+"</div><!-- end"+std::to_string(div.id)+"!-->\r\n";
		return body;
	}
	else if( div.children.size() == 0 ){
		
		if( div.divContent == Div::TEXT ){
			
			sprintf(info,"id=\"%d\" style=\"float:left;width:%dpx;\"",div.id,div.width);
			body += tabs+"<div "+string(info)+">\r\n";
			body += tabs+"\t<p>\r\n"+tabs+"\t\t"+getPreparedText(div.width,div.height)+"\r\n"+tabs+"\t</p>\r\n";
			body += tabs+"</div><!-- end"+std::to_string(div.id)+"!-->\r\n";
			return body;
		}
		if( div.divContent == Div::IMAGE ){
		
			body += tabs+"<div style=\"float:left\">\r\n";
			sprintf(info,"id=\"%d\" style=\"width:%dpx;height:%dpx;display:table-cell;vertical-align:middle;\"",div.id,div.width-2*PADDING,div.height);
			body += tabs+"\t<div "+string(info)+">\r\n";
			body += tabs+"\t\t<img src=\""+dummy_image+"\"/>\r\n";
			body += tabs+"\t</div><!-- end"+std::to_string(div.id)+"!-->\r\n";
			body += tabs+"</div>\r\n";
			return body;
		}
		if( div.divContent == Div::LINKS ){
			
			sprintf(info,"id=\"%d\" style=\"float:left;width:%dpx;\"",div.id,div.width);
			body += tabs+"<div "+string(info)+">\r\n";
			body += tabs+"\t<ul class=\"nav nav-list\">\r\n";
			vector<Website> links = getLinks(div.height);
			for(int i=0;i<links.size();i++){

				Website w = links[i];
				body += tabs+"\t\t<li><a href=\""+w.address+"\"/>"+w.name+"</a></li>\r\n";
			}
			body += tabs+"\t</ul>\r\n";
			body += tabs+"</div><!-- end"+std::to_string(div.id)+"!-->\r\n";
			return body;
		}
		if( div.divContent == Div::TABLE ){
			
			body += tabs+"<div style=\"float:left\">\r\n";
			sprintf(info,"id=\"%d\" style=\"width:%dpx;height:%dpx;display:table-cell;vertical-align:middle;\"",div.id,div.width-2*PADDING,div.height);
			body += tabs+"\t<div "+string(info)+">\r\n";
			body += getTable(div.width,div.height,tabLevel+2);
			body += tabs+"\t</div><!-- end"+std::to_string(div.id)+"!-->\r\n";
			body += tabs+"</div>\r\n";
			return body;
		}
	}
	
}
string createHTML(Div mainDiv){

	string head = createHead();
	string end = createEnd();
	string body = "";
	body = createBody(mainDiv,mainDiv.divOrientation,body,2,false);

	string html = head + body + end;

	return html;
}

void getMaxWidthOfAllDivs(vector<Div> divs){

	if( divs.size() > 0 ){

		int minCol = divs[0].col;
		int maxCol = divs[0].col+divs[0].width;

		for( int i=1;i<divs.size();i++ ){

			if( divs[i].col < minCol )
				minCol = divs[i].col;
			if( divs[i].col+divs[i].width > maxCol )
				maxCol = divs[i].col+divs[i].width;
		}
		drawnDivWidth = maxCol - minCol;
		divComparison = 0.125*drawnDivWidth;
	}
}
void getChildHeight(vector<Div> *children, int height){

	int combinedHeight = 0;

	for(int i=0;i<children->size();i++){

		for(int j=0;j<children->size();j++){

			if( abs( (*children)[i].height - (*children)[j].height ) < divComparison ){

				if( (*children)[i].height > (*children)[j].height){
					(*children)[i].height = (*children)[j].height;
				}else{
					(*children)[j].height = (*children)[i].height;
				}

			}
		}
	}
	
	double *totalHeights = new double[children->size()];
	double total=0;
	for( int i=0;i<children->size();i++ ){
		totalHeights[i]= ceil((double)((*children)[i].height)/(*children)[0].height);
		cout << "totalHeights[i] " << totalHeights[i] <<endl;
		total += totalHeights[i];
	}
	double divVal = height/total;
	
	for(int i=0;i<children->size();i++){

		(*children)[i].height = totalHeights[i]*divVal;
		combinedHeight += (*children)[i].height;
	}
	if( combinedHeight!= height ){
		(*children)[ children->size()-1 ].height += (height-combinedHeight);
	}
	delete [] totalHeights;
}
void getChildWidth(vector<Div> *children, int width){

	int combinedWidth = 0;

	for(int i=0;i<children->size();i++){

		for(int j=0;j<children->size();j++){

			if( abs( (*children)[i].width - (*children)[j].width ) < divComparison ){

				if( (*children)[i].width > (*children)[j].width){
					(*children)[i].width = (*children)[j].width;
				}else{
					(*children)[j].width = (*children)[i].width;
				}

			}
		}
	}
	
	double *totalWidths = new double[children->size()];
	double total=0;
	for( int i=0;i<children->size();i++ ){
		totalWidths[i]= ceil((double)((*children)[i].width)/(*children)[0].width);
		cout << "totalWidths[i] " << totalWidths[i] <<endl;
		total += totalWidths[i];
	}
	double divVal = width/total;
	
	for(int i=0;i<children->size();i++){

		(*children)[i].width = totalWidths[i]*divVal;
		combinedWidth += (*children)[i].width;
	}
	if( combinedWidth!= width ){
		(*children)[ children->size()-1 ].width += (width-combinedWidth);
	}
	delete [] totalWidths;
}
void resizeAllDivs(Div &div,int width,int height){


	cout << "Width: "<<width<<endl;

	div.width = width;
	div.height = height;

	if( div.children.size() > 0 ){

		if( div.divOrientation == Div::VERTICAL ){

			getChildHeight( &div.children , height );
			for(int i = 0; i < div.children.size(); i++){
				int childHeight = div.children[i].height-2*PADDING;
				resizeAllDivs(div.children[i],width-2*PADDING,childHeight);
			}
			//set width to be maximum child's width;
			int maxWidth = div.children[0].width;

			for(int i=1;i<div.children.size();i++){
				if( div.children[i].width > maxWidth )
					maxWidth = div.children[i].width;
			}
			for(int i=0;i<div.children.size();i++){
				div.children[i].width = maxWidth;
			}
		}
		else{

			getChildWidth( &div.children , width );
			for (int i = 0; i < div.children.size(); i++){
				int childWidth = div.children[i].width-2*PADDING;
				resizeAllDivs( div.children[i],childWidth,height-2*PADDING);
			}
			//set height to be maximum child's height
			int maxHeight = div.children[0].height;

			for(int i=1;i<div.children.size();i++){
				if( div.children[i].height > maxHeight )
					maxHeight = div.children[i].height;
			}
			for(int i=0;i<div.children.size();i++){
				div.children[i].height = maxHeight;
			}
		}
	}
}
void resizeAllDivs2(Div &div,int width){


	cout << "Width: "<<width<<endl;

	div.width = width;

	if( div.children.size() > 0 ){

		if( div.divOrientation == Div::VERTICAL ){

			for(int i = 0; i < div.children.size(); i++){

				resizeAllDivs2(div.children[i],width-2*PADDING);
			}
		}
		else{

			size_t childWidth = width/div.children.size()-2*PADDING;

			for (int i = 0; i < div.children.size(); i++){

				resizeAllDivs2(div.children[i],childWidth);
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

			if (abs(div1.row - div2.row) < 20+divComparison &&
				abs(div1.height - div2.height) < 20+divComparison){
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

			if (abs(div1.col - div2.col) < 20+divComparison &&
				abs(div1.width - div2.width) < 20+divComparison){
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

	if( divs.size() == 0 ){
		return divs[0];
	}
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

				if( divs[i].similar(nonDup[j],drawnDivWidth)){

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
			Rect rect = boundingRect(myShapes[i].points);
			if (myShapes[i].type == 'T' || myShapes[i].type == 'L' || myShapes[i].type == 'I' || myShapes[i].type == 'B'){
				cout <<"i: "<<i<<endl;
				Div newDiv(&myShapes[i]);
				newDiv.setId(divId);
				divId += 1;
				tempDiv.push_back(newDiv);
			}
		}
		getMaxWidthOfAllDivs(tempDiv);
		tempDiv = removeDupDivs(tempDiv);
		printDivs(tempDiv);
		cout<<"setup hierarchy"<<endl;
		if( tempDiv.size() > 0 )
			mainDiv = setupHierarchy(tempDiv, 0);
	}
	return mainDiv;
}

/*
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
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC4);
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
} */

/** @function thresh_callback */
void doWork2(Mat &img, Mat &src_gray, int, void*)
{
	Mat threshImage;
	vector<vector<Point> > contours;
	vector<Point> approxShape;
	/*
	threshold(src_gray,threshImage,127,255,THRESH_BINARY_INV);
	imshow("INV",threshImage);
	threshold(src_gray,threshImage,127,255,THRESH_TRUNC);
	imshow("TRUNC",threshImage);
	threshold(src_gray,threshImage,127,255,THRESH_TOZERO);
	imshow("2ZERO",threshImage);
	threshold(src_gray,threshImage,127,255,THRESH_TOZERO_INV);
	imshow("2ZEROINV",threshImage);
	*/
	double ret = threshold(src_gray,threshImage,127,255,THRESH_BINARY_INV);
	findContours(threshImage,contours,1,2);
	imshow("THRESH",threshImage);
	
	int compareVal = 30;

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(contours[i], approxShape, 0.01*arcLength(contours[i], true), true);
		Rect rect = boundingRect( contours[i] );

		if( rect.width < compareVal && rect.height < compareVal ){
			continue;
		}
		if( rect.x < 10 || rect.y < 10 || rect.x+rect.width + 20 > img.cols || rect.y+rect.height + 20 > img.rows ){
			continue;
		}/*
		int count=0;
		for(int i=0;i<approxShape.size();i++){

			if( approxShape[i].x < 5 || approxShape[i].x > img.cols - 5 || approxShape[i].y < 5 || approxShape[i].y > img.rows - 5 ){
				count ++;
			}
		}
		if( count > 0.5*approxShape.size() ){
			continue;
		}*/
		if (checkShapes(approxShape)) {
			// do nothing because it's a duplicate
		}
		else {
			Scalar color;
			myShape newShape(approxShape);
			newShape.pointSortX();
			if ( approxShape.size() >=2  && rect.width < compareVal && rect.height > compareVal ){
				newShape.type = 'l'; // l for line
				color = Scalar(255, 255, 0);
			}
			else if( approxShape.size() >=2 && rect.height < compareVal && rect.width > compareVal){
				newShape.type = 't'; // t for triangle
				color = Scalar(0,255,0);
			}
			else if (approxShape.size() == 3){
				newShape.type = 't'; // t for triangle
				color = Scalar(0,255,0);
			}
			else if (approxShape.size() == 4){
				newShape.type = 'T'; // T for Text
				color = Scalar(255,0,0);
			}
			else if (approxShape.size() > 4 && abs( rect.height - rect.width ) > compareVal ){
				newShape.type = 'T'; // T for Text
				color = Scalar(255,0,0);
			}
			else if (approxShape.size() > 7 ){// it's a circle
				newShape.type = 'c'; // c for circle
				color = Scalar(0,0,255);
			}
			myShapes.push_back(newShape);
			drawContours(img, contours, i, color, 2);
		}
	}
	for(int i=0;i<myShapes.size();i++){
		Rect rect = boundingRect(myShapes[i].points);
		printf("Size: %d bounding box (%d,%d) %dx%d\n",myShapes[i].points.size(),rect.x,rect.y,rect.width,rect.height);
	}
	cout << "Checking shape type"<<endl;
	checkShapeType();
}
int main(int argc, char** argv) {

	cout << "Choose an image file"<<endl;
	cout << " - 0 for rects2.png"<<endl;
	cout << " - 1 for shapes13.png"<<endl;
	cout << " - 2 for test.png'"<<endl;
	cout << " - 3 for test2.png'"<<endl;
	cout << " - 4 for htmlPhoto1.png'"<<endl;
	cout << " - 5 for shapes7.png'"<<endl;
	cout << " - 6 for shapes8.png'"<<endl;
	cout << " - 7 for shapes10.png'"<<endl;
	cout << " - 8 for shapes11.png'"<<endl;
	cout << " - 9 for shapes12.png'"<<endl;
	cout << " - 10 for Capture3.png"<<endl;

	//int choice = getchar()-'0';
	char buf[10];
	fgets(buf,5,stdin);
	int choice = atoi(buf);

	string file = "HTML Drawings/";

	switch( choice ){

		case 0:{file+="rects2.png";break;}
		case 1:{file+="shapes13.png";break;}
		case 2:{file+="test.png";break;}
		case 3:{file+="test2.png";break;}
		case 4:{file+="htmlPhoto1.png";break;}
		case 5:{file+="shapes7.png";break;}
		case 6:{file+="shapes8.png";break;}
		case 7:{file+="shapes10.png";break;}
		case 8:{file+="shapes11.png";break;}
		case 9:{file+="shapes12.png";break;}
		case 10:{file+="Capture3.png";break;}
		default:{file+="test.png";break;}
	}
	
	Mat img, gray;
	cout << "Read image" << endl;
	readImage(file, &img, &gray);
	getDummyLinks();
	

	cout << "Do work" << endl;
	doWork2(img,gray, 0, 0);
	cout << "Setup divs" << endl;
	imshow("Main image", img);
	waitKey(0);
	Div mainDiv = setUpDivs();
	cout << "printing hierarchy" << mainDiv.children.size() << endl;
	resizeAllDivs(mainDiv,MAX_PAGE_WIDTH,MAX_PAGE_HEIGHT);
	mainDiv.printDivWithChildren(0);
	string html = createHTML(mainDiv);
	writeHTML(html);
	cout << "cols: "<< img.cols << " rows: "<< img.rows <<endl;
	
	
	system ("start file:///C:/Users/Holly/Documents/GitHub/Python-Image-Detect/UI%20Project/UI%20Project/output.html");
	return 0;
}
