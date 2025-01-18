
#include "testing.h"


void nest_try_block(int keep_nesting);


/**
 * Exceed maximum number of exception blocks
 */
TEST_CASE{

    /* will overflow */
    nest_try_block(EXCEPTIONS4C_MAX_BLOCKS + 1);
}

void nest_try_block(int keep_nesting){

    if(keep_nesting){

        TRY {

            nest_try_block(--keep_nesting);
        }
    }
}
