#include <system.h>

double sqroot(double square)
{
    double root=square/3, last, diff=1;
    if (square <= 0) return 0;
    do {
        last = root;
        root = (root + square / root) / 2;
        diff = root - last;
    } while (diff > MINDIFF || diff < -MINDIFF);
    return root;
}


//makes code slightly neater
double sqr(double root)
{
	return root*root;
}
