/**
 * @file manual_interaction.h
 * @author Mahimana Bhatt (mahimanabhatt@gmail.com)
 * @brief Defines structure for manual interaction with device
 * @version 0.1
 * @date 2022-07-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _MAN_INT_INCLUDE_GUARD
#define _MAN_INT_INCLUDE_GUARD

#include "../config/config.h"

class ManualInteraction {
   public:
    /**
     * @brief Construct a new ManualInteraction object, initializes the push
     * button as inputs
     *
     */
    ManualInteraction();

    /**
     * @brief Destroy the ManualInteraction object
     *
     */
    ~ManualInteraction();

    /**
     * @brief This function detects various types of pushes, and returns the type
     * of push detected
     *
     */
    CONFIG_SET::MANUAL_PUSH detectPushes();
};

#endif
