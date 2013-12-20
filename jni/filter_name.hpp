// HIPAcc only parses main functions, so we need to abstract the function name
// depending on who is currently compiling this file.

#ifdef HIPACC
  // Main function for HIPAcc
# define FILTER_NAME(NAME) \
      uchar4* pin; \
      uchar4* pout; \
      const int w = 1024; \
      const int h = 1024; \
      int main(int argc, const char **argv)
#else
# ifndef FILTERSCRIPT
    // Filter function name for Renderscript
#   define FILTER_NAME(NAME) \
        int runRS ## NAME (int w, int h, uchar4 *pin, uchar4 *pout)
# else
    // Filter function name for Filterscript
#   define FILTER_NAME(NAME) \
        int runFS ## NAME (int w, int h, uchar4 *pin, uchar4 *pout)
# endif
#endif
