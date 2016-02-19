/**
 * A set of temperature functions to convert volts to temp and filter these results
 */

//Private Structs
typedef struct StateVars{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_state;

//Private global
kalman_state kstate;

/**
 * Converts a voltage to Degrees celcius
 * @param  voltage voltage value
 * @return         celcius conversion of voltage
 */
float convertVtoC(int voltage){
	float convert = (float)voltage;
	convert *= 3000; //MV max voltage
	convert /= 0xfff; //mv
	convert *= 0.001f; //volt
	convert -= 0.770f; //Ref voltage for temp
	convert *= 400.0f; //slope
	convert += 25.0f; //offset

	return convert;
}

/**
 * Set the initial values for the kalman filter
 * @param q 
 * @param r 
 * @param x 
 * @param p 
 * @param k 
 */
void k_filter_init(float q, float r, float x, float p, float k){
    kstate.q = q;
    kstate.r = r;
    kstate.x = x;
    kstate.p = p;
    kstate.k = k;
}

/**
 * Filter one measurement and update the kalman state
 * @param  measurement measurement to be filtered
 * @return             filtered value
 */
float k_filter_value(float measurement){
	kstate.p = kstate.p + kstate.q;
	kstate.k = kstate.p  / (kstate.p + kstate.r);
	kstate.x = kstate.x + kstate.k * ( measurement - kstate.x);
	kstate.p = (1 - kstate.k) * kstate.p;
	return kstate.x;
}
