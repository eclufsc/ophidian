//
// Created by renan on 3/10/16.
//

#ifndef OPENEDA_FLUTE_H

#define POWVFILE "POWV9.dat"    // LUT for POWV (Wirelength Vector)
#define PORTFILE "PORT9.dat"    // LUT for PORT (Routing Tree)
#define FLUTE_D 9        // LUT is used for d <= FLUTE_D, FLUTE_D <= 9
#define FLUTEROUTING 1   // 1 to construct routing, 0 to estimate WL only
#define REMOVE_DUPLICATE_PIN 0  // Remove dup. pin for flute_wl() & flute()
#define ACCURACY 8  // Default accuracy is 3
//#define MAXD 2008840  // max. degree of a net that can be handled
#define MAXD 1000  // max. degree of a net that can be handled

#ifndef DTYPE   // Data type for distance
#define DTYPE unsigned
#endif

#define OPENEDA_FLUTE_H

namespace openeda {
    namespace interconnection_estimation {
        typedef struct
        {
            unsigned x, y;   // starting point of the branch
            int n;   // index of neighbor
        } tree_branch;

        struct tree
        {
            int deg;   // degree
            unsigned length;   // total wirelength
            tree_branch * branch;   // array of tree branches
        };

        // Major functions
        extern void readLUT();
        extern DTYPE flute_wl(int d, DTYPE x[], DTYPE y[], int acc);
//Macro: DTYPE flutes_wl(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);
        extern tree flute(int d, DTYPE x[], DTYPE y[], int acc);
//Macro: Tree flutes(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);
        extern DTYPE wirelength(tree t);
        extern void printtree(tree t);

//extern Tree flautist(int d, DTYPE x[], DTYPE y[], int acc, const uofm::vector<BBox> &obs, const uofm::vector<unsigned> &relevantObs, unsigned &legal);


// Other useful functions
        extern DTYPE flutes_wl_LD(int d, DTYPE xs[], DTYPE ys[], int s[]);
        extern DTYPE flutes_wl_MD(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);
        extern DTYPE flutes_wl_RDP(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);
        extern tree flutes_LD(int d, DTYPE xs[], DTYPE ys[], int s[]);
        extern tree flutes_MD(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);
        extern tree flutes_RDP(int d, DTYPE xs[], DTYPE ys[], int s[], int acc);

#if REMOVE_DUPLICATE_PIN==1
        #define flutes_wl(d, xs, ys, s, acc) flutes_wl_RDP(d, xs, ys, s, acc)
  #define flutes(d, xs, ys, s, acc) flutes_RDP(d, xs, ys, s, acc)
#else
#define flutes_wl(d, xs, ys, s, acc) flutes_wl_ALLD(d, xs, ys, s, acc)
#define flutes(d, xs, ys, s, acc) flutes_ALLD(d, xs, ys, s, acc)
#endif

#define flutes_wl_ALLD(d, xs, ys, s, acc) flutes_wl_LMD(d, xs, ys, s, acc)
#define flutes_ALLD(d, xs, ys, s, acc) flutes_LMD(d, xs, ys, s, acc)

#define flutes_wl_LMD(d, xs, ys, s, acc) \
    (d<=FLUTE_D ? flutes_wl_LD(d, xs, ys, s) : flutes_wl_MD(d, xs, ys, s, acc))
#define flutes_LMD(d, xs, ys, s, acc) \
    (d<=FLUTE_D ? flutes_LD(d, xs, ys, s) : flutes_MD(d, xs, ys, s, acc))

#define ADIFF(x,y) ((x)>(y)?(x-y):(y-x))  // Absolute difference
    }
}


#endif //OPENEDA_FLUTE_H
