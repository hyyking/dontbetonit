#include <vector>
using namespace std;

#define PSIZE 3     //number of partitions per player
#define PAMOUNT 2   //number of players

bool empty[PSIZE][PSIZE] = {};

struct Partition
{
    double proba;
    bool posmatrix[PSIZE][PSIZE];
};

struct Players //Alpha & Beta
{
    Partition* alpha[PSIZE];
    Partition* beta[PSIZE];
};

enum Player
{
    Alpha,
    Beta
};

struct Event
{
    double proba;
    double partmatrix[PSIZE][PSIZE];
};


struct chainedState
{
    int index;
    double decided;
    Player player;

    Partition* alphastate[PSIZE];
    Partition* betastate[PSIZE];
    chainedState* next = NULL;
};


/* Function prototypes */

void inter(bool output[PSIZE][PSIZE], bool p[PSIZE][PSIZE]);
void uni(bool output[PSIZE][PSIZE], bool p[PSIZE][PSIZE]);

int autoFree(chainedState*);
void stateHistory(chainedState*, Event, bool empty[PSIZE][PSIZE]);

void printTable(bool table[PSIZE][PSIZE]);