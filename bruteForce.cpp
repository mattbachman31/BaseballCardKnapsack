#include <iostream>
#include <string.h>
#include <vector>
#include "mytimer.h"
#include "player.h"
#include <cmath>
#include <stdlib.h>
#include <fstream>

using namespace std;

int main(int argc, char** argv){
	struct timeval start, end;
	ofstream myOut("outfile.txt");
	if(argc != 5 || (strcmp(argv[1],"-m") != 0) || (strcmp(argv[3],"-p") != 0)){
		myOut << "Usage:\nprogram1 -m <market-price-file> -p <price-list-file>\n";
		exit(1);
	}

	ifstream marketFile(argv[2]);
	ifstream priceFile(argv[4]);
	if(!(marketFile.is_open() && priceFile.is_open())){
		myOut << "File(s) not found. Terminating.\n";
		exit(1);
	}
	string line, s1, s2;
	getline(marketFile, line);
	int numCards = atoi(line.c_str());
	Player arr[numCards];
	int playerIndex = 0;
	int space, cardsToCome, constCardsToCome;
	int moneyToSpend = 0;
	int startingIndexToSell = 0;
	while(getline(marketFile, line)){
		space = line.find(' ');
		s1 = line.substr(0,space);
		s2 = line.substr(space + 1, line.length()-space-1);
		arr[playerIndex].name = s1;
		arr[playerIndex].sellingPrice = atoi(s2.c_str());
		playerIndex++;
	}

	/*
	begin writing code to extract buying price from gertrude
	*/
	while(getline(priceFile, line)){
		start_the_clock(&start);
		space = line.find(' ');
		s1 = line.substr(0, space);
		s2 = line.substr(space+1, line.length()-space-1);
		cardsToCome = atoi(s1.c_str());
		constCardsToCome = cardsToCome;
		moneyToSpend = atoi(s2.c_str());
		while(cardsToCome){
			getline(priceFile, line);
			cardsToCome--;
			space = line.find(' ');
			s1 = line.substr(0, space);
			s2 = line.substr(space+1, line.length()-space-1);
			for(int i=startingIndexToSell; i<(constCardsToCome + startingIndexToSell); i++){
				if(arr[i].name == s1){
					arr[i].buyingPrice = atoi(s2.c_str());
					break;
				}
			}
		}

		vector<Player> optimal;
		int bestProfit = 0;

		for(int i=0; i< pow(2,constCardsToCome); i++){
			vector<bool> bitset;
			vector<Player> inside;
			int tempProfit = 0;
			int temp = i;
			int sum = 0;
			while(temp > 0){
				if(temp % 2 == 0){
					bitset.push_back(0);
				}
				else{
					bitset.push_back(1);
				}
				temp /= 2;
			}
			while(bitset.size() < constCardsToCome){
				bitset.push_back(0);
			}
			for(int j=startingIndexToSell; j< (startingIndexToSell + constCardsToCome); j++){
				if(bitset.back() == 1){
					//add to set
					inside.push_back(arr[j]);
					sum += arr[j].buyingPrice;
					tempProfit += (arr[j].sellingPrice - arr[j].buyingPrice); //if strictly profit, offset buying price
				}
				bitset.pop_back();
			}
			if(sum <= moneyToSpend){
				if(tempProfit > bestProfit){
					bestProfit = tempProfit;
					optimal = inside;
				}
			}
		}
		startingIndexToSell += constCardsToCome;
		double totalTime = total_time(&start, &end);
		myOut << constCardsToCome << " " << bestProfit << " " << optimal.size() << " " << totalTime << "\n";

		for(int z = 0; z < optimal.size(); z++){
			myOut << optimal[z].name << "\n"; // ||||| " << optimal[z].sellingPrice << " ||||| " << optimal[z].buyingPrice << "\n";
		}
	}
	myOut.close();
	return 0;
}
