#ifndef _MM1_GUARD
#define _MM1_GUARD

#include<bits/stdc++.h>
#include "constants.h"
using namespace std;

class MM1
{
    public:
        // This struct will store timing information for each migrant
        struct Timing
        {
            double x, y, z;
        };
        queue<double> times;
        queue<double> getTime();
        MM1();
    
};

MM1::MM1()
{}

queue<double> MM1::getTime(){

    double lambda_1 = 0.1;
    double mu_1 = 0.06;


    // Exponential distribution generation
    std::random_device rd; 
    std::mt19937 rnd_gen (rd ());

    exponential_distribution<> lambda_generator_1 (lambda_1);
    exponential_distribution<> lambda_generator = lambda_generator_1;

    exponential_distribution<> mu_generator_1 (mu_1);
    exponential_distribution<> mu_generator = mu_generator_1;

    


    double current_time = 0;
    long long completed_migrants = 0;

    // Next time when the migrants will come
    double next_incoming_timing = current_time + lambda_generator(rnd_gen);
    // States which tell whether the officer is working or not
    int processing_state = 0;
    // Time when the current servicing will be completed
    double processing_complete_timing = current_time;

    long long total_migrants = NUM_PACKETS*INITIAL_PEERS;

    // Queue which will hold the migrants
    queue<Timing> waiting_q = queue<Timing> ();

    // Timing values of migrants which are currently getting serviced
    Timing currently_processing = {0, 0, 0};

    // Run the loop till we have not serviced "total_migrants"
    while(completed_migrants < total_migrants)
    {
        // First check which event will happen first(next incoming or next completion of service)
        vector<double> times;

        times.push_back(next_incoming_timing);
        
        if (processing_state == 1){
            times.push_back(processing_complete_timing);
        }
        else{
            times.push_back(DBL_MAX);
        }

        if(times[0]<times[1]){
            // We have an incoming migrant
            current_time = next_incoming_timing;

            if(processing_state == 0){  // The server is empty, directly give the migrant to server
                currently_processing = {current_time, current_time, 0};
                processing_state = 1;
                processing_complete_timing = current_time + mu_generator(rnd_gen); // Calculate the next completion time
            }
            else{  // The server is busy,  deposit the migrant to the queue
                // count++;
                waiting_q.push({current_time, 0, 0});
            }

            next_incoming_timing = current_time + lambda_generator(rnd_gen); // calculate the next incoming time
        }
        else{
            //A migrant has been serviced, we have an outgoing migrant, complete the timing tuple and do calculations
            current_time = processing_complete_timing;
            currently_processing.z = current_time;
            
            completed_migrants++;
            this->times.push(currently_processing.z - currently_processing.x);
            if(completed_migrants == total_migrants){
                break;
            }

            if(waiting_q.empty()){  // Queue is empty change the processing state to 1
                processing_state = 0;
            }
            else{  // Pop from the queue
                currently_processing = waiting_q.front();
                waiting_q.pop();
                currently_processing.y = current_time;
                processing_complete_timing = current_time + mu_generator(rnd_gen);
            }


        }

    }

    return times;

}
#endif
