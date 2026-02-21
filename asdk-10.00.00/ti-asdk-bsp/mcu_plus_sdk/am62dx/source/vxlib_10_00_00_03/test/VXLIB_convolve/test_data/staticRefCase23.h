#ifdef WIN32
uint8_t staticRefInCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefInCase23[] =
#endif
     {
        191, 16, 80, 133, 198, 139, 245, 88, 112, 59, 241, 5, 241, 100, 66,
        252, 46, 58, 48, 60, 75, 224, 145, 66, 28, 69, 91, 141, 171, 50,
        88, 191, 89, 1, 167, 213, 22, 68, 159, 231, 2, 52, 197, 151, 78,
        56, 161, 51, 100, 39, 156, 244, 183, 85, 98, 248, 109, 223, 61, 21,
        173, 83, 160, 46, 191, 56, 55, 214, 172, 16, 177, 32, 51, 198, 146,
        165, 134, 251, 134, 167, 192, 110, 34, 191, 202, 41, 9, 151, 135, 162,
        132, 6, 249, 188, 221, 225, 82, 62, 73, 112, 72, 91, 127, 12, 162,

     };

#ifdef WIN32
int16_t staticRefFilterCase23[] =
#else
__attribute__((section (".staticData")))
     static int16_t staticRefFilterCase23[] =
#endif
     {
        -1, -1, 0, 0, -1,
        0, 1, 0, 0, -1,
        0, -1, 1, 2, 0,
        0, 1, 0, 2, 0,
        0, 1, -1, 1, 0,

     };

#ifdef WIN32
uint8_t staticRefOutCase23[] =
#else
__attribute__((section (".staticData")))
     static uint8_t staticRefOutCase23[] =
#endif
     {
        19, 0, 2, 62, 14, 26, 1, 42, 9, 29, 0,
        6, 12, 0, 17, 30, 29, 27, 20, 25, 15, 27,
        14, 9, 0, 20, 13, 26, 63, 19, 18, 56, 3,

     };
