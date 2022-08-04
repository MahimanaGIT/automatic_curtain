/**
 * @file alexa_interaction.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for class which will be interacting with alexa API
 * @version 0.1
 * @date 2022-07-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _ALEXA_INT_INCLUDE_GUARD
#define _ALEXA_INT_INCLUDE_GUARD

class AlexaInteraction {
public:
  /**
   * @brief Construct a new AlexaInteraction object, will receive the function
   * to be called
   *
   */
  AlexaInteraction(void (*)(bool, int));

  /**
   * @brief Destroy the AlexaInteraction object
   *
   */
  ~AlexaInteraction();
};

#endif
