#include <math.h>
#include <iostream>

using namespace std;

#define ZIPF_CONSTANT 0.99

class ZipfianGenerator {

    long items; // Number of items.
    long base;  //  Min item to generate.
    double zipfianconstant; // The zipfian constant to use.
    double alpha, zetan, eta, theta, zeta2theta; // Computed parameters for generating the distribution.
    long countforzeta; // The number of items used to compute zetan the last time.

    /**
    *   Flag to prevent problems. If you increase the number of items the zipfian generator is allowed to choose from,
    *   this code will incrementally compute a new zeta value for the larger itemcount. However, if you decrease the
    *   number of items, the code computes zeta from scratch; this is expensive for large itemsets.
    *   Usually this is not intentional; e.g. one thread thinks the number of items is 1001 and calls "nextLong()" with
    *   that item count; then another thread who thinks the number of items is 1000 calls nextLong() with itemcount=1000
    *   triggering the expensive recomputation. (It is expensive for 100 million items, not really for 1000 items.) Why
    *   did the second thread think there were only 1000 items? maybe it read the item count before the first thread
    *   incremented it. So this flag allows you to say if you really do want that recomputation. If true, then the code
    *   will recompute zeta if the itemcount goes down. If false, the code will assume itemcount only goes up, and never
    *   recompute. */
    bool allowitemcountdecrease = false;

    public:
    
    ZipfianGenerator(){}

    /**
    *   Create a zipfian generator for the specified number of items.
    *   @param items The number of items in the distribution.
    *      */
    ZipfianGenerator(long items) : ZipfianGenerator(0, items - 1) { }

    /**
    *   Create a zipfian generator for items between min and max.
    *   @param min The smallest integer to generate in the sequence.
    *   @param max The largest integer to generate in the sequence.
    *   */
    ZipfianGenerator(long min, long max) : ZipfianGenerator(min, max, ZIPF_CONSTANT) { }

    /**
    *   Create a zipfian generator for the specified number of items using the specified zipfian constant.
    *
    *   @param items The number of items in the distribution.
    *   @param zipfianconstant The zipfian constant to use.
    *   */
    /*ZipfianGenerator(long items, double zipf_c) : ZipfianGenerator(0, items - 1, zipf_c) { }*/

    /**
    *   Create a zipfian generator for items between min and max (inclusive) for the specified zipfian constant.
    *   @param min The smallest integer to generate in the sequence.
    *   @param max The largest integer to generate in the sequence.
    *   @param zipfianconstant The zipfian constant to use.
    *   */
    ZipfianGenerator(long min, long max, double zipf_c) : ZipfianGenerator(min, max, zipf_c, zetastatic(max - min + 1, zipf_c)){ }

    /**
    *   Create a zipfian generator for items between min and max (inclusive) for the specified zipfian constant, using
    *   the precomputed value of zeta.
    *
    *   @param min The smallest integer to generate in the sequence.
    *   @param max The largest integer to generate in the sequence.
    *   @param zipfianconstant The zipfian constant to use.
    *   @param zetan The precomputed zeta constant.
    *   */
    ZipfianGenerator(long min, long max, double zipf_c, double zetan) {
        items = max - min + 1;
        base = min;
        this->zipfianconstant = zipf_c;
        theta = this->zipfianconstant;

        zeta2theta = zeta(2, theta);

        alpha = 1.0 / (1.0 - theta);
        this->zetan = zetan;
        countforzeta = items;
        eta = (1 - pow(2.0 / items, 1 - theta)) / (1 - zeta2theta / this->zetan);
    }

    /** 
    *   * Return the next value, skewed by the Zipfian distribution. The 0th item will be the most popular, followed by
    *   * the 1st, followed by the 2nd, etc. (Or, if min != 0, the min-th item is the most popular, the min+1th item the
    *   * next most popular, etc.) If you want the popular items scattered throughout the item space, use
    *   * ScrambledZipfianGenerator instead.
    *   */
    long nextValue() {
        return 0;
        //return nextLong(items);
    }

    long getItems(){
        return items;
    }

    long getMax(){
        return items + base-1;
    }

    /****************************************************************************************/

    /**
    *  * Generate the next item as a long.
    *  *
    *  * @param random_num A given random number < 1.
    *  * @return The next item in the sequence.
    *  */
    long nextLong(double random_num) {
        if(random_num>=1){
            printf("ERROR! Random number must be < 1.0!\n");
            return -1;
        }
        double uz = random_num * zetan;
        if (uz < 1.0) {
            return base;
        }

        if (uz < 1.0 + pow(0.5, theta)) {
            return base + 1;
        }

        long ret = base + (long) (items * pow(eta * random_num - eta + 1, alpha));
        return ret;
    }


    private:

    /**
    *    * Compute the zeta constant needed for the distribution. Do this from scratch for a distribution with n items,
    *    * using the zipfian constant thetaVal. Remember the value of n, so if we change the itemcount, we can recompute zeta.
    *    *
    *    * @param n The number of items to compute zeta over.
    *    * @param thetaVal The zipfian constant.
    *       */
    double zeta(long n, double thetaVal) {
        countforzeta = n;
        return zetastatic(n, thetaVal);
    }

    /**
    *    * Compute the zeta constant needed for the distribution. Do this from scratch for a distribution with n items,
    *    * using the zipfian constant theta. This is a static version of the function which will not remember n.
    *    * @param n The number of items to compute zeta over.
    *    * @param theta The zipfian constant.
    *    */
    static double zetastatic(long n, double theta) {
        return zetastatic(0, n, theta, 0);
    }

    /**
    *    * Compute the zeta constant needed for the distribution. Do this incrementally for a distribution that
    *    * has n items now but used to have st items. Use the zipfian constant thetaVal. Remember the new value of
    *    * n so that if we change the itemcount, we'll know to recompute zeta.
    *    *
    *    * @param st The number of items used to compute the last initialsum
    *    * @param n The number of items to compute zeta over.
    *    * @param thetaVal The zipfian constant.
    *    * @param initialsum The value of zeta we are computing incrementally from.
    *      */
    double zeta(long st, long n, double thetaVal, double initialsum) {
        countforzeta = n;
        return zetastatic(st, n, thetaVal, initialsum);
    }

    /**
    *    * Compute the zeta constant needed for the distribution. Do this incrementally for a distribution that
    *    * has n items now but used to have st items. Use the zipfian constant theta. Remember the new value of
    *    * n so that if we change the itemcount, we'll know to recompute zeta.
    *    * @param st The number of items used to compute the last initialsum
    *    * @param n The number of items to compute zeta over.
    *    * @param theta The zipfian constant.
    *    * @param initialsum The value of zeta we are computing incrementally from.
    *      */
    static double zetastatic(long st, long n, double theta, double initialsum) {
        double sum = initialsum;
        for (long i = st; i < n; i++) {
            sum += 1 / (pow(i + 1, theta));
        }
        return sum;
    }
   

};

