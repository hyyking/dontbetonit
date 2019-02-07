#include <vector>
#include <iostream>
#include <math.h>


using namespace std;

struct Event
{
    double proba;
    
    //omega
    unsigned int alpha_info;
    unsigned int beta_info;
    
    //all proba associated to events in order
    vector<double>* subproba;
};

enum class Players
{
    Alpha,
    Beta,
};


class Partition
{
private:
    Players type;
    int index; 
    int size;

public:
    double proba;
    unsigned long long table;

    Partition(Players, int, int, double);
    void make_table();
    void update_si(int, int);
};

Partition::Partition(Players type, int size, int index, double proba)
: type(type), size(size), index(index), proba(proba)
{
}


void Partition::make_table()
{
    switch (type)
    {
        case Players::Alpha:
            if (size >= index) 
            { 
                table = (unsigned int) (pow(2, size) - 1) << size*(index - 1); 
            } 
            else
            { 
                cout << "Couldn't create table for class: " << index << endl; 
            }        
            break;
        
        case Players::Beta:
            if (size >= index) 
            { 
                table = pow(2, size - 1);
                for (int i = 0; i < size-1; i++)
                {
                    unsigned int buffer = table;
                    table <<= size;
                    table |= buffer;
                }
                table = table >> index-1;
            } 
            else
            { 
                cout << "Couldn't create table for class: " << index << endl; 
            }
            
            break;
    }
}
/*
unsigned int Partition::operator|(Partition& other)
{
    return table | other.table;
}
*/

void Partition::update_si(int new_size, int new_index)
{
    size = new_size;
    index = new_index;
}