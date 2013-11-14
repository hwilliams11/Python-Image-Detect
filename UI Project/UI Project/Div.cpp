#include "Div.hpp"

Div::Div(){
}
Div::Div(int row, int col, int width, int height, vector<Div>children, int divType, int divID){
	this->row = row;
	this->col = col;
	this->width = width;
	this->height = height;
	this->divType = divType;
	this->children = vector<Div>(children);
	this->id = divID;
}
Div::Div(Div *div){
	this->row = div->row;
	this->col = div->col;
	this->width = div->width;
	this->height = div->height;
	this->divType = div->divType;
	this->children = div->children;
	this->id = div->id;
}

Div::Div(myShape *ms) {
	this->row = ms->points[0].y;
	this->col = ms->points[0].x;
	this->width = abs(ms->points[1].x - ms->points[0].x);
	this->height = abs(ms->points[2].y - ms->points[0].y);
	this->children = (vector<Div>)NULL;
	this->divType = -1;
}

void Div::setId(int divID){
	this->id = divID;
}
string Div::printDiv() const {
	char buffer[100];
	sprintf(buffer, "(%d,%d) %d x %d", row, col, width, height);
	return string(buffer);
}
bool Div::match(Div div2) const {
	if (this->row == div2.row && this->col == div2.col &&
		this->width == div2.width && this->height == div2.height){
		return true;
	}
	return false;
}
bool Div::similar(Div div2) const {
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