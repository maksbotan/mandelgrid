
#include <stdio.h>
#include <math.h>

/*
#include <gmp.h>
#include <mpfr.h>

#define PREC 200

long test_function(double x0, double y0, long quality){
    mpfr_t x, y;
    mpfr_t t_x, t_y, t2_x;
    mpfr_t abs, four;
    long iteration;

    mpfr_init2(x, PREC);
    mpfr_init2(y, PREC);
    mpfr_init2(t_x, PREC);
    mpfr_init2(t2_x, PREC);
    mpfr_init2(t_y, PREC);
    mpfr_init2(abs, PREC);
    mpfr_init2(four, PREC);

    mpfr_set_d(x, x0, GMP_RNDD);
    mpfr_set_d(y, y0, GMP_RNDD);
    mpfr_set_ui(four, 4, GMP_RNDD); * Constant for comparing *

    iteration = 0;

    while (iteration < quality){
        mpfr_sqr(t_x, x, GMP_RNDU); * x^2 *
        mpfr_sqr(t_y, y, GMP_RNDU); * y^2 /
        
        * Do bail-out check *
        mpfr_add(abs, t_x, t_y, GMP_RNDD);
        if (mpfr_greater_p(abs, four)){
            break;
        }

        mpfr_sub(t_x, t_x, t_y, GMP_RNDD); * x^2-y^2 *
        mpfr_add_d(t_x, t_x, x0, GMP_RNDD); * x^2-y^2+x0 *
        
        mpfr_set(t2_x, t_x, GMP_RNDD); * Remember new 'x' (can't substitute now 'cause original value will be used for 'y' calculation *

        mpfr_mul(t_y, x, y, GMP_RNDU); * x*y *
        mpfr_mul_ui(t_y, t_y, 2, GMP_RNDU); * 2*x*y *
        mpfr_add_d(t_y, t_y, y0, GMP_RNDD); * 2*x*y+y0 *

        mpfr_set(x, t2_x, GMP_RNDD);
        mpfr_set(y, t_y, GMP_RNDD);

        iteration++;
    }

    mpfr_clears(x, y, t_x, t_y, abs, four, NULL);

    return iteration;
}
*/

long test_function(double x0, double y0, long quality){
    double x, new_x, y;
    double q;
    long iteration;

    x = x0;
    y = y0;
    iteration = 0;

    q = pow(x-0.25, 2)+pow(y,2);
    if (q*(q+(x-0.25))<0.25*pow(y,2)) {
        return quality;
    }

    if (pow(x+1, 2)+pow(y,2) < 1.0/16){
        return quality;
    }

    if (pow(x+1+5.0/16, 2) + pow(y, 2) < 1.0/256) {
        return quality;
    }

    while ((x*x + y*y <= 4) && (iteration < quality)) {
        new_x = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = new_x;
        
        iteration++;
    }
    
    return iteration;
}


int main(){
    printf("%ld\n", test_function(0, 0, 64));

    return 0;
}
