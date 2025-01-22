
#include <exceptions4c-lite.h>


struct e4c_context exceptions4c = {0};
void nest_try_block(int keep_nesting);


/**
 * Exceed maximum number of exception blocks
 */
int main(void) {

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
