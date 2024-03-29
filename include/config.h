/**
 * @file config.h
 * @author José Antonio Septién Hernández (ja.septienhernandez@ugto.mx)
 * @brief It holds configuration macros, so the stack can be changed according to 
 * the application in hand. It contains the global configurations for each of the
 * layers and each additional module. For any particular configuration, not 
 * part of this global file, search inside each module or layer. This file is
 * organized as follows:
 * 
 * Global Configuration:
 *  It hols variables that are accessible or necessary to all the layers or modules.
 * MAC layer configuration:
 *  It holds variables to configure the MAC layer.
 * Route layer configuration:
 *  Variables to configure the routing protocol.
 * Transport layer:
 *  Allow to configure the Transport layer.
 * 
 * IPC Queues:
 *  It contains the macros for configuring the IPC Queues module.
 * 
 * Utils:
 *  To configure the utils module, including the memory management for RIOT, and the timers API.
 * @version 0.1
 * @date 2022-07-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*-------------------------------------- GLOBAL CONFIGURATION -----------------------------------*/

/**
 * @brief Hold the node ID, which is generated using the UUID version 4: https://www.uuidgenerator.net/
 */
#ifndef UUID
#define UUID        {0x0c0b4663131a4c5f, 0xa1b73100144476c0}
#endif
