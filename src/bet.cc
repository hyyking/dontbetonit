#include <iostream>

#include "bet.h"
#include <tuple>

int main()
{
    // Partition Alpha     || Partition Beta    
    Partition p1 = {0.5, 
        {
            {true, true, true},
            {false, false, false},
            {false, false, false}
        }
    }; 
    
    Partition p2 = {0.33,
        {
            {false, false ,false},
            {true, true, true},
            {false, false, false}
        }
    };

    Partition p3 = {0.17,
        {
            {false, false ,false},
            {false, false, false},
            {true, true, true}
        }
    };

    Partition q1 = {0.4,
        {
            {true, false ,false},
            {true, false, false},
            {true, false, false}
        }
    };

    Partition q2 = {0.4,
        {
            {false, true, false},
            {false, true, false},
            {false, true, false}
        }
    };

    Partition q3 = {0.2,
        {
            {false, false, true},
            {false, false, true},
            {false, false, true}
        }
    };


    Event event = {0.6, 
        {   
            {0.067, 0.500, 0.117},
            {0.100, 0.108, 0.083}, 
            {0.017, 0.008, 0.000}, 
        }
    };

    
    tuple<int, int> omega (1, 1); 


    //loop logic
    bool alpha = true;
    bool beta = true;
    int g_index = 0;
    
    chainedState* initial = (chainedState*) malloc(sizeof(chainedState));
    initial->index = 0;
    (initial->alphastate)[0] = &p1; (initial->betastate)[0] = &q1;
    (initial->alphastate)[1] = &p2; (initial->betastate)[1] = &q2;
    (initial->alphastate)[2] = &p3; (initial->betastate)[2] = &q3;
    
    chainedState* upmost = initial;
    bool T[PSIZE][PSIZE] = {{true, true, true},{true, true, true},{true, true, true}};

    stateHistory(initial, event, T);

    do
    {
        chainedState* last = (chainedState*) malloc(sizeof(chainedState));
        last->index = ++g_index;
        
        //copy last state
        {
            (last->alphastate)[0] = (upmost->alphastate)[0];  
            (last->alphastate)[1] = (upmost->alphastate)[1];  
            (last->alphastate)[2] = (upmost->alphastate)[2]; 

            (last->betastate)[0] = (upmost->betastate)[0];
            (last->betastate)[1] = (upmost->betastate)[1];
            (last->betastate)[2] = (upmost->betastate)[2];
        }   
        
        if (g_index % 2 == 0)
        {
            last->player = Player::Alpha;
            bool emptyb[PSIZE][PSIZE] = {};
            bool emptya[PSIZE][PSIZE] = {};

            for (int a = 0; a < PSIZE; a++) // U all beta partitions
                if ((last->betastate)[a] != NULL)
                    uni(emptya, (last->betastate)[a]->posmatrix);
            
            for (int a = 0; a < PSIZE; a++) // U all alpha partitions
                if ((last->alphastate)[a] != NULL)
                    uni(emptyb, (last->alphastate)[a]->posmatrix); 

            inter(emptya, emptyb); //alpha partitions n beta partitions
                    
            //checking for none usable partitions
            for (int i = 0; i < PSIZE; i++)
                if ((last->betastate)[i] != NULL)
                {
                    double probuff = 0;
                    for (int o = 0; o < PSIZE; o++)
                        if (emptya[o][i])
                            probuff  += (event.partmatrix[o][i]*event.proba)/(last->betastate)[i]->proba;
                        
                    if (probuff > BORDER)
                        (last->betastate)[i] = NULL;
                }
            
            //checking P(A|p(w))
            double buff= 0;
            for (int d = 0; d < PSIZE; d++)
                if (emptya[get<0>(omega)-1][d] && (last->betastate)[get<0>(omega)-1] != NULL)
                    buff  += (event.partmatrix[get<0>(omega)-1][d]*event.proba)/(last->alphastate)[get<0>(omega)-1]->proba;

            last->decided = buff;
            if (buff < BORDER)
                alpha = false;
            
            stateHistory(last, event, emptya);   
        } 
        else //BETA
        {
            last->player = Player::Beta;
            bool emptyb[PSIZE][PSIZE] = {};
            bool emptya[PSIZE][PSIZE] = {};

            for (int a = 0; a < PSIZE; a++) //U beta
                if ((last->betastate)[a] != NULL)
                    uni(emptya, (last->betastate)[a]->posmatrix);
            
            for (int a = 0; a < PSIZE; a++) //U alpha
                if ((last->alphastate)[a] != NULL)
                    uni(emptyb, (last->alphastate)[a]->posmatrix);

            inter(emptya, emptyb); //alpha n beta
                    

            for (int i = 0; i < PSIZE; i++)
                if ((last->alphastate)[i] != NULL)
                {
                    double probuff = 0;
                    for (int o = 0; o < PSIZE; o++)
                        if (emptya[i][o])
                            probuff += (event.partmatrix[i][o]*event.proba)/(last->alphastate)[i]->proba;
                    
                    if (probuff < BORDER)
                        (last->alphastate)[i] = NULL;
                }

            double buff= 0;
            for (int d = 0; d < PSIZE; d++)
                if (emptya[d][get<1>(omega)-1] && (last->alphastate)[get<0>(omega)-1])
                    buff  += (event.partmatrix[d][get<1>(omega)-1]*event.proba)/(last->betastate)[get<1>(omega)-1]->proba;

            last->decided = buff;
            if (buff > BORDER)
                beta = false;
            
            stateHistory(last, event, emptya);   
        }
        
        
        upmost->next = last;
        upmost = last;
         
    } while (alpha && beta);
    
    
    
    //FREE
    if (autoFree(initial) == 0)
    {
        return 0;
    } else 
    {
        return 1;
    }
}


void inter(bool output[PSIZE][PSIZE], bool p[PSIZE][PSIZE])
{
    for (int i = 0; i < PSIZE; i++)
        for (int o = 0; o < PSIZE; o++)
            if (p[i][o] && output[i][o]) {
                output[i][o] = true;
            }
            else {
                output[i][o] = false;
            }
}

void uni(bool output[PSIZE][PSIZE], bool p[PSIZE][PSIZE])
{
    for (int i = 0; i < PSIZE; i++)
        for (int o = 0; o < PSIZE; o++)
            if (p[i][o] || output[i][o]) {
                output[i][o] = true;
            }
            else {
                output[i][o] = false;
            }
}


int autoFree(chainedState* head) //start to end
{
    chainedState* current = head;
    while(current->next != NULL)
    {
        chainedState* buffer = current;
        current = current->next;
        
        
        free(buffer);
    }
    free(current);
    return 0;
}

void stateHistory(chainedState* current, Event event, bool empty[PSIZE][PSIZE])
{
    printTable(empty);
    cout << "|" << current->index << "|" << endl;
    
    switch (current->player)
    {
        case Alpha:
            cout << "\t" << "Player: Alpha" << endl;
            break;
        
        case Beta:
            cout << "\t" << "Player: Beta" << endl;
            break;
    }
    
    cout << "\tDecided: " << current->decided <<   endl;
    cout << "\t" << "Partitions:" << endl;
    cout << "\t\tAlpha" << endl;
    
    for (int i = 0; i < PSIZE; i++)
    {
        double res = 0;
        for (int z = 0; z < PSIZE; z++)
            if ((current->alphastate)[i] != NULL && empty[i][z])
                res += event.partmatrix[i][z] * event.proba / (current->alphastate)[i]->proba;        
        
        if ((current->alphastate)[i] != NULL)
            cout << "\t\t\t" << "P(A|p" << i+1 << ")"  << ": " << res << endl;
        else
            cout << "\t\t\t" << "P(A|p" << i+1 << ")" << ": NULL" << endl;
    }
    cout << "\t\tBeta" << endl;
    for (int i = 0; i < PSIZE; i++)
    {
        double res = 0;
        for (int z = 0; z < PSIZE; z++)
            if ((current->betastate)[i] != NULL && empty[z][i])
                res += event.partmatrix[z][i] * event.proba / (current->betastate)[i]->proba;        
        
        if ((current->betastate)[i] != NULL)
            cout << "\t\t\t" << "P(A|q" << i+1 << ")"  << ": " << res << endl;
        else
            cout << "\t\t\t" "P(A|q" << i+1 << ")" << ": NULL" << endl;
    }
}


void printTable(bool table[PSIZE][PSIZE])
{
    for (int i = 0; i < PSIZE; i++)
    {
        for (int o = 0; o < PSIZE; o++)
        {
            if (table[i][o]) {
                cout << "1";
            }
            else {
                cout << "0";
            }
        }
        cout << endl;
    }
}