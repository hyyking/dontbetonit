#include "bet.h"

void join(Partition**, unsigned int &size, const unsigned int);
void create_tables(Partition**, unsigned int, unsigned int, const unsigned int);

double* beta_logic(const uint64_t, Event*, const unsigned int, const unsigned int);
double* alpha_logic(const uint64_t, Event*, const unsigned int, const unsigned int);


const unsigned int ALPHA_T = 4; //(doesn't mean effective sizes, just describes how big the line table will be)
const unsigned int BETA_T = 5;  //(doesn't mean effective sizes, just describes how big the column table will be)


int main()
{
    unsigned int ALPHASIZE = ALPHA_T; //lines     (effective sizes, will be modified dureing execution)
    unsigned int BETASIZE  = BETA_T; //columns   (effective sizes, will be modified during execution)

    //Describing the event
    Event* event = (Event*) malloc(sizeof(Event));
    std::vector<double> event_description = {
        0.1, 0.1, 0.1, 0.1, 0.1,
        0.1, 0.1, 0.1, 0.1, 0.1,
    };
    event->proba = 0.6;
    event->subproba = &event_description;
    
    event->alpha_info = 3;
    event->beta_info = 2;
    

    Partition* Alpha[ALPHA_T] = {                         //creating new partitions for player Alpha
        new Partition(Players::Alpha, ALPHA_T, 1, 0.2),
        new Partition(Players::Alpha, ALPHA_T, 2, 0.8),
        new Partition(Players::Alpha, ALPHA_T, 3, 0),
        new Partition(Players::Alpha, ALPHA_T, 4, 0)
    };

    Partition* Beta[BETA_T] = {                           //creating new partitions for player BETA
        new Partition(Players::Beta, BETA_T, 1, 0.1),
        new Partition(Players::Beta, BETA_T, 2, 0.3),
        new Partition(Players::Beta, BETA_T, 3, 0.2),
        new Partition(Players::Beta, BETA_T, 4, 0.3),
        new Partition(Players::Beta, BETA_T, 5, 0.1)
    };

    //removing useless partitions
    join(Alpha, ALPHASIZE, ALPHA_T); 
    join(Beta, BETASIZE, ALPHA_T);

    //updating Partion arrays (size), creating a bit table for each partitions and moving nullptr elements at the back of the array
    create_tables(Alpha, ALPHASIZE, BETASIZE, ALPHA_T);
    create_tables(Beta, BETASIZE, ALPHASIZE, BETA_T);

    Alpha[0]->table = 0;
    Beta[0]->table = 0;

    //union of player partitions
    uint64_t alphau = 0;
    uint64_t betau = 0;
    for (int i = 0; i < ALPHASIZE; i++)         //alpha
        alphau = alphau | Alpha[i]->table;        
    
    for (int i = 0; i < BETASIZE; i++)          //beta
        betau = betau | Beta[i]->table;
    
    //intersection of alpha and beta partitions (represents available space)
    uint64_t intersection = alphau & betau;

    //reading columns of the intersection
    double* beta_rest = beta_logic(intersection, event, ALPHASIZE, BETASIZE);
    double* alpha_rest = alpha_logic(intersection, event, ALPHASIZE, BETASIZE);
    
    for (int i = 0; i < BETASIZE; i++)
        delete Beta[i];

    
    for (int i = 0; i < ALPHASIZE; i++)
        delete Alpha[i];
    
    
    free(beta_rest);
    free(alpha_rest);
    
    free(event);
    return 0;
}

//adjusting partition space by removing useless partitions and modifying partition size
void join(Partition** part, unsigned int &size, const unsigned int t_size)
{
    unsigned int size_copy = size;
    for (int n = 0; n < size_copy; n++)
    {   
        if (part[n] != nullptr && part[n]->proba == 0 && n !=0)
        {
            delete part[n];     //deleting object (not necessary because you could keep the object in memory)
            part[n] = nullptr;     //setting pointer to nullptr
            size--;             //decrementing partition size count
        } 
        
        else if (part[n] != nullptr && part[n]->proba == 0 && n == 0) //finding next none-null partition to serve as the table index
        { 
            int next_nn = 0;
            while (part[next_nn] != nullptr)
            {
                
                next_nn++;
                if (part[next_nn] != nullptr)
                {
                    Partition* buffer;
                    buffer = part[n];
                    part[n] = part[next_nn];
                    part[next_nn] = buffer;
                    delete part[next_nn];
                    part[next_nn] = nullptr;
                }
            }
            size--; 
        }
        
    }
}


//correcting partitions by updating size, table and moving null elements at the back of the array
void create_tables(Partition** part, unsigned int size, unsigned int o_size, const unsigned int t_size) 
{
    int z = 0;
    int null_index = -1;
    do //moving null partitions to the end of the table
    {
        if (null_index >= 0 && part[z] != nullptr)
        {
            part[null_index] = part[z];
            part[z] = nullptr;
            
            null_index = -1;
            z = 0;
        }

        if (null_index < 0 && part[z] == nullptr)
        {
            null_index = z;
        }
        z++;
    } while (z < t_size);
    
    for (int n = 0; n < size; n++)
    {
        if (size > o_size)
            part[n]->update_si(size, n+1);
        else
            part[n]->update_si(o_size, n+1);    //adjusting number of partitions before creating the table
        part[n]->make_table();                  //creating table with adjusted partition size  
    }
}

double* beta_logic(const uint64_t intersection, Event* event, const unsigned int alpha_s, const unsigned int beta_s)
{
    double* betares = (double*) malloc(sizeof(double) * beta_s);
    for (int o = 0; o < beta_s; o++)                                                          //for each column
    {
        double mid_res = 0;                                                                     //  create a new empty result
        for (int i = 0; i < alpha_s; i++)                                                     //  for each line
        {   
            uint64_t bit_check = (uint64_t) pow(2, i*beta_s) << beta_s - 1 - o;     //      create the checking bit
            if ((bit_check & intersection) > 0)                                                 //      check if the checking bit AND the intersection are on (checking bit will always be on)
            {
                mid_res += (*(event->subproba))[o + i*beta_s];                                //          if it is read the event description and add it to the column result
            }
        }    
        betares[o] = mid_res;                                                                   //  update the result table with the column table
    }
    return betares;
}

double* alpha_logic(const uint64_t intersection, Event* event, const unsigned int alpha_s, const unsigned int beta_s)
{
    double* alphares = (double*) malloc(sizeof(double) * alpha_s);
    for (int o = 0; o < alpha_s; o++)                                                          //for each column
    {
        double mid_res = 0;                                                                     //  create a new empty result
        for (int i = 0; i < beta_s; i++)                                                     //  for each line
        {   
            uint64_t bit_check = (uint64_t) pow(2, i) << o*beta_s;     //      create the checking bit
            if ((bit_check & intersection) > 0)                                                 //      check if the checking bit AND the intersection are on (checking bit will always be on)
            {
                mid_res += (*(event->subproba))[i];                                //          if it is read the event description and add it to the column result
            }
        }    
        alphares[o] = mid_res;                                                                   //  update the result table with the column table
    }
    return alphares;
}