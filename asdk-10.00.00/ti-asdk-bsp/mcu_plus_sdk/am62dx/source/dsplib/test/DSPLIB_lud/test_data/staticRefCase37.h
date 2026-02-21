// A Matrix
#ifdef WIN32
double staticRefInACase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefInACase37[] =
#endif
     {
        5.0599, 3.6420, 2.1848, 4.3432,
        -6.2869, -6.2081, -5.8071, 8.7819,
        -3.7569, -3.2351, -7.8147, -9.2018,
        -4.9844, 9.5559, 3.7766, 7.9161,

     };

// B vector
#ifdef WIN32
double staticRefInBCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefInBCase37[] =
#endif
     {
        -9.8875, 6.4100, 4.0424, 4.8733,
     };

// P Matrix
#ifdef WIN32
unsigned short staticRefOutPCase37[] =
#else
__attribute__((section (".staticData")))
     static unsigned short staticRefOutPCase37[] =
#endif
     {
        1, 0, 0, 0,
        0, 0, 0, 1,
        0, 0, 1, 0,
        0, 1, 0, 0,

     };

// L Matrix
#ifdef WIN32
double staticRefOutLCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutLCase37[] =
#endif
     {
        1.0000, 0.0000, 0.0000, 0.0000,
        -0.9851, 1.0000, 0.0000, 0.0000,
        -0.7425, -0.0404, 1.0000, 0.0000,
        -1.2425, -0.1280, 0.3920, 1.0000,

     };

// U Matrix
#ifdef WIN32
double staticRefOutUCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutUCase37[] =
#endif
     {
        5.0599, 3.6420, 2.1848, 4.3432,
        0.0000, 13.1435, 5.9288, 12.1945,
        0.0000, 0.0000, -5.9529, -5.4843,
        0.0000, 0.0000, 0.0000, 17.8894,

     };

// X vector
#ifdef WIN32
double staticRefOutXCase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutXCase37[] =
#endif
     {
        -1.7241, -0.4883, 0.8513, -0.2867,
     };

// invA Matrix
#ifdef WIN32
double staticRefOutInvACase37[] =
#else
__attribute__((section (".staticData")))
     static double staticRefOutInvACase37[] =
#endif
     {
        0.1523, -0.0051, 0.0200, -0.0546,
        0.1038, -0.0286, 0.0870, 0.0759,
        -0.1861, -0.0515, -0.1478, -0.0126,
        0.0594, 0.0559, -0.0219, 0.0063,

     };
