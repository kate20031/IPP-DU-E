/** @file
 * Implementacja interfejsu opisujące drzewo,
 * które przechowuje prefiksy możliwych przekierowań numerów telefonicznych.
 *
 * @author Kateryna Pavlichenko <marpe@mimuw.edu.pl>
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "phone_forward.h"
#include "phone_reverse.h"
#include "../../../../Pobrane/Telegram Desktop/duże/src/list_of_numbers.h"
#include "phnum.h"
#define CHILDREN_NUMB 12 ///<Rozmiar drzewa



PhoneForward *phfwdNew(void) {
    PhoneForward *pf = (PhoneForward *) malloc(sizeof(PhoneForward));

    if (pf != NULL) {
        for (int i = 0; i < CHILDREN_NUMB; i++) {
            pf->children[i] = NULL;
        }
        pf->parent = NULL;
        pf->forwarding = NULL;
        pf->pfRev = phrevNew();
        if (pf->pfRev == NULL) {
            free(pf);
            pf = NULL;
        }
    }
    return pf;
}


/**
 * @brief Tworzy i zwraca nowy wierzchołek.
 *
 * @return PhoneForward* nowy wierzchołek.
 */
static PhoneForward *newNode() {
    PhoneForward *node = (struct PhoneForward *) malloc(sizeof(PhoneForward));
    if (node == NULL) {
        return node;
    }

    for (int i = 0; i < CHILDREN_NUMB; i++) {
        node->children[i] = NULL;
    }
    return node;
}


int get_digit(char c) {
    if (c == '*') return 10;
    if (c == '#') return 11;
    if ((c == ';') || (c == ':')) return -1;
    return c - '0';
}


bool isStringAPhoneNumber(const char *num) {
    int length = 0;
    if ((num == NULL) || (num[0] == '\0')) {    // Napis jest pusty albo wskazuje na NULL.
        return false;
    }

    while (num[length] != '\0') {
        if (isdigit(num[length]) || (get_digit(num[length]) == 10) || (get_digit(num[length]) == 11)) {
            length++;
        } else {          // Natrafiliśmy na znak różny od cyfry
            return false;
        }
    }
    return true;
}


/**
 * @brief Funkcja usuwa rekurencyjnie przekierowanie z drzewa zwykłego.
 * @param pf –  wskaźnik na drzewo.
 * @param pfRev – wskaźnik na drzewo odwrócone.
 * @param num - prefiks, numery zaczynające sie na ten prefiks beda usunięte.
 * @param listOfRemoves - lista przekierowań.
 */
static void phfwdRemoveRek(PhoneForward *pf, PhoneReverse *pfRev, char const *num, List **listOfRemoves) {
    if (pf != NULL) {
        for (int i = 0; i < CHILDREN_NUMB; i++) {
            if (pf->children[i]) {
                phfwdRemoveRek(pf->children[i], pfRev, num, listOfRemoves);
            }
            if (pf->forwarding != NULL) {
                phrevRemoveNumStartsWithPref(pfRev, pf->forwarding, num);
                free(pf->forwarding);
                pf->forwarding = NULL;
            }
        }
    }
}


void phfwdRemove(PhoneForward *pf, char const *num) {
    if ((pf != NULL) && (num != NULL) && isStringAPhoneNumber(num)) {
        PhoneForward *curr = pf;
        char *tempNum = (char *) num;
        size_t numberLength = strlen(tempNum);
        for (size_t i = 0; i < numberLength; i++) {
            if (!curr->children[get_digit(*tempNum)]) {
                return;
            }
            curr = curr->children[get_digit(*tempNum)];
            tempNum++;
        }
        List *listOfRemoves = NULL;

        for (int i = 0; i < CHILDREN_NUMB; i++) {
            if (curr->children[i] != NULL && curr->children[i]->forwarding != NULL) {
                phrevRemoveNumStartsWithPref(pf->pfRev, curr->children[i]->forwarding, num);
                free(curr->children[i]->forwarding);
                curr->children[i]->forwarding = NULL;
            }
        }
        phfwdRemoveRek(curr, pf->pfRev, num, &listOfRemoves);
        listDelete(listOfRemoves);
    }
}


void createAForward(char *const *firstPart, char **secondPart, char **lastForward) {
    size_t length;
    const char *lastSign = "\0";

    if (!*secondPart) {
        length = strlen(*firstPart) + 1;
    } else {
        length = strlen(*secondPart) + strlen(*firstPart) + 1;
    }
    free(*lastForward);
    *lastForward = (char *) malloc(sizeof(char) * length);

    if (*lastForward) {
        memcpy(*lastForward, *firstPart, strlen(*firstPart) * sizeof(char));
    }
    if (*secondPart && *lastForward) {
        memcpy(*lastForward + strlen(*firstPart), *secondPart, strlen(*secondPart) * sizeof(char));
    }
    if (*lastForward) memcpy(*lastForward + length - 1, lastSign, sizeof(char));
}


PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {
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

    // Postępuje analogicznie jak w phfwdReverse (tam jest krótko opisana metoda szukania przekierowania).
    PhoneForward const *curr = pf;
    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char *maxForward = NULL; // Najdłuższe dotychczasowe przekierowanie/
    char *secondPart = NULL;
    size_t length;
    const char *lastSign = "\0";
    char const *numCopy = num;

    maxForward = (char *) malloc(sizeof(char));
    if (maxForward) {
        memcpy(maxForward, "\0", sizeof(char));
    }

    while (*num != '\0') {
        curr = curr->children[get_digit(*num)];  // Ide do następnego wierzchołka
        if (curr == NULL) {
            break;
        }
        num++;                       // Przesuwam się do innego znaku
        length = strlen(num) + 1;
        secondPart = realloc(secondPart, (sizeof(char) * length));

        if (secondPart) {
            memcpy(secondPart, (char *) num, sizeof(char) * (length - 1));
            memcpy(secondPart + length - 1, lastSign, sizeof(char));
        }

        if (curr->forwarding) {
            createAForward(&curr->forwarding, &secondPart, &lastForward);
        }
        if (lastForward != NULL && maxForward != NULL) {
            if (strlen(lastForward) >= strlen(maxForward)) {
                length = strlen(lastForward) + 1;
                free(maxForward);
                maxForward = malloc(sizeof(char) * length);
                if (maxForward) memcpy(maxForward, lastForward, length);
            }
        }
    }
    // Wstawiam do listy wynikowej
    if (lastForward != NULL) {
        pnum->allNumbers = insertToList(pnum->allNumbers, lastForward);
    } else {
        pnum->allNumbers = insertToList(pnum->allNumbers, numCopy);
    }
    if (secondPart) free(secondPart);
    if (lastForward) free(lastForward);
    if (maxForward) free(maxForward);

    return pnum;
}


/**
 * @brief Funkcja pomocnicza sprawdza poprawność danych wejściowych.
 * @param[in,out] pf - wskaźnik na strukturę przechowująca przekierowania
 *                     numerów;
 * @param[in] num1   - wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   - wskaźnik na napis reprezentujący prefiks numerów,
 *                     na które jest wykonywane przekierowanie.
 * @return  * @return Wartość @p true, jeśli dane są poprawne.
 *         Wartość @p false – wpp.
 */
static bool isPhfwdAddCorrectInput(PhoneForward *pf, char const *num1, char const *num2) {
    if ((pf == NULL) || (num1 == NULL) || (num2 == NULL)) {
        return false;
    }
    if ((isStringAPhoneNumber(num1) == false) ||
        (isStringAPhoneNumber(num2) == false)) {  // Któryś z napisów nie jest numerem.
        return false;
    }
    if (strcmp(num1, num2) == 0) {   // Podane numery są identyczne.
        return false;
    }
    return true;
}


bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2) {
    if (!isPhfwdAddCorrectInput(pf, num1, num2)) return false;

    struct PhoneForward *temp = pf;
    char const *copyNum1 = num1;

    while (*num1) {
        int code = get_digit(*num1);
        // Tworzę nowy węzeł, jeśli ścieżka nie istnieje.
        if (temp->children[code] == NULL) {
            temp->children[code] = newNode();
            if (temp->children[code] == NULL) {
                return false;
            }
            temp->children[code]->forwarding = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam się do następnego węzła.
        temp = temp->children[code];

        //  Przesuwam się do następnego węzła.
        num1++;
    }

    if (temp->forwarding) {
        phrevRemove(pf->pfRev, temp->forwarding, copyNum1);
        free(temp->forwarding);
        temp->forwarding = NULL;
    }
    temp->forwarding = (char *) malloc(sizeof(char) * (strlen(num2) + 1));
    if (temp->forwarding == NULL) {
        return false;
    }
    strcpy(temp->forwarding, num2);
    // Dodaje przekierowania do drzewa przekierowań forwarding ("odwróconego").
    bool ok = phrevAdd(pf->pfRev, copyNum1, num2);

    if (!ok) {
        return false;
    }
    return true;
}


/**
 * @brief Rekurencyjne usuwanie struktury PhoneForward.
 * (Funkcja pomocnicza)
 * Rekurencyjne usuwanie struktury PhoneForward nie usuwając
 * podstruktury drzewa odwróconego "PfRev".
 * @param pf - wskaźnik na usuwana strukturę.
 */
static void deleteRegularTree(PhoneForward *pf) {
    for (int i = 0; i < CHILDREN_NUMB; i++) {
        if (pf->children[i] != NULL) {
            deleteRegularTree(pf->children[i]);
        }
    }
    if (pf->forwarding != NULL) {
        free(pf->forwarding);
        pf->forwarding = NULL;
    }
    free(pf);
    pf = NULL;
}


void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        deleteReverseTree(pf->pfRev);
        deleteRegularTree(pf);
    }
}