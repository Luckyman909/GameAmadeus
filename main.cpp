#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <utility>

using namespace std;

#define MAX_TURNS 50

const int kEnemyPredictVal = -5;
const int maxX = 30;
const int maxY = 15;


typedef vector<vector<int> > MAP;
MAP gMap(30, vector<int> (15, 0));
pair<int, int> gMyPos;
map<int, pair<int, int>> lastEnemyPos;

enum DIR : int {
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
};


map<DIR, string> dirToString = {{DOWN,"DOWN"}, {UP,"UP"}, {RIGHT,"RIGHT"}, {LEFT,"LEFT"}};
	

pair<int, int> getNextPos(DIR iDir, const pair<int, int> &iCurrentPos)
{
	auto aOutput = iCurrentPos;
	switch(iDir)
	{
		case UP:
			aOutput.second = (aOutput.second - 1 +15)%15;
			break;
		case DOWN:
			aOutput.second = (aOutput.second +1)%15;
			break;
		case LEFT:
			aOutput.first = (aOutput.first - 1 +30 )%30;
			break;
		case RIGHT:
			aOutput.first = (aOutput.first +1)%30;
			break;
	}
	return aOutput;
}

bool detectColision(DIR iDir, const pair<int, int> &iCurrentPos, const MAP& iMap)
{
	pair<int, int> aNextPos = getNextPos(iDir, iCurrentPos);
	return iMap[aNextPos.first][aNextPos.second] != 0;
}

DIR computeDirection(DIR iDir, const pair<int, int> &iCurrentPos, bool&dead, const MAP& iMap)
{
    int nbTurns = 0;
    DIR aDir = iDir, forbiddenDir = static_cast<DIR>((iDir+2)%4);
    while(detectColision(aDir, iCurrentPos, iMap) && nbTurns < 3)
    {
     	cerr  << "Collision detected, turning: (" << dirToString[aDir] << ", (" << iCurrentPos.first << "," << iCurrentPos.second << ")" << ", (" << getNextPos(aDir, iCurrentPos).first << "," <<getNextPos(aDir, iCurrentPos).second << ")" << endl;
        aDir = static_cast<DIR>((aDir+1)%4) ;
        if(aDir == forbiddenDir)
        {
        	aDir = static_cast<DIR>((aDir+1)%4);
		}
        nbTurns++;
	}
	dead = nbTurns == 3;
    return aDir;
}

void cleanMapPredictions(){
	for(int i=0;i<maxX; i++)
		for(int j=0;j<maxY; j++)
			if(gMap[i][j] == kEnemyPredictVal)
				gMap[i][j] = 0;
}

void initLastEnemyPos(int iPlayerCount){
	for(int i=0;i< iPlayerCount; i++)
		lastEnemyPos[i]=make_pair(-1, -1);
}

bool validEnemy(int iX, int iY){
	return (iX != -1 && iY != -1);
}

DIR getEnemyDirection(int iID, pair<int, int>& iCurrentPos){
	DIR enemyDir;
	if(iCurrentPos.first - lastEnemyPos[iID].first)
		enemyDir = (iCurrentPos.first - lastEnemyPos[iID].first >= 1 ) ? RIGHT:LEFT;
	else
		enemyDir = (iCurrentPos.second - lastEnemyPos[iID].second >= 1 ) ? DOWN:UP;
	return enemyDir;
}

void predictEnemyMove(int iID, pair<int, int>& iCurrentPos){
	pair<int, int> nextMove{0,0};
	DIR enemyDir = getEnemyDirection(iID, iCurrentPos);

	//nextMove = getNextPos(nextDir, iCurrentPos);
	bool dead;
	nextMove = computeDirection(enemyDir, iCurrentPos, dead, gMap);
	if(gMap[nextMove.first][nextMove.second] == 0)
	gMap[nextMove.first][nextMove.second]= kEnemyPredictVal;

}

// in how many turn will we loose
int computeMaxTurnBeforeDead(DIR iDir)
{
	if (detectColision(iDir, gMyPos, gMap))
		return 0;
	bool dead = false;
	auto aPredMap = gMap;
	auto aPredPos = gMyPos;
	int aTurns = 0;
	while(!dead && aTurns < MAX_TURNS)
	{
    	iDir = computeDirection(iDir, aPredPos, dead, aPredMap);
    	
    	aPredPos = getNextPos(iDir, aPredPos);
    	aPredMap[aPredPos.first][aPredPos.second] = 10;
    	aTurns++;
    }
    return aTurns;
}

void firstTurnEnemyPrediction(pair<int, int> iPosition){
	if (gMap[getNextPos(UP, iPosition).first][getNextPos(UP, iPosition).second] == 0)
		gMap[getNextPos(UP, iPosition).first][getNextPos(UP, iPosition).second] = kEnemyPredictVal;
	if (gMap[getNextPos(DOWN, iPosition).first][getNextPos(UP, iPosition).second] == 0)
		gMap[getNextPos(DOWN, iPosition).first][getNextPos(UP, iPosition).second] = kEnemyPredictVal;
	if (gMap[getNextPos(LEFT, iPosition).first][getNextPos(UP, iPosition).second] == 0)
		gMap[getNextPos(LEFT, iPosition).first][getNextPos(UP, iPosition).second] = kEnemyPredictVal;
	if (gMap[getNextPos(RIGHT, iPosition).first][getNextPos(UP, iPosition).second] == 0)
		gMap[getNextPos(RIGHT, iPosition).first][getNextPos(UP, iPosition).second] = kEnemyPredictVal;
}

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
int main()
{
    int playerCount;
    cin >> playerCount; cin.ignore();
    initLastEnemyPos(playerCount);

    int myId;
    cin >> myId; cin.ignore();

	DIR aDirection = DOWN;
	bool firstTurn = true;

    // game loop
    while (1) {
    	int helperBots;
    	cin >> helperBots; cin.ignore();

		cleanMapPredictions();

        // read player trails
        for (int i = 0; i < playerCount; i++) {
            int x;
            int y;
            cin >> x >> y; cin.ignore();
            if (!validEnemy(x, y))
            	continue;
            pair<int, int> aPosition = make_pair(x, y);
            gMap[x][y] = i+1;

            if(myId == i)
            {
            	gMyPos = aPosition;
			}
            // try to predict next enemy position
            else if (validEnemy(lastEnemyPos[i].first, lastEnemyPos[i].second))
            {
            	predictEnemyMove(i, aPosition);

            } else if (firstTurn)
            {
            	firstTurnEnemyPrediction(aPosition);
			}
            lastEnemyPos[i]=aPosition;
        }

        // deploy helper bots
        int removalCount;
        cin >> removalCount; cin.ignore();
        for (int i = 0; i < removalCount; i++) {
            int removeX;
            int removeY;
            cin >> removeX >> removeY; cin.ignore();
            gMap[removeX][removeY] = 0;
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;
        
        //bool dead = false;
        //aDirection = computeDirection(aDirection, gMyPos, dead, gMap);
        
        map<DIR, int> aMaxTurnMap = {{DOWN,0}, {UP,0}, {RIGHT,0}, {LEFT,0}};
        for(auto& aDirMap: aMaxTurnMap)
        {
        	aDirMap.second = computeMaxTurnBeforeDead(aDirMap.first);
        	cerr << "Direction " << dirToString[aDirMap.first] << " will be loosing in " << aDirMap.second << " turns !" << endl;
		}
        cerr << "Loosing in " << computeMaxTurnBeforeDead(aDirection) << " turns !" << endl;
        
		int maxValue = 0;      
        for(auto& aDirMap: aMaxTurnMap)
        {
			if(maxValue <=  aDirMap.second)
			{
				aDirection = aDirMap.first;
				maxValue = aDirMap.second;
			}	
        }
        
        if (maxValue == 0)
			cout << "DEPLOY" << endl;
		else
        	cout << dirToString[aDirection] << endl;
        	
        firstTurn = false;
    }
}
