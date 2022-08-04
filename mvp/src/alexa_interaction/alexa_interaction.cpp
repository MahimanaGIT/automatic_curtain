/**
 * @file alexa_interaction.cpp
 * @author Dhiraj Deshmukh (deshmukhdhiraj15@gmail.com)
 * @brief Contains algorithms for handling alexa requests and make appropriate
 * call at every request
 * @version 0.1
 * @date 2022-07-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "alexa_interaction.h"

AlexaInteraction::AlexaInteraction(void (*callback)(bool, int)) {
  callback(0, 0);
  return;
}