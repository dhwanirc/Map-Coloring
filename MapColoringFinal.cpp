#include<iostream>
#include<vector>
#include<time.h>
#include<map>
#include<string>
#define WITHOUT_COLOR "NoColor"


using namespace std;

int c = 0;
int maxstep;

clock_t t_fc, t_hill;

//Generates random number within 0 and given limit
int getRandom(int limit) {
	return rand() % limit;
}
class Region {
public:
	int number;
	string color;
	vector<int> neighbourRegion;
	map<string, bool> possibleColors;
	//Initialization of Forward Checking
	void initFC(int i) {
		number = i + 1;
		color = WITHOUT_COLOR; // The color assigned to the region, initially colorless
							   //Initially all colors are available for this region
		possibleColors["Blue"] = true;
		possibleColors["Yellow"] = true;
		possibleColors["Green"] = true;
		possibleColors["Red"] = true;

	}
	//Initialization of Min-Conflict Algo
	void initMinConflict(int i) {
		number = i + 1;
		gencolor(getRandom(4));
	}
	//Assigns color to the region
	void gencolor(int cnum) {
		if (cnum == 0)
			color = "Blue";
		else if (cnum == 1)
			color = "Yellow";
		else if (cnum == 2)
			color = "Green";
		else if (cnum == 3)
			color = "Red";
	}
	//Assigns neighbours from the generated map.
	void genNeighbour(int i, int size, int** Map) {

		for (int j = 0; j < size; j++) {
			if (Map[i][j] == 1) {
				neighbourRegion.push_back(j + 1);
			}
		}
	}
	//Returns the Region-number of neighbours
	vector<int> getNeighbors() {
		return neighbourRegion;
	}
	//Checks if the region has color or not
	bool hasColor() {
		if (color == WITHOUT_COLOR)
			return false;
		return true;
	}
	//Updates colors of neighbour region in MRV 
	void updatePossibleColors(bool flag, string colorName) {
		possibleColors[colorName] = flag;
	}

	//Returns to quantity of available cores for this region
	int getPossibleColors() {
		int cont = 0;
		for (map<string, bool>::iterator it = possibleColors.begin(); it != possibleColors.end(); ++it) {
			if (it->second)
				cont += 1;
		}
		return cont;
	}
	//Returns to name of available colors for this region
	vector<string> getPossibleColorsname() {
		vector<string> col;
		for (map<string, bool>::iterator it = possibleColors.begin(); it != possibleColors.end(); ++it) {
			if (it->second)
				col.push_back(it->first);
		}
		return col;
	}

};

class Map {
public:
	vector<Region> regionList;
	vector<string> colors;
	//Initialization of colors
	void MapcolorInt() {
		colors.push_back("Blue");
		colors.push_back("Yellow");
		colors.push_back("Green");
		colors.push_back("Red");
	}
	//Counts the number of conflicts in the given map
	int getHeuristic(int** M) {
		int heuristic = 0;
		for (int i = 0; i < regionList.size(); i++) {
			for (int j = 0; j <= i; j++) {
				if (M[i][j] == 1 && regionList.at(i).color == regionList.at(j).color) {
					heuristic++;
					//cout << i + 1 << " & " << j +1<< " have same color";
				}

			}
			//cout << "\n";
		}
		return heuristic;
	}
	//Prints the solution for the given map
	void printSolution() {
		for (int i = 0; i < regionList.size(); i++) {
			cout << regionList.at(i).number << " will have " << regionList.at(i).color << "\n";
		}
	}
	//Chooses the region with the fewest possible colors
	int selectVariableMVR() {
		int mrv = colors.size() + 1;
		int variable = 0;
		for (int i = 0; i < regionList.size(); i++) {
			if (!regionList[i].hasColor()) {
				if (regionList[i].getPossibleColors() < mrv) {
					variable = i;
					mrv = regionList[i].getPossibleColors();
				}
			}
		}
		return variable;
	}
	void hillclimbiing(int** M) {
		//Get the conflicts for the given map with the colors assigned to it
			int H = getHeuristic(M);
			//If there is no conflicts then print the solution
			if (H == 0) {
				cout << "\n\nsolution for Hill climbing with Min-conflict: \n";
				printSolution();
				t_hill = clock() - t_hill;
				cout << "\nIt took " << (float)t_hill / CLOCKS_PER_SEC << " seconds for solving this problem with Min-conflict in hill climbing.\n";
				exit(0);
			}
			else {
				int nextH = H;
				for (int i = 0; i < regionList.size(); i++) {
					//save the original color to assign it back if the new Heuristic is bigger than older one.
					string originalColor = regionList.at(i).color;
					//Check heuristic for all the colors and assign the color with the least heuristic.
					for (int j = 0; j < colors.size(); j++) {
						if (colors.at(j) != originalColor) {
							regionList.at(i).color = colors.at(j);
							int newH = getHeuristic(M);
							if (newH < nextH) {
								nextH = newH;
								hillclimbiing(M);
							}
						}
					}
					regionList.at(i).color = originalColor;
				}
				H = getHeuristic(M);
				//If no such combination found for the values assigned to the map then local max has been reached.
				if (H != 0) {
					//Check if number of random restarts has not exceeded the maxstep.
					//This loop make sure that the algorithm doesn't run for longer period of time.
					while (c < maxstep) {
					c++;
					cout << "\nLocal Max Reached " <<c << " times with Heuristic " << H;
					//
					for (int i = 0; i < regionList.size(); i++) {
						regionList[i].initMinConflict(i);
						regionList[i].genNeighbour(i, regionList.size(), M);
						}
					hillclimbiing(M);
					}
					//If still the solution is not found than maximum number of restarts than map can be unsolvable.
					cout << "\nTry another map because this map is not solvable";
					exit(0);
			}
		
		}
	
}

	void ForwardChecking() {
		for (int j = 0; j < regionList.size(); j++) {
			//Find the region which has MINIMUM REMAINING VALUES
			int mvr = selectVariableMVR();

			//Get the possible color names for that region
			vector<string> possColorsList = regionList[mvr].getPossibleColorsname();

			//assign one of the possible colors to the region
			regionList.at(mvr).color = possColorsList[0];

			//Find the neighbours of the region to whom the color is just assigned.
			vector<int> neighbor = regionList.at(mvr).getNeighbors();

			//Remove the assigned color from the possible colors of its neighbours
			for (int i = 0; i < neighbor.size(); i++) {
				if (!regionList[neighbor[i] - 1].hasColor()) {
					regionList[neighbor[i] - 1].updatePossibleColors(false, regionList.at(mvr).color);
				}
			}
		}
		//Once all the regions are colored then the solution will be printed
		cout << "\n\nsolution for Forward checking with MRV: \n";
		printSolution();
	}
};

//To print the map
void printMap(int** M, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j< n; j++) {
			cout << M[i][j] << " ";
		}
		cout << "\n";
	}
}


//Create map function generates a practically feasible map.
int** createMap(int n) {
	int** newMap = new int*[n];
	for (int i = 0; i < n; ++i)
		newMap[i] = new int[n];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j)newMap[i][j] = 0;
			else
			{
				newMap[i][j] = -1;
			}
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (newMap[i][j] == -1) {
				if (j == i - 4 || j == i - 3 || j == i - 2 || j == i - 1 || j == i + 1 || j == i + 2 || j == i + 3 || j == i + 4) {
					newMap[i][j] = getRandom(2);
					newMap[j][i] = newMap[i][j];
				}
				else
				{
					newMap[i][j] = 0;
				}
			}

		}
	}

	printMap(newMap, n);
	return newMap;
}


int main()
{
	int n;
	
	srand(time(NULL));
	cout << "Enter number of region for a map: ";
	cin >> n;
	int** M = new int*[n];
	M = createMap(n);

	//Making 2 different regions because FC and Min-conflict algorithm starts with different initiallizations.
	vector<Region> Region_fc(n), Region_hill(n);

	//initialization for the regions in FC includes giving numbers to the regions, giving "NoColor" to the present color of each region
	//making possibility of all color=1 and put the values of neighbors from the map.
	//initialization for the regions Hill climbing with min-conflict includes giving numbers to the regions, giving random color to each region
	//and put the values of neighbors from the map.

	for (int i = 0; i < n; i++) {
		Region_fc[i].initFC(i);
		Region_fc[i].genNeighbour(i, n, M);
		Region_hill[i].initMinConflict(i);
		Region_hill[i].genNeighbour(i, n, M);
	}

	Map map_fc,map_hill;
	map_fc.regionList = Region_fc;
	//Initialize colors with red, blue, green, yellow
	map_fc.MapcolorInt();
	t_fc = clock();
	//Forward checking function of Map class actually assigns the values to all the regions.
	map_fc.ForwardChecking();
	t_fc = clock() - t_fc;
	cout<<"It took "<< (float)t_fc / CLOCKS_PER_SEC <<" seconds for solving this problem with forward checking with Minimum remaining value.\n";

	map_hill.regionList = Region_hill;
	//Initialize colors with red, blue, green, yellow
	map_hill.MapcolorInt();
	//for maximum 20 random restarts.
	maxstep = 20;
	t_hill = clock();
	//Hill climbing algorithm was used as local search algorithm
	map_hill.hillclimbiing(M);
	getchar();
	return 0;
}