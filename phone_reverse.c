/** @file
 * Implementacja interfejsu opisujące drzewo odwrócone,
 * które przechowuje prefiksy możliwych odwrotnych przekierowań numerów telefonicznych.
 */

#include "phone_forward.h"
#include "phone_reverse.h"
#include "list_of_numbers.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


PhoneReverse *phrevNew(void) {
    PhoneReverse *phrev = (PhoneReverse *) malloc(sizeof(PhoneReverse));
    if (phrev != NULL) {
        for (int i = 0; i < CHILDREN_NUMB; i++) {
            phrev->children[i] = NULL;
        }
        phrev->parent = NULL;
        phrev->listOfFrwd = NULL;
    }
    return phrev;
}


/**
 * @brief Tworzy i zwraca nowy wierzchołek (Reverse).
 * @return PhoneForward* nowy wierzchołek.
 */
static PhoneReverse *newNodeReverse() {
    PhoneReverse *node = (struct PhoneReverse *) malloc(sizeof(PhoneReverse));
    if (node == NULL) {
        return node;
    }

    for (int i = 0; i < CHILDREN_NUMB; i++) {
        node->children[i] = NULL;
    }
    return node;
}


bool phrevAdd(PhoneReverse *pfRev, char const *num1, char const *num2) {
    struct PhoneReverse *temp = pfRev;
    while (*num2) {
        int code = get_digit(*num2);
        // Tworze nowy węzeł, jeśli ścieżka nie istnieje
        if (temp->children[code] == NULL) {
            temp->children[code] = newNodeReverse();
            if (temp->children[code] == NULL) {
                return false;
            }
            temp->children[code]->listOfFrwd = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam się do następnego węzła.
        temp = temp->children[code];
        //  Przesuwam się do następnej literki.
        num2++;
    }
    pfRev = temp;
    pfRev->listOfFrwd = insertToList(pfRev->listOfFrwd, num1);

    return true;
}


void phrevRemove(PhoneReverse *pfRev, const char *num1, const char *num2) {
    if (pfRev != NULL) {
        deleteFrwdFromList(getListOfForwardings(pfRev, num1), num2);
    }
}


void phrevRemoveNumStartsWithPref(PhoneReverse *pfRev, const char *num1, const char *num2) {
    if (pfRev != NULL) {
        deleteFrwdStartsWthPref(getListOfForwardings(pfRev, num1), num2);
    }
}


PhoneNumbers *phfwdReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *pnum = (PhoneNumbers *) malloc(sizeof(PhoneNumbers));

    if (pnum == NULL) {
        return NULL;
    }
    pnum->allNumbers = NULL;

    if (pf == NULL) {
        free(pnum);
        return NULL;
    }
    if (isStringAPhoneNumber(num) == false) {   // Podany napis nie reprezentuje numeru.
        return pnum;
    }

    pnum->allNumbers = insertToList(pnum->allNumbers, num);   // Dodaje od razu num do ciągu wynikowego.
    PhoneReverse *curr = pf->pfRev;

    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char *secondPart = NULL;
    size_t length;
    const char *lastSign = "\0";
    while (*num != '\0') {
        if (*num) {
            curr = curr->children[get_digit(*num)];  // Ide do następnego wierzchołka
        } else {
            break;
        }
        if (curr == NULL) break;
        num++;              // Przesuwam się do innego znaku
        length = strlen(num) + 1;
        secondPart = realloc(secondPart, (sizeof(char) * length));
        memcpy(secondPart, (char *) num, sizeof(char) * (length - 1));
        memcpy(secondPart + length - 1, lastSign, sizeof(char));
        List *currList = curr->listOfFrwd;

        while (currList) {
            createAForward(&currList->forwarding, &secondPart, &lastForward);
            currList = currList->next;
            if (lastForward != NULL) {
                pnum->allNumbers = insertToList(pnum->allNumbers, lastForward);
            }
        }
    }
    if (secondPart) free(secondPart);
    if (lastForward) free(lastForward);

    return pnum;
}


PhoneNumbers *phfwdGetReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *pnum = (PhoneNumbers *) malloc(sizeof(PhoneNumbers));
    if (pnum == NULL) return NULL;
    pnum->allNumbers = NULL;

    if (pf == NULL) {
        free(pnum);
        return NULL;
    }
    if (isStringAPhoneNumber(num) == false) {   // Podany napis nie reprezentuje numeru.
        return pnum;
    }

    PhoneNumbers *pnum1;
    pnum1 = phfwdReverse(pf, num);
    if (pnum1 == NULL) {
        phnumDelete(pnum);
        return NULL;
    }
    List *temp = pnum1->allNumbers;

    while (temp != NULL) {
        PhoneNumbers *pnum2;
        pnum2 = phfwdGet(pf, temp->forwarding);
        List *curr = temp->next;
        if (num && pnum2 && pnum2->allNumbers && strcmp(pnum2->allNumbers->forwarding, num) != 0) {
            if (pnum1->allNumbers) {
                deleteFrwdFromList(&(pnum1->allNumbers), temp->forwarding);
            }
        }
        temp = curr;
        phnumDelete(pnum2);
    }
    pnum->allNumbers = pnum1->allNumbers;
    free(pnum1);

    return pnum;
}


void deleteReverseTree(PhoneReverse *phrev) {
    if (phrev != NULL) {
        for (int i = 0; i < CHILDREN_NUMB; i++) {
            if (phrev->children[i] != NULL) {
                deleteReverseTree(phrev->children[i]);
            }
        }
        // Usuwanie przekierowania (listy)
        listDelete(phrev->listOfFrwd);
        free(phrev);
        phrev = NULL;
    }
}