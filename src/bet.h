#include <vector>
using namespace std;

#define PSIZE 3     //postulat que l'espace est carré
#define BORDER 0.5   //border win / lose knowing that alpha starts the bet

enum Player
{
    Alpha,
    Beta
};

struct Partition
{
    double proba;
    bool posmatrix[PSIZE][PSIZE];
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