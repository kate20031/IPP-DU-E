/** @file
 * Implementacja interfejsu przechowującego otrzymane przekierowania numerów telefonicznych
 */

#include "phnum.h"
#include "list_of_numbers.h"
#include <stdlib.h>



char const *phnumGet(PhoneNumbers const *pnum, size_t idx) {
    if ((pnum != NULL)) {
        char const *p = NULL;
        if (pnum->allNumbers != NULL) {
            List *current = pnum->allNumbers;
            int i = 0;
            while (current != NULL && i <= (int) idx) {
                if (i == (int) idx) {
                    p = current->forwarding;
                    break;
                }
                current = current->next;
                i++;
            }
        }
        return p;
    } else {
        return NULL;
    }
}


void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        // Usuwanie przekierowania (listy)
        listDelete(pnum->allNumbers);
        // Usuwam podstrukturę i strukturę.
        free(pnum);
        pnum = NULL;
    }
}