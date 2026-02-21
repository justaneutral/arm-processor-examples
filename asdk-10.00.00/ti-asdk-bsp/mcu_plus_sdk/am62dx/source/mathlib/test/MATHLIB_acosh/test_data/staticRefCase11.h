#ifdef WIN32
float staticRefInCase11[] =
#else
__attribute__((section (".staticData")))
     static float staticRefInCase11[] =
#endif
     {
        -0.12284122796970925, 0.7296239857684799, 0.1344061987383357, 0.3959040848320554,
           0.6716326263655983, -0.08093066269630655, 0.9679178293123432, -0.059255185701716784,
           0.9801503764188642, -0.2519435435006202,
     };

#ifdef WIN32
float staticRefOutCase11[] =
#else
__attribute__((section (".staticData")))
     static float staticRefOutCase11[] =
#endif
     {
        2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0,
           2147483647.0, 2147483647.0, 2147483647.0, 2147483647.0,
     };
