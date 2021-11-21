#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED


float PIController(float Vref, float V_measured, float Ki, float Kp, float Ts, float min, float max);
float converterModel(float u_in);

#endif
