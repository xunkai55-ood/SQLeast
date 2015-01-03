#ifndef RM_BITMAP_H
#define RM_BITMAP_H

namespace bitmap8 {

    char lowest0[256];  // table of lowest 0 in a int

    void init() {
        for (int i = 0; i < 255; i++) {
            int tmp = ((i ^ (i + 1)) >> 1) + 1;
            switch (tmp) {
                case 1: lowest0[i] = 0; break;
                case 2: lowest0[i] = 1; break;
                case 4: lowest0[i] = 2; break;
                case 8: lowest0[i] = 3; break;
                case 16: lowest0[i] = 4; break;
                case 32: lowest0[i] = 5; break;
                case 64: lowest0[i] = 6; break;
                case 128: lowest0[i] = 7; break;
            }
        }
        lowest0[255] = -1;
    }

    init();
}

#endif