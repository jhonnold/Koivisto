//
// Created by Luecx on 21.11.2020.
//
#include "Network.h"

#ifdef NN_TRAIN
void nn::Network::compute(Sample* sample, int threadID) {
    
    // does the entire forward pass on a given sample using
    // the outputs/activations for the coresponding thread
    
    // firstly, the apply the affine transormation for the input
    affine_transformation_input(sample, weights[0], biases[0], outputs[0][threadID]);
    
    // next, we need to apply the activation for the first layer to make it non-linear
    this->layers[0].activation(outputs[0][threadID], activations[0][threadID]);
    
    // we need to go through all other layers and apply the affine transformation and the activation.
    for(int i = 1; i < LAYER_COUNT; i++){
        affine_transformation(weights[i], biases[i], activations[i-1][threadID], outputs[i][threadID]);
        this->layers[i].activation(outputs[i][threadID], activations[i][threadID]);
    }
}
void nn::Network::backprop(Sample* sample, int threadID) {
    // applies automatic differentiation on the network starting at
    // the end and going to the beginning of the network
    // note that we do not include the first layer in the for-loop as its heavily depending on the
    // network structure -> split into a new method
    for(int i = LAYER_COUNT-1; i > 0; i--){
        this->layers[i].backprop(activations[i][threadID], outputs[i][threadID]);
        affine_transformation_backprop(weights[i], biases[i], activations[i-1][threadID], outputs[i][threadID], threadID);
    }
    // applying backprop to the input layer
    this->layers[0].backprop(activations[0][threadID], outputs[0][threadID]);
    affine_transformation_input_backprop(sample, weights[0], biases[0], outputs[0][threadID], threadID);
}

float nn::Network::getOutput(int threadID, int id) {
    // returns the output of the network
    // this is the same as the activation at the last layer (LAYER_COUNT-1)
    return this->activations[LAYER_COUNT-1][threadID]->values[id];
}

float nn::Network::applyLoss(Data* target, int threadID) {
    // computes the loss at the output using the lossFunction defined in structure.h
    // beside computing the loss, it also generates the gradients at the output nodes
    return lossFunction(activations[LAYER_COUNT-1][threadID], target);
}

void nn::Network::mergeGrad() {
    // merges the gradients of all the weights/biases into the first gradient entry
    // it does not delete the other gradients so optimally, clearGrad() should be called afterwards
    for(int i = 0; i < LAYER_COUNT; i++){
        for(int t = 1; t < NN_THREADS; t++){
            weights[i]->getGradient(t)->mergeInto(weights[i]->getGradient(0));
            biases [i]->getGradient(t)->mergeInto(biases [i]->getGradient(0));
        }
    }
}
void nn::Network::clearGrad() {
    // deletes the gradients for all weights/biases for all threads
    for (int i = 0; i < LAYER_COUNT; i++) {
        for (int t = 0; t < NN_THREADS; t++) {
            weights[i]->getGradient(t)->clear();
            biases [i]->getGradient(t)->clear();
        }
    }
}
void nn::Network::optimise() {
    // we assume that there are never more than one networks active while training
    optimiser.optimise(LAYER_COUNT, weights, biases);
}

#else
void nn::Network::compute(Sample* sample) {
    affine_transformation_input(sample, weights[0], biases[0], outputs[0]);
    this->layers[0].activation(outputs[0], activations[0]);
    for (int i = 1; i < LAYER_COUNT; i++) {
        affine_transformation(weights[i], biases[i], activations[i - 1], outputs[i]);
        this->layers[i].activation(outputs[i], activations[i]);
    }
}
#endif
