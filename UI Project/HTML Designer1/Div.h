#include <vector>
#include <string>

using namespace std;

class Div {

public:
	static int divId;
	int row;
	int col;
	int id;
	int width;
	int height;
	int divType;
	vector<Div> children;

	Div();
	Div(int row,int col,int width,int height,vector<Div>children,int divType);
	Div(Div *div);
	string printDiv();
	bool match(Div div2 );
	bool similar(Div div2 );
	friend bool operator==(const Div &div1, const Div &div2);
};