#include <iostream>
#include <string.h>
#include <vector>
#include "mytimer.h"
#include "player.h"
#include <cmath>
#include <stdlib.h>
#include <fstream>
#include <cstdlib>
#include <cmath>

int* include;
int* bestSet;
int* tempBestSet;
int maxProfit = 0;
int globalC;
int globalPlayerCount;
int numTaken=1;
int numAvoided=0;
int mode=3;

int comp(const void* a, const void* b){
	Player one = *(Player*)b;
	Player two = *(Player*)a;
	if((one.pow - two.pow) > 0){return 1;}
	else{return -1;}
}

int greedy1(int i, int weight, Player* arr, int size, int C, ofstream& myOut){
	if(mode == 0){myOut << "Greedy1: ";}
	int arrSet[size];
	int resultingProfit=0;
	for(int j=0; j<size; j++){
		arrSet[j] = 0;
	}
	while(weight < C && i < size){
		if(weight + arr[i].buyingPrice <= C){
			arrSet[i] = 1;
			bestSet[i] = 1;
			weight += arr[i].buyingPrice;
			resultingProfit += arr[i].profit;
		}
		i++;
	}
	return resultingProfit;
}

int greedy2(int i, int weight, Player* arr, int size, int C, ofstream& myOut){
	if(mode == 1){myOut << "Greedy2: ";}
	int arrSet[size];
	int resultingProfit=0;
	for(int j=0; j<size; j++){
		arrSet[j] = 0;
	}
	while(weight < C && i < size){
		if(weight + arr[i].buyingPrice <= C){
			arrSet[i] = 1;
			bestSet[i] = 1;
			weight += arr[i].buyingPrice;
			resultingProfit += arr[i].profit;
		}
		i++;
	}
	int playerWithMaxProfit=0, maxP=0;
	for(int j=0; j<size; j++){
		if(arr[j].profit > maxP){
			playerWithMaxProfit = j;
			maxP = arr[j].profit;
		}
	}
	if(maxP > resultingProfit){
		resultingProfit = maxP;
		bestSet[playerWithMaxProfit] = 1;
	}
	return resultingProfit;
}

int KWF(int i, int weight, Player* arr, int size, int C, ofstream& myOut){
	float arrSet[size];
	int oldi = i;
	int resultingProfit=0;
	for(int j=i; j<size; j++){
		arrSet[j] = 0;
	}
	while(weight < C && i < size){ 
		if(weight + arr[i].buyingPrice <= C){
			arrSet[i] = 1;
			weight += arr[i].buyingPrice;
			resultingProfit += arr[i].profit;
		}
		else{
			arrSet[i] = (float)(C - weight)/(float)arr[i].buyingPrice;
			resultingProfit += (int)(arrSet[i] * arr[i].profit);
			weight = C;
		}
		i++;
	}
	return resultingProfit;
}

int KWF2(int i, int weight, Player* arr, int size, int C, ofstream& myOut, int profitSoFar){
	return KWF(i,weight,arr,size,C,myOut) + profitSoFar;
}

bool promising(int i, int weight, int profit, Player* arr, ofstream& myOut){
	if(weight >= globalC){return false;}
	int ret = KWF2(i+1,weight,arr,globalPlayerCount,globalC,myOut,profit);
	return(ret > maxProfit);
}

void knapsack(int profit, int weight, int i, Player* arr, ofstream& myOut){
	if(mode == 2){myOut << "Backtracking: ";}
	if(weight <= globalC && profit > maxProfit){
		maxProfit = profit;
		for(int q=0; q<globalPlayerCount; q++){
			bestSet[q] = tempBestSet[q];
		}
	}
	if(promising(i,weight, profit, arr, myOut)){
		numTaken += 2;
		//include i+1
		tempBestSet[i+1] = 1;
		knapsack(profit+arr[i+1].profit,weight+arr[i+1].buyingPrice,i+1, arr, myOut);
		//dont
		tempBestSet[i+1] = 0; //undo it
		knapsack(profit,weight,i+1,arr,myOut);
	}
	else{
		if(i != globalPlayerCount-1) {numAvoided += pow(2,globalPlayerCount-i)-2;}
	}
}

/**DYNAMIC CODE STARTS HERE **/

void dynamic(ofstream& myOut, int S1, int moneyToSpend, Player* arr){
	myOut << "Dynamic Programming: ";
	int** thisArray = new int*[S1+1];
	int lesserToSpend=0;
	int currentPlayer=0;
	thisArray[0] = new int[moneyToSpend+1];
	for(lesserToSpend=0; lesserToSpend<=moneyToSpend; lesserToSpend++){
		thisArray[0][lesserToSpend] = 0;
	}
	for(currentPlayer=1; currentPlayer<=S1; currentPlayer++){
		if(currentPlayer > 1){
			delete [] thisArray[currentPlayer-2];
		}
		thisArray[currentPlayer] = new int[moneyToSpend+1];
		thisArray[currentPlayer][0] = 0;
		for(lesserToSpend=1; lesserToSpend<=moneyToSpend; lesserToSpend++){
			if(arr[currentPlayer-1].buyingPrice <= lesserToSpend){
				if(thisArray[currentPlayer-1][lesserToSpend-arr[currentPlayer-1].buyingPrice] + arr[currentPlayer-1].profit > thisArray[currentPlayer-1][lesserToSpend]){
					thisArray[currentPlayer][lesserToSpend] = thisArray[currentPlayer-1][lesserToSpend-arr[currentPlayer-1].buyingPrice] + arr[currentPlayer-1].profit;
				}
				else{
					thisArray[currentPlayer][lesserToSpend] = thisArray[currentPlayer-1][lesserToSpend];
				}
			}
			else{
				thisArray[currentPlayer][lesserToSpend] = thisArray[currentPlayer-1][lesserToSpend];
			}
		}
	}
	myOut << S1 << " " << thisArray[S1][moneyToSpend] << " ";
	delete [] thisArray[currentPlayer-2];
	delete [] thisArray[currentPlayer-1];
	delete [] thisArray;
}

int main(int argc, char** argv){
	struct timeval start, end;
	if(argc != 8 || (strcmp(argv[1],"-m") != 0) || (strcmp(argv[3],"-p") != 0) || (strcmp(argv[5],"-o") != 0)){
		cout << "Usage:\nprogram1 -m <market-price-file> -p <price-list-file>\n";
		exit(1);
	}

	ifstream marketFile(argv[2]);
	ifstream priceFile(argv[4]);
	ofstream myOut(argv[6]);
	int currentFileIndex = 0;
	if(!(marketFile.is_open() && priceFile.is_open())){
		myOut << "File(s) not found. Terminating.\n";
		exit(1);
	}
	int mode = atoi(argv[7]);
	string price = argv[4];
	string line, s1, s2;
	getline(marketFile, line);
	int numCards = atoi(line.c_str());
	Player* arrMarket = new Player[numCards];
	int playerIndex = 0;
	int space, cardsToCome, constCardsToCome;
	int moneyToSpend = 0;
	int startingIndexToSell = 0;
	while(getline(marketFile, line)){
		space = line.find(' ');
		s1 = line.substr(0,space);
		s2 = line.substr(space + 1, line.length()-space-1);
		arrMarket[playerIndex].name = s1;
		arrMarket[playerIndex].sellingPrice = atoi(s2.c_str());
		playerIndex++;
	}
	marketFile.close();


	while(getline(priceFile, line)){
		numTaken = 1;
		numAvoided = 0;
		space = line.find(' ');
		s1 = line.substr(0, space);
		int S1 = atoi(s1.c_str());
		s2 = line.substr(space+1, line.length()-space-1);
		moneyToSpend = atoi(s2.c_str());
		Player* arr = new Player[S1];
		globalC = moneyToSpend;
		int arrIndex=0;
		int decS1 = S1;
		int j=0;
		while(decS1){
			decS1--;
			getline(priceFile, line);
			space = line.find(' ');
			s1 = line.substr(0, space);
			s2 = line.substr(space+1, line.length()-space-1);
			while(arrMarket[j].name != s1){j++;}
			arr[arrIndex].name = s1;
			arr[arrIndex].sellingPrice = arrMarket[j].sellingPrice;
			arr[arrIndex].buyingPrice = atoi(s2.c_str());
			arr[arrIndex].profit = arr[arrIndex].sellingPrice - arr[arrIndex].buyingPrice;
			arr[arrIndex].pow = (float)arr[arrIndex].profit/(float)arr[arrIndex].buyingPrice;
			arrIndex++;
		}
		qsort(arr, S1, sizeof(Player), comp);
		int weight=0, index=0;
		int* includeArr = new int[S1];
		int* bestSetArr = new int[S1];
		int* tempBestSetArr = new int[S1];
		for(int init=0; init<S1; init++){
			bestSetArr[init] = 0;
			tempBestSetArr[init] = 0;
		}
		include = includeArr;
		bestSet = bestSetArr;
		tempBestSet = tempBestSetArr;
		globalPlayerCount = S1;
		
		struct timeval start, end;
		start_the_clock(&start);
		if(mode == 0){maxProfit = greedy1(0,0,arr,S1,moneyToSpend,myOut);}
		else if(mode == 1){maxProfit = greedy2(0,0,arr,S1,moneyToSpend,myOut);}
		else if(mode == 2){
			maxProfit = greedy2(0,0,arr,S1,moneyToSpend,myOut);
			knapsack(0,0,-1,arr,myOut);
		}
		else if(mode == 3){
			dynamic(myOut, S1, moneyToSpend, arr);
		}
		double totaltime = total_time(&start, &end);
		if(mode != 3){
			myOut << S1 << " " << maxProfit << " ";
			int numToAchieve = 0;

			for(int w=0; w<S1; w++){
				if(bestSet[w] == 1){
					numToAchieve++;
				}
			}
			myOut << numToAchieve << " " << totaltime << endl;
			if(mode == 2 && price != "LargePriceList.txt"){myOut << numTaken << " subsets considered; " << numAvoided << " subsets avoided via backtracking" << endl;} // << pow(2, globalPlayerCount+1)-1 << endl;
			for(int w=0; w<S1; w++){
				if(bestSet[w] == 1){
					myOut << arr[w].name << endl;
				}
			}
		}
		else{
			myOut << totaltime << endl;
		}
		delete [] arr;
		delete [] includeArr;
		delete [] bestSetArr;
		delete [] tempBestSetArr;
	}


	priceFile.close();
	delete [] arrMarket;
	return 0;
}
