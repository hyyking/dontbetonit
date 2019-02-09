#include "bet.h"

void join(Partition**, unsigned int &size, const unsigned int);
void create_tables(Partition**, unsigned int, unsigned int, const unsigned int);

double* beta_logic(const uint64_t, Event*, const unsigned int, const unsigned int);
double* alpha_logic(const uint64_t, Event*, const unsigned int, const unsigned int);

double bayes_theorem(const double BsA, const double A, const double B); //  P(A|B) = (P(B|A)*P(A))/P(B)

const unsigned int ALPHA_T = 3; //(doesn't mean effective sizes, just describes how big the line table will be)
const unsigned int BETA_T = 3;  //(doesn't mean effective sizes, just describes how big the column table will be)


int main()
{
    unsigned int ALPHASIZE = ALPHA_T; //lines     (effective sizes, will be modified dureing execution)
    unsigned int BETASIZE  = BETA_T; //columns   (effective sizes, will be modified during execution)

    //Describing the event
    Event* event = (Event*) malloc(sizeof(Event));
    std::vector<double> event_description = {
            0.067, 0.500, 0.117,
            0.100, 0.108, 0.083, 
            0.017, 0.008, 0.000, 
    };
    event->proba = 0.6;
    event->subproba = &event_description;
    event->alpha_info = 1;
    event->beta_info = 1;
    
    Partition* Alpha[ALPHA_T] = {                         //creating new partitions for player Alpha
        new Partition(Players::Alpha, ALPHA_T, 1, 0.5),
        new Partition(Players::Alpha, ALPHA_T, 2, 0.33),
        new Partition(Players::Alpha, ALPHA_T, 3, 0.17),
    };

    Partition* Beta[BETA_T] = {                           //creating new partitions for player BETA
        new Partition(Players::Beta, BETA_T, 1, 0.4),
        new Partition(Players::Beta, BETA_T, 2, 0.4),
        new Partition(Players::Beta, BETA_T, 3, 0.2),
    };

    //removing useless partitions
    join(Alpha, ALPHASIZE, ALPHA_T); 
    join(Beta, BETASIZE, ALPHA_T);

    //updating Partion arrays (size), creating a bit table for each partitions and moving nullptr elements at the back of the array
    create_tables(Alpha, ALPHASIZE, BETASIZE, ALPHA_T);
    create_tables(Beta, BETASIZE, ALPHASIZE, BETA_T);



    bool alpha = true;
    bool beta = true;
    int turn_counter = 0;
    
    #define LIMIT 0.5
    do
    {
        std::cout << "\n Turn nb: " << turn_counter << std::endl;
        uint64_t alphau = 0;
        uint64_t betau = 0;
        
        //union of player partitions
        for (int i = 0; i < ALPHASIZE; i++)         //alpha
            alphau = alphau | Alpha[i]->table;            
        for (int i = 0; i < BETASIZE; i++)          //beta
            betau = betau | Beta[i]->table;

        //intersection of alpha and beta partitions (represents available space)
        uint64_t intersection = alphau & betau;

        double* alpha_rest = alpha_logic(intersection, event, ALPHASIZE, BETASIZE);
        double* beta_rest = beta_logic(intersection, event, ALPHASIZE, BETASIZE);

        std::cout << "alpha_rest[i]" << std::endl;
        for(int i = 0; i < ALPHASIZE; i++)
        {
            alpha_rest[i] = bayes_theorem(alpha_rest[i], event->proba, Alpha[i]->proba);
            std::cout << Alpha[i]->index << " | " << Alpha[i]->table << ": " << alpha_rest[i] << std::endl;
        }
        
        std::cout << "beta_rest[i]" << std::endl;
        for(int i = 0; i < BETASIZE; i++)
        {   
            beta_rest[i] = bayes_theorem(beta_rest[i], event->proba, Beta[i]->proba);
            std::cout << Beta[i]->index << " | " << Beta[i]->table << ": " << beta_rest[i] << std::endl;
        }
            
        if (alpha_rest[event->alpha_info] < LIMIT)
            alpha = false;
        
        if (beta_rest[event->beta_info] > LIMIT)
            beta = false;
        
        if (turn_counter % 2 == 0)
        {
            for (int a = 0; a < ALPHASIZE; a++)
                if (alpha_rest[a] < LIMIT)
                {
                    Alpha[a]->table = 0;
                    Alpha[a]->proba = 0;
                }
        } else
        {
            for (int b = 0; b < BETASIZE; b++)
                if (beta_rest[b] > LIMIT)
                {
                    Beta[b]->table = 0;
                    Beta[b]->proba = 0;
                }
        }           
        turn_counter++;
        
        free(alpha_rest);
        free(beta_rest);
    } while (alpha || beta);
    
    


    //deleting objects
    for (int i = 0; i < BETASIZE; i++)
        delete Beta[i];

    for (int i = 0; i < ALPHASIZE; i++)
        delete Alpha[i];
    
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
                mid_res += (*(event->subproba))[o*beta_s + i];                                //          if it is read the event description and add it to the column result
            }
        }    
        alphares[o] = mid_res;                                                                   //  update the result table with the column table
    }
    return alphares;
}

double bayes_theorem(const double BsA, const double A, const double B)
{
    if (B > 0)
        return (BsA*A)/B;
    else
        return 0;
}