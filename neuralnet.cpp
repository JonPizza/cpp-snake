class NeuralNet {
    private:

    float layer2[L2];
    float layer3[L3];
    float weights1[L1 * L2];
    float weights2[L2 * L3];
    float bias1[L2];
    float bias2[L3];

    int max;
    float t;
    float e = 2.718281828459045;

    float getRand() {
        float f = (float) rand() / 1073741823.5;
        return f - 1;
    }

    float biasedSigmoid(float x, float sigmoidBias) {
        return 1 / (1 + pow(e, (x + sigmoidBias) * -1));
    }

    public:

    void init() {
        for (int i = 0; i < L1 * L2; i++) {
            if (i < L2 * L3)
                weights2[i] = getRand(); 
            if (i < L2)
                bias1[i] = getRand();
            if (i < L3)
                bias2[i] = getRand();
            weights1[i] = getRand(); 
        }
    }

    void mutate(int mutationLevel) {
        for (int i = 0; i < L1 * L2; i++) {
            weights1[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2 * L3; i++) {
            weights2[i] += getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L2; i++) {
            bias1[i] = getRand() / (float) mutationLevel;
        }

        for (int i = 0; i < L3; i++) {
            bias2[i] = getRand() / (float) mutationLevel;
        }
    }

    float* feedForward(float inputs[L1]) {
        for (int i = 0; i < L2; i++) {
            t = 0;
            for (int j = 0; j < L1; j++) {
                t += weights1[(i + 1) * j] * inputs[j];
            }
            layer2[i] = biasedSigmoid(t, bias1[i]);
        }
        
        for (int i = 0; i < L3; i++) {
            t = 0;
            for (int j = 0; j < L2; j++) {
                t += weights2[(i + 1) * j] * layer2[j];
            }
            layer3[i] = biasedSigmoid(t, bias2[i]);
        }

        return layer3;
    }
};
