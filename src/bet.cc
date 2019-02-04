#include "bet.h"


int main()
{
    Partition* alphaone = new Partition(4, 1, 0.2);
    Partition* alphatwo = new Partition(4, 2, 0.8);
    Partition* alphathree = new Partition(4, 3, 0);
    Partition* alphafour = new Partition(4, 4, 0);

    cout << alphaone->table << endl;
    cout << alphatwo->table << endl;
    cout << alphathree->table << endl;
    cout << alphafour->table << endl;
    return 0;
}
