#include "bet.h"

void join(Partition**, unsigned int &size);
void create_tables(Partition**, unsigned int);

int main()
{
        
    unsigned int ALPHASIZE = 4; //line
    unsigned int BETASIZE  = 5; //column

    unsigned int alpha_info = 3;
    unsigned int beta_info = 2;

    Event* event = (Event*) malloc(sizeof(Event));
    vector<double> event_description = {
        0.125, 0.125, 0.125, 0.125,
        0.125, 0.125, 0.125, 0.125,
    };
    event->proba = 0.6;
    event->subproba = &event_description;

    Partition* Alpha[ALPHASIZE] = {
        new Partition(Players::Alpha, ALPHASIZE, 1, 0.2),
        new Partition(Players::Alpha, ALPHASIZE, 2, 0.8),
        new Partition(Players::Alpha, ALPHASIZE, 3, 0),
        new Partition(Players::Alpha, ALPHASIZE, 4, 0)
    };

    Partition* Beta[BETASIZE] = {
        new Partition(Players::Beta, BETASIZE, 1, 0.1), //same type as ALPHA 
        new Partition(Players::Beta, BETASIZE, 2, 0.2),
        new Partition(Players::Beta, BETASIZE, 3, 0.3),
        new Partition(Players::Beta, BETASIZE, 4, 0.4),
        new Partition(Players::Beta, BETASIZE, 5, 0)
    };

    join(Alpha, ALPHASIZE);
    join(Beta, BETASIZE);

    create_tables(Alpha, ALPHASIZE);
    create_tables(Beta, BETASIZE);
    /* cout << Beta[0]->table << endl; */

    unsigned int alphau = 0;
    unsigned int betau = 0;

    for (int i = 0; i < ALPHASIZE; i++)
        if (Alpha[i] != NULL)
            alphau = alphau | Alpha[i]->table;    
        
    for (int i = 0; i < BETASIZE; i++)
        if (Beta[i] != NULL)
            betau = betau | Beta[i]->table;


    
    //column check
    for (int o = 0; o < BETASIZE; o++)
    {
        double mid_res = 0;
        for (int i = 0; i < ALPHASIZE; i++)
        {   
            unsigned int bit_check = (unsigned int) pow(2, i*BETASIZE) << BETASIZE - 1 - o;
            if ((bit_check & betau) > 0)
            {
                mid_res += (*(event->subproba))[o + i*BETASIZE];
            }
        }    
        cout << 3-o << ": " << mid_res << endl;
    }   
    

    free(event);
    return 0;
}


void join(Partition** part, unsigned int &size)
{
    unsigned int size_copy = size;
    for (int n = 0; n < size_copy; n++)
    {   
        if (part[n]->proba == 0)
        {
            part[n] = NULL;
            size--;
        }    
    }
}

void create_tables(Partition** part, unsigned int size)
{
    for (int n = 0; n < size; n++)
    {   
        part[n]->size = size;
        part[n]->make_table();    
    }
}