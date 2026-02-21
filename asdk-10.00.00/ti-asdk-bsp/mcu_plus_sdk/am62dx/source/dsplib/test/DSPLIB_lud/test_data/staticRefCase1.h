// A Matrix
#ifdef WIN32
float staticRefInACase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInACase1[] =
#endif
     {
        -0.7000, -7.2900, -1.1800, 4.4200,
        3.7500, -3.0700, -9.8100, -3.9400,
        -5.4200, 9.2400, 0.6900, 3.3200,
        2.1100, -7.3100, -8.8500, 3.6800,

     };

// B vector
#ifdef WIN32
float staticRefInBCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInBCase1[] =
#endif
     {
        5.9400, -9.3500, -6.8800, 0.3100,
     };

// P Matrix
#ifdef WIN32
unsigned short staticRefOutPCase1[] =
#else
__attribute__((section (".staticData")))
     static unsigned short staticRefOutPCase1[] =
#endif
     {
        0, 0, 1, 0,
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 1,

     };

// L Matrix
#ifdef WIN32
float staticRefOutLCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutLCase1[] =
#endif
     {
        1.0000, 0.0000, 0.0000, 0.0000,
        0.1292, 1.0000, 0.0000, 0.0000,
        -0.6919, -0.3917, 1.0000, 0.0000,
        -0.3893, 0.4377, 0.8165, 1.0000,

     };

// U Matrix
#ifdef WIN32
float staticRefOutUCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutUCase1[] =
#endif
     {
        -5.4200, 9.2400, 0.6900, 3.3200,
        0.0000, -8.4834, -1.2691, 3.9912,
        0.0000, 0.0000, -9.8297, -0.0796,
        0.0000, 0.0000, 0.0000, 3.2906,

     };

// X vector
#ifdef WIN32
float staticRefOutXCase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutXCase1[] =
#endif
     {
        1.4607, -0.4085, 1.1536, 1.2095,
     };

// invA Matrix
#ifdef WIN32
float staticRefOutInvACase1[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutInvACase1[] =
#endif
     {
        -0.5217, -0.3383, -0.1837, 0.4302,
        -0.2205, -0.1018, 0.0138, 0.1433,
        -0.0380, -0.0997, -0.0650, -0.0025,
        -0.2302, -0.2481, -0.0236, 0.3039,

     };
