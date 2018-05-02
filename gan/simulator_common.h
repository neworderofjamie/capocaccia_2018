#pragma once


void setExcitatoryWeight(unsigned int preIdx, unsigned int postIdx, float weight);
void setInhibitoryWeight(unsigned int preIdx, unsigned int postIdx, float weight);
void setBlueInput(float value);
void setRedInput(float value);

void initConnectivity();