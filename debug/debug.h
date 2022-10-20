/**
 * @file debug.h
 * @brief Define some preprocessor flags used in debugging
 * @author Yetong (Tony) Li
 * @date Oct 17, 2022
 * @version 1.0.1
*/

#define RELEASE
#define DEBUG            // DEBUG flag
#define FILE_SELECT 0    // debug FILE selector
#define SECTION 0        // debug SECTION selector

// If build in release mode, all debug info are turned off
#ifdef RELEASE
    #undef DEBUG
    #undef FILE_SELECT
    #undef SECTION
#endif

// Files are selected based on selector
#ifdef DEBUG
    #if (defined FILE_SELECT) && (FILE_SELECT == 0)
        #define GENOME
    #elif (defined FILE_SELECT) && (FILE_SELECT == 1)
        #define SPECIES
        #if (defined SECTION) && (SECTION == 0)
            #define EXCESS
        #elif (defined SECTION) && (SECTION == 1)
            #define EMPTY
        #endif
    #elif (defined FILE_SELECT) && (FILE_SELECT == 2)
        #define GENERATION
    #endif
#endif