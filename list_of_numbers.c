/** @file
 *  * Implementacja interfejsu opisującą strukturę
 *  listy numerów i wszystkich operacji na liście.
 */

#include "list_of_numbers.h"
#include "phone_forward.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>



/**
 * @brief Sprawdza czy podany numer zaczyna sie prefiksem
 * @param number - wskaźnik na numer(przekierowanie).
 * @param prefix - wskaźnik na prefiks.
 * @return true  - jeśli numer zaczyna sie prefiksem.
 * @return false -  jeśli numer nie zaczyna sie prefiksem.
 */
static bool hasAPrefix(char *number, const char *prefix) {
    if (strlen(prefix) > strlen(number)) {
        return false;
    }
    char *prefixTemp = (char *) prefix;
    char *numberTemp = number;
    int digitPref = get_digit(*prefixTemp);
    int digitNumb = get_digit(*numberTemp);

    while ((*prefixTemp != '\0') && (digitPref == digitNumb)) {
        prefixTemp++;
        numberTemp++;
        digitPref = get_digit(*prefixTemp);
        digitNumb = get_digit(*numberTemp);
    }

    if (*prefixTemp != '\0') {
        return false;
    }
    return true;
}


void deleteFrwdStartsWthPref(List **list, const char *prefix) {
    // Usuwam elementy na początku.
    while (*list && hasAPrefix((*list)->forwarding, prefix)) {
        List *tmp = *list;
        *list = (*list)->next;
        free(tmp->forwarding);
        tmp->forwarding = NULL;
        free(tmp);
        tmp = NULL;
    }

    // Usuwam element w środku lub na końcu listy.
    for (List *current = *list; current != NULL; current = current->next) {
        while (current->next != NULL && hasAPrefix(current->next->forwarding, prefix)) {
            List *tmp = current->next;
            current->next = tmp->next;
            free(tmp->forwarding);
            tmp->forwarding = NULL;
            free(tmp);
            tmp = NULL;
        }
    }
}


/**
 * @brief Porównuję napisy leksykograficznie
 * Zmodyfikowana funkcja strcmp
 * @param firstStr - wskaźnik na pierwszy napis
 * @param secondStr - wskaźnik na drugi napis
 * @return int - liczba dodatnia/ujemna/zero w zależności od wyniku porównania
 */
static int compare(const char *firstStr, const char *secondStr) {
    const unsigned char *tmpFirstStr = (const unsigned char *) firstStr;
    const unsigned char *tmpSecondStr = (const unsigned char *) secondStr;
    unsigned char signFirstStr, signSecondStr;
    do {
        signFirstStr = (unsigned char) *tmpFirstStr++;
        signSecondStr = (unsigned char) *tmpSecondStr++;
        if (signFirstStr == '\0') {
            return get_digit(signFirstStr) - get_digit(signSecondStr);
        }
    } while (get_digit(signFirstStr) == get_digit(signSecondStr));
    return get_digit(signFirstStr) - get_digit(signSecondStr);
}


void deleteFrwdFromList(List **list, const char *num) {
    // Usuwam element na początku.
    if (*list && (compare((*list)->forwarding, num) == 0)) {
        List *tmp = *list;
        *list = (*list)->next;
        if (tmp->forwarding != NULL) {
            free(tmp->forwarding);
            tmp->forwarding = NULL;
            free(tmp);
            tmp = NULL;
        }
        return;
    }

    // Usuwam element w środku lub na końcu listy.
    for (List *current = *list; current != NULL; current = current->next) {
        if ((current->next != NULL) && (current->next->forwarding != NULL)
            && (compare(current->next->forwarding, num) == 0)) {

            List *tmp = current->next;
            current->next = tmp->next;
            if (tmp->forwarding != NULL) {
                free(tmp->forwarding);
                tmp->forwarding = NULL;
                free(tmp);
                tmp = NULL;
            }
            break;
        }
    }
}


List *insertToList(List *list, const char *num) {
    List *ptr = malloc(sizeof(List));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->forwarding = malloc(sizeof(char) * (strlen(num) + 1));
    if (ptr->forwarding == NULL) {
        free(ptr);
        ptr = NULL;
        return NULL;
    }
    memcpy(ptr->forwarding, (char *) num, sizeof(char) * (strlen(num) + 1));
    ptr->next = NULL;

    if (list == NULL) {
        ptr->next = list;
        return ptr;
    } else if (compare(num, list->forwarding) < 0) {
        ptr->next = list;
        return ptr;
    } else {
        List *cur = list;
        while (cur->next != NULL && compare(num, cur->next->forwarding) >= 0) {
            if (compare(num, cur->next->forwarding) == 0) {
                free(ptr->forwarding);
                free(ptr);
                return list;
            }
            cur = cur->next;
        }
        ptr->next = cur->next;
        cur->next = ptr;
        return list;
    }
}


List **getListOfForwardings(PhoneReverse *pfRev, const char *num) {
    PhoneReverse *curr = pfRev;
    size_t numberLength = strlen(num);
    for (size_t i = 0; i < numberLength; i++) {
        if (!curr->children[get_digit(*num)]) {
            break;
        }
        curr = curr->children[get_digit(*num)];
        num++;
    }
    return &curr->listOfFrwd;
}


void listDelete(List *list) {
    if (list != NULL) {
        List *current = list;
        List *next;
        while (current != NULL) {
            next = current->next;
            if (current->forwarding != NULL) {
                free(current->forwarding);
                current->forwarding = NULL;
            }
            free(current);
            current = NULL;
            current = next;
        }
        list = NULL;
    }
}