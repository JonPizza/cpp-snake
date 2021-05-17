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

    float getRand() {
        float f = (float) rand() / 1073741823.5;
        return f - 1;
    }

    float sigmoid(float x) {
        return 1 / (1 + pow(e, x * -1));
    }

    float biasedSigmoid(float x) {
        return 1 / (1 + sigmoidBias + pow(e, x * -1));
    }

    public:

    void init() {
        srand(time(0));
        for (int i = 0; i < L1 * L2; i++) {
            if (i < L2 * L3)
                weights2[i] = getRand(); 
            weights1[i] = getRand(); 
        }

        sigmoidBias = getRand() / 10.0;
    }

    void mutate(int mutationLevel) {
        for (int i = 0; i < L1 * L2; i++) {
            weights1[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2 * L3; i++) {
            weights2[i] += getRand() / (float) mutationLevel;
        }

        sigmoidBias += getRand() / (float) mutationLevel / 10.0;
    }

    int feedForward(float inputs[L1]) {
        for (int i = 0; i < L2; i++) {
            t = 0;
            for (int j = 0; j < L1; j++) {
                t += weights1[(i + 1) * j] * inputs[j];
            }
            layer2[i] = biasedSigmoid(t);
        }

        for (int i = 0; i < L3; i++) {
            t = 0;
            for (int j = 0; j < L2; j++) {
                t += weights2[(i + 1) * j] * layer2[j];
            }
            layer3[i] = biasedSigmoid(t);
        }

        max = 0;
        for (int i = 0; i < L3; i++) {
            printf("layer3[%d] -> %f\n", i, layer3[i]);
            if (layer3[i] > layer3[max]) {
                max = i;
            }
        }

        return max;
    }
};

// int main() {
//     float inputs[8] = {1, 2, 3, 4, 5, 6, 7, 8};
//     NeuralNet nn;
//     nn.init();
//     printf("Output -> %d\n", nn.feedForward(inputs));
//     nn.mutate(2);
//     printf("Mutated by 2\n");
//     printf("Output -> %d\n", nn.feedForward(inputs));
//     return 0;
// }