#include "Div.hpp"

Div::Div(){
}
Div::Div(int row, int col, int width, int height, vector<Div>children, DivOrientation divOrientation, int divID){
	this->row = row;
	this->col = col;
	this->width = width;
	this->height = height;
	this->divOrientation = divOrientation;
	this->children = vector<Div>(children);
	this->id = divID;
}
Div::Div(Div *div){
	this->row = div->row;
	this->col = div->col;
	this->width = div->width;
	this->height = div->height;
	this->divOrientation = div->divOrientation;
	this->children = div->children;
	this->id = div->id;
}

Div::Div(myShape *ms) {
	ms->pointSortX();
	this->col = ms->points[0].x;
	this->width = abs(ms->points[3].x - ms->points[0].x);
	ms->pointSortY();
	this->row = ms->points[0].y;	
	this->height = abs(ms->points[3].y - ms->points[0].y);
	this->children = (vector<Div>)NULL;
	this->divOrientation = NONE;
	switch( ms->type ){
		case 'B': {this->divContent = TABLE;break;}
		case 'I': {this->divContent = IMAGE;break;}
		case 'L': {this->divContent = LINKS;break;}
		case 'T': {this->divContent = TEXT;break;}
	}
}

void Div::setId(int divID){
	this->id = divID;
}
string Div::printDiv() const {
	char buffer[150];
	string orient="";
	switch( divOrientation ){
	case HORIZONTAL: {orient = "HORIZONTAL";break;}
	case VERTICAL:{orient = "VERTICAL";break;}
	default:{orient="NONE";break;}
	}
	const char * orient2 = orient.c_str();
	sprintf(buffer, "(%d,%d) %d x %d --- %s", row, col, width, height,orient2);
	return string(buffer);
}
bool Div::match(Div div2) const {
	if (this->row == div2.row && this->col == div2.col &&
		this->width == div2.width && this->height == div2.height){
		return true;
	}
	return false;
}
bool Div::similar(Div div2,int divComparison) const {
	if (abs(this->row - div2.row) < 10 &&
		abs(this->col - div2.col) < 10 &&
		abs(this->width - div2.width) < 20 &&
		abs(this->height - div2.height) < 20){
		return true;
	}
	return false;
}
void Div::printDivWithChildren(int level){

	string indent = "";
	for (int i = 0; i<5 * level; i++)
		indent += " ";
	cout << indent + this->printDiv() << endl;
	cout << this->children.size() << endl;
	for (int i = 0; i<this->children.size(); i++){
		this->children[i].printDivWithChildren(level + 1);
	}
}