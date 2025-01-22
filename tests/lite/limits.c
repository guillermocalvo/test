
#include <exceptions4c-lite.h>


struct e4c_context exceptions4c = {0};
void nest_try_block(int keep_nesting);


/**
 * Reach maximum number of exception blocks
 */
int main(void) {

    /* will not overflow */
    nest_try_block(EXCEPTIONS4C_MAX_BLOCKS);
}

void nest_try_block(int keep_nesting){

    if(keep_nesting){

        TRY {

            nest_try_block(--keep_nesting);
        }
    }
}
