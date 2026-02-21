#ifdef WIN32
uint8_t staticRefInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase1[] =
#endif
     {
        244, 126, 232, 223, 238, 73,
        135, 165, 150, 222, 64, 50,
        110, 230, 150, 57, 39, 62,
        0, 222, 151, 200, 82, 112,
        7, 192, 171, 187, 207, 32,
        242, 14, 112, 38, 125, 110,

     };

#ifdef WIN32
uint32_t staticRefPixelsProcessedInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefPixelsProcessedInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
uint32_t staticRefCurrentSumInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSumInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
uint32_t staticRefCurrentSqSumInCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSqSumInCase1[] =
#endif
     {
        0,
     };

#ifdef WIN32
float staticRefOut0Case1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOut0Case1[] =
#endif
     {
        132.61111450195312,
     };

#ifdef WIN32
float staticRefOut1Case1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOut1Case1[] =
#endif
     {
        75.39130401611328,
     };

#ifdef WIN32
uint32_t staticRefPixelsProcessedOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefPixelsProcessedOutCase1[] =
#endif
     {
        36,
     };

#ifdef WIN32
uint32_t staticRefCurrentSumOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSumOutCase1[] =
#endif
     {
        4774,
     };

#ifdef WIN32
uint32_t staticRefCurrentSqSumOutCase1[] =
#else
__attribute__((section (".staticData")))
     static uint32_t staticRefCurrentSqSumOutCase1[] =
#endif
     {
        837704,
     };
