
#include "testing.h"


void nest_try_block(int keep_nesting);


/**
 * Exceed maximum number of exception blocks
 */
TEST_CASE{

    /* will overflow */
    nest_try_block(sizeof(e4c.block) / sizeof(e4c.block[0]) + 1);
}

void nest_try_block(int keep_nesting){

    if(keep_nesting){

        E4C_TRY{

            nest_try_block(--keep_nesting);
        }
    }
}
