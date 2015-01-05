#ifndef RM_BITMAP_H
#define RM_BITMAP_H

namespace sqleast {

    namespace rm {

        class Bitmap8Util {

        public:


            inline static int lowest0(int x) {
                if (!inited) init();
                return lowest0_[x];
            }

            inline static void init() {
                initLowest0();
            }

        private:

            static bool inited;
            static char lowest0_[256];  // table of lowest 0 in a int

            static void initLowest0() {
                for (int i = 0; i < 255; i++) {
                    int tmp = ((i ^ (i + 1)) >> 1) + 1;
                    switch (tmp) {
                        case 1:
                            lowest0_[i] = 0;
                            break;
                        case 2:
                            lowest0_[i] = 1;
                            break;
                        case 4:
                            lowest0_[i] = 2;
                            break;
                        case 8:
                            lowest0_[i] = 3;
                            break;
                        case 16:
                            lowest0_[i] = 4;
                            break;
                        case 32:
                            lowest0_[i] = 5;
                            break;
                        case 64:
                            lowest0_[i] = 6;
                            break;
                        case 128:
                            lowest0_[i] = 7;
                            break;
                    }
                }
                lowest0_[255] = -1;
            }

        };

    }

}

#endif