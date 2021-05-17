#ifndef NEURALNET_H
#define NEURALNET_H

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <math.h>

using namespace std;

#define L1 8
#define L2 10
#define L3 4

class NeuralNet {
    private:

    float layer2[L2];
    float layer3[L3];
    float weights1[L1 * L2];
    float weights2[L2 * L3];
    float sigmoidBias;

    int max;
    float t;
    float e = 2.718281828459045;

    float getRand();
    float sigmoid(float x);
    float biasedSigmoid(float x);

    public:

    void init();
    void mutate(int mutationLevel);
    int feedForward(float inputs[L1]);
};

#endif