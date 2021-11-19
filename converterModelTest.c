#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define X_SIZE 6

typedef struct PIctrl {
    float kp;
    float ki;

    float maxValue;
    float minValue;

} PIctrl;


static volatile float states = 0.0;
static volatile float oldStates[X_SIZE] = {0.0};
static volatile float i_memory = 0.0;


float model(float u){
    static const float A[X_SIZE] = {0.2261, 0.6421, 0.271, 0.03519, 0.2018, 0.3215};

    static const float B = 0.0539;

    states = 0.0;
    for(int i = 0; i<X_SIZE; i++){
        
            states = (states + A[i]*oldStates[i])*u + B;
            oldStates[i] = states;
        }
        

    return states;

}

void init_pi(PIctrl *pi){
    // experimental values
    pi->ki = 0.0;
    pi->kp = 0.0;
    pi->maxValue = 100.0;
    pi->minValue = 0.0;
}

float pi(float measurement, float voltage, float samplingTime, float min, float max, float kp, float ki){
   
   clock_t s = clock();
   clock_t now = clock();

   // cannot used in board!
   while(((now - s)/CLOCKS_PER_SEC) > samplingTime ){
       now = clock();
       
   }

   float err = voltage-measurement;
   float pVal = kp * err;
   float iVal = ki * samplingTime;

    if((pVal+iVal)>max) return max;
    if((pVal+iVal)<min) return min;

   return pVal + iVal;

}

int main(int argc, char *argv[]){
    
    float voltage = atof(argv[1]);
    PIctrl pictrl;
    init_pi(&pictrl);

    clock_t s = clock();
    float result = pi(model(voltage), voltage, 0.5, pictrl.minValue, pictrl.maxValue, pictrl.kp, pictrl.ki);
    while(result != voltage){
        result = pi(model(voltage), voltage, 0.5, pictrl.minValue, pictrl.maxValue, pictrl.kp, pictrl.ki);
        printf("%f\n", result);
    }
    clock_t now = clock();
    printf("Stable after %f\n", (float)(now-s)/CLOCKS_PER_SEC);

    

    return 0;
}


