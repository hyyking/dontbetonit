#include <vector>
#include <iostream>
#include <math.h>


using namespace std;

struct Event
{
    double proba;
    
    //all proba associated to events in order
    vector<double> subproba;
};

class Partition
{
private:
    int index;
    int size;

public:    
    double proba;
    unsigned int table;

    //OR operator

    Partition(int, int, double);
    unsigned int operator|(Partition& other);


};

Partition::Partition(int size, int index, double proba)
: size(size), index(index), proba(proba)
{
    if (size >= index)
    {
        table = (unsigned int) (pow(2, size) - 1) << size*(index - 1);
    } else
    {
        cout << "Couldn't create table for class: " << index << endl;
    }

}



unsigned int Partition::operator|(Partition& other)
{
    return table | other.table;
}


