/** @file
 * Implementacja interfejsu przechowujacego przekierowania numerow telefonicznych
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

#define SIZE 12 ///<Rozmiar drzewa

/**
 * @brief Structura do przechowywania przekierowan.
 *  Przechowywam przekierowania w dzrewie tries.
 * Drzewo ma 12 dzieci (od 0 do 11). 10 - to '*', 11 - to '*',
 * a pozostale cyferki sa sa odpowiednikami cyfr w numerze.
 * Przekierowanie 'dakad' przechowywam w forwarding. (Znajduje sie w synie najmniej
 * znaczacej cyfry przekierowania 'skad'.
 * W pfRev przechowuje drzewo przekierowan odwrotnych (Reverse).
 */
struct PhoneForward {
    struct PhoneForward *children[SIZE]; ///<"dzieci" wierzcholka drzewa.
    struct PhoneForward *parent;    ///<rodzic danego wierzcolka.
    char *forwarding;  ///<przekierowanie.
    struct PhoneReverse *pfRev; ///<struktura przekierowan odwroconych (Reverse).
};
/**
 * @brief to jest typ PhoneForward
 *
 */
typedef struct PhoneForward PhoneForward;


/**
 * @brief Lista przekierowan.
 * Przechowuje przekierowania (jest uzywana dla pfRev i w funkcji phfwdGet).
 */
struct List {
    char *forwarding;  ///<przekierowanie(numer).
    struct List *next;  ///<wskaznik na nastepny element.
};
/**
 * @brief to jest typ Lista
 *
 */
typedef struct List List;


/**
 * @brief Struktura przekierowan odwroconych.
 * Trzymam drzewo odwrocone przekierowan.
 * Skoro przekierowan 'dokad' moze byc kilka,
 * przekierowania przechowywam w liscie 'forwarding'.
 */
struct PhoneReverse {
    struct PhoneReverse *children[SIZE];  ///<"dzieci" wierzcholka drzewa.
    struct PhoneReverse *parent;  ///<Rodzic danego wierzcolka.
    struct List *listOfFrwd;  ///<Przekierowanie.
};
/**
 * @brief To jest typ PhoneReverse
 *
 */
typedef struct PhoneReverse PhoneReverse;


/**
 * @brief Przechowuje strukture numerow przekierowanych.
 * Przechowywam wszystkie wynikowe przekierowania w liscie
 */
struct PhoneNumbers {
    struct List *allNumbers;  ///<wskaznik na liste numerow.
};
/**
 * @brief To jest typ PhoneNumbers.
 *
 */
typedef struct PhoneNumbers PhoneNumbers;


PhoneReverse *phrevNew(void) {
    PhoneReverse *phrev = (PhoneReverse *) malloc(sizeof(PhoneReverse));
    if (phrev != NULL) {
        for (int i = 0; i < SIZE; i++) {
            phrev->children[i] = NULL;
        }
        phrev->parent = NULL;
        phrev->listOfFrwd = NULL;
    }
    return phrev;
}


PhoneForward *phfwdNew(void) {
    PhoneForward *pf = (PhoneForward *) malloc(sizeof(PhoneForward));

    if (pf != NULL) {
        for (int i = 0; i < SIZE; i++) {
            pf->children[i] = NULL;
        }
        pf->parent = NULL;
        pf->forwarding = NULL;
        pf->pfRev = phrevNew();
        if (pf->pfRev == NULL) {
            free(pf);
            pf = NULL;
        //    return NULL;
        }
    }
    return pf;
}


/**
 * @brief Usuwa liste
 *
 * @param list - wskaznik na liste
 */
static void listDelete(List *list) {
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


/**
 * @brief Zwraca liczbowa postac znaku.
 *
 * @param c - znak.
 * @return int liczbowa postac znaku.
 */
static int get_digit(char c) {
    if (c == '*') return 10;
    if (c == '#') return 11;
    if ((c == ';') || (c == ':')) return -1;
    return c - '0';
}


/**
 * @brief Porownuje napisy leksykograficznie
 * Zmodyfikowana funkcja strcmp
 * @param firstStr - wskaznik na pierwszy napis
 * @param secondStr - wskaznik na drugi napis
 * @return int - liczba dodatnia/ujemna/zero w zaleznosci od wyniku porownania
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


/**
 * @brief Sprawdza czy podany numer zaczyna sie prefiksem
 *
 * @param number - wskaznik na numer(przekierowanie).
 * @param prefix - wskaznik na prefiks.
 * @return true  - jesli numer zaczyna sie przefiksem.
 * @return false -  jesli numer nie zaczyna sie przefiksem.
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


/**
 * @brief Usuwanie z listy przekierowan, ktore sa takie same jak "num" (leksykograficznie)
 *
 * @param list - wskaznik na liste.
 * @param num - wskaznik na szukane przekierowanie.
 */
static void deleteFrwdFromList(List **list, const char *num) {
    // Usuwam element na poczatku.
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

    // Usuwam element w srodku lub na koncu listy.
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


/**
 * @brief Usuwanie z listy przekierowan zaczynajacych sie przefiksem "prefix"
 *
 * @param list - wskaznik na liste.
 * @param prefix - wskaznik na przefiks.
 */
static void deleteFrwdStartsWthPref(List **list, const char *prefix) {
    // Usuwam elementy na poczatku.
    while (*list && hasAPrefix((*list)->forwarding, prefix)) {
        List *tmp = *list;
        *list = (*list)->next;
        free(tmp->forwarding);
        tmp->forwarding = NULL;
        free(tmp);
        tmp = NULL;
    }

    // Usuwam element w srodku lub na koncu listy.
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


/** @brief Usuwa strukture.
 * Usuwa strukture wskazywana przez @p phrev. Nic nie robi, jesli wskaznik ten ma
 * wartosc NULL. (Funkcja pomocnicza do usuwania drzewa odwroconego).
 * @param[in] phrev - wskaznik na usuwana strukture.
 */
static void deleteReverseTree(PhoneReverse *phrev) {
    if (phrev != NULL) {
        for (int i = 0; i < SIZE; i++) {
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


/**
 * @brief Rekyrencyjne usuwanie struktury PhoneForward.
 * (Funkcja pomocnicza)
 * Rekyrencyjne usuwanie struktury PhoneForward nie usuwając
 * podstruktury drzewa odwroconego "PfRev".
 * @param pf - wskaznik na usuwana strukture.
 */
static void deleteRegularTree(PhoneForward *pf) {
    for (int i = 0; i < SIZE; i++) {
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


/**
 * @brief Sprawdza czy napis jest numerem
 * Sprawdza czy podany napis sklada sie tylko z cyfr.
 * @param[in] num  -  Sprawdzany napis.
 * @return int 0, jezeli napis nie jest numerem (zawiera inny znak niz cyfra
 *                  lub jest pusty).
 *         int 1, jezeli napis jest numerem.
 */
static bool isStringAPhoneNumber(const char *num) {
    int length = 0;
    if ((num == NULL) || (num[0] == '\0')) {    // Napis jest pusty albo wskazuje na NULL.
        return false;
    }

    while (num[length] != '\0') {
        if (isdigit(num[length]) || (get_digit(num[length]) == 10) || (get_digit(num[length]) == 11)) {
            length++;
        } else {          // Natrafilismy na znak rozny od cyfry
            return false;
        }
    }
    return true;
}


/**
 * @brief Tworzy i zwraca nowy wierzcholek.
 *
 * @return PhoneForward* nowy wierzcholek.
 */
static PhoneForward *newNode() {
    PhoneForward *node = (struct PhoneForward *) malloc(sizeof(PhoneForward));
    if (node == NULL) {
        return node;
    }

    for (int i = 0; i < SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}


/**
 * @brief Tworzy i zwraca nowy wierzcholek (Reverse).
 *
 * @return PhoneForward* nowy wierzcholek.
 */
static PhoneReverse *newNodeReverse() {
    PhoneReverse *node = (struct PhoneReverse *) malloc(sizeof(PhoneReverse));
    if (node == NULL) {
        return node;
    }

    for (int i = 0; i < SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}


/**
 * @brief Dodaje przekierowanie (odwrocone) do listy
 * Dodaje przekierowanie (odwrocone) do listy posortowanej leksykograficznie
 * W wiercholku pod numerem (numeruje od 0 do SIZE - 1) ostaniej cyfry przekierowania "dokad" trzymam
 * liste numerow przekierowan "skad".
 * @param list - wskaznik na liste z ktorych sa przekierowania
 * @param num  - wskaznik na napis do ktorego jest przekierowanie
 */
static List *insertToList(List *list, const char *num) {
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


/**
 * @brief Dodaje przekierowanie(odwrocone).
 *
* @param pfRev - wskaznik na strukture przechowujaca przekierowania
 *                     numerow;
 * @param[in] num1   - wskaznik na napis reprezentujacy prefiks numerow
 *                     przekierowywanych;
 * @param[in] num2   - wskaznik na napis reprezentujacy prefiks numerow
 *
 *  @return Wartosc @p true, jesli przekierowanie zostalo dodane.
 *         Wartosc @p false, jesli wystapil blad, np. podany napis nie
 *         reprezentuje numeru, oba podane numery sa identyczne lub nie udalo
 *         sie alokowac pamieci.
 */
static bool phrevAdd(PhoneReverse *pfRev, char const *num1, char const *num2) {
    struct PhoneReverse *temp = pfRev;
    while (*num2) {
        int code = get_digit(*num2);
        // Tworze nowy wezel, jesli sciezka nie istnieje
        if (temp->children[code] == NULL) {
            temp->children[code] = newNodeReverse();
            if (temp->children[code] == NULL) {
                return false;
            }
            temp->children[code]->listOfFrwd = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam sie do nastepnego wezla.
        temp = temp->children[code];
        //  Przesuwam sie do nastepnej literki.
        num2++;
    }
    pfRev = temp;
    pfRev->listOfFrwd = insertToList(pfRev->listOfFrwd, num1);

    return true;
}


/**
 * @brief Funkcja znajduje i zwraca listę przekierowan
 * Funkcja znajduje i zwraca listę przekierowan w dzewie odwroconym
 * za podanym przefiksem.
 * @param pfRev - wskaznik na drzewo odwrocone.
 * @param num - wskaznik na numer przekierowania "skad".
 * @return lista przekierowan "dokąd"
 */
static List **getListOfForwardings(PhoneReverse *pfRev, const char *num) {
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

/**
 * @brief Usuwa przekierowania z drzewa odwroconego
 * Usuwa z drzewa wszystkie przekierowania, które "=="
 * leksykograficznie num2.
 * @param pfRev - wskaznik na drzewo odwrocone.
 * @param num1 - wskaznik na numer przekierowania "dokad".
 * @param num2- wskaznik na numer przekierowania "skad".
 */
static void phrevRemove(PhoneReverse *pfRev, const char *num1, const char *num2) {
    if (pfRev != NULL) {
        deleteFrwdFromList(getListOfForwardings(pfRev, num1), num2);
    }
}


/**
 * @brief Usuwa przekierowania (za prefiksem) z drzewa odwroconego
 * Usuwa z drzewa wszystkie przekierowania, zaczynajace sie podanym prefiksem
 * @param pfRev - wskaznik na drzewo odwrocone.
 * @param num1- wskaznik na numer przekierowania "dokad"
 * @param num2- wskaznik na numer przekierowania "skad" (przefiks)
 */
static void phrevRemoveNumStartsWithPref(PhoneReverse *pfRev, const char *num1, const char *num2) {
    if (pfRev != NULL) {
        deleteFrwdStartsWthPref(getListOfForwardings(pfRev, num1), num2);
    }
}


 /**
  * Funkcja usuwa rekursywnie przekierowanie z drzewa zwyklego.
  * @param pf –  wskaznik na drzewo.
  * @param pfRev – wskaznik na drzewo odwrocone.
  * @param num - prefiks, numery zaczynajace sie na ten prefiks beda usuniete.
  * @param listOfRemoves - lista przekierowan.
  */
static void phfwdRemoveRek(PhoneForward *pf, PhoneReverse *pfRev, char const *num, List **listOfRemoves) {
    if (pf != NULL) {
        for (int i = 0; i < SIZE; i++) {
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

        for (int i = 0; i < SIZE; i++) {
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


void phnumDelete(PhoneNumbers *pnum) {
    if (pnum != NULL) {
        // Usuwanie przekierowania (listy)
        listDelete(pnum->allNumbers);
        // Usuwam podstrukture i strukture.
        free(pnum);
        pnum = NULL;
    }
}

/**
 * Funkcja tworzy przekierowanie, kopijuje go zawartość do "lastForward"
 * @param firstPart – pierwsza część tworzonego przekierowania.
 * @param secondPart – druga część tworzonego przekierowania.
 * @param lastForward – ostannie znalezione przekierowanie.
 */
static void createAForward(char *const *firstPart, char **secondPart, char **lastForward) {
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

    // Postepuje analogicznie jak w  phfwdReverse (tam jest krotko opisana metoda szukania przekierowania).
    PhoneForward const *curr = pf;
    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char *maxForward = NULL; // Najdluzsze dotychczasowe przekierowanie/
    char *secondPart = NULL;
    size_t length;
    const char *lastSign = "\0";
    char const *numCopy = num;

    maxForward = (char *) malloc(sizeof(char));
    if (maxForward) {
        memcpy(maxForward, "\0", sizeof(char));
    }

    while (*num != '\0') {
        curr = curr->children[get_digit(*num)];  // Ide do nastepnogo wierzcholka
        if (curr == NULL) {
            break;
        }
        num++;                       // Przesuwam sie do innego znaku
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


/** @brief Udostepnia numer.
 * Udostepnia wskaznik na napis reprezentujacy numer. Napisy sa indeksowane
 * kolejno od zera.
 * @param[in] pnum - wskaznik na strukture przechowujaca ciag numerow telefonow;

 * @param[in] idx  - indeks numeru telefonu.
 * @return Wskaznik na napis reprezentujacy numer telefonu. Wartosc NULL, jesli
 *         wskaznik @p pnum ma wartosc NULL lub indeks ma za duza wartosc.
 */
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

/**
 * Funkcja pomocnicza sprawdza poprawnosc danych wejsciowych.
 * @param[in,out] pf - wskaznik na strukture przechowujaca przekierowania
 *                     numerow;
 * @param[in] num1   - wskaznik na napis reprezentujacy prefiks numerow
 *                     przekierowywanych;
 * @param[in] num2   - wskaznik na napis reprezentujacy prefiks numerow,
 *                     na ktore jest wykonywane przekierowanie.
 * @return  * @return Wartosc @p true, jesli doane są poprawne.
 *         Wartosc @p false – wpp.
 */
static bool isPhfwdAddCorrectInput(PhoneForward *pf, char const *num1, char const *num2) {
    if ((pf == NULL) || (num1 == NULL) || (num2 == NULL)) {
        return false;
    }
    if ((isStringAPhoneNumber(num1) == false) ||
        (isStringAPhoneNumber(num2) == false)) {  // Ktorys z napisow nie jest numerem.
        return false;
    }
    if (strcmp(num1, num2) == 0) {   // Podane numery sa identyczne.
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
        // Tworze nowy wezel, jesli sciezka nie istnieje.
        if (temp->children[code] == NULL) {
            temp->children[code] = newNode();
            if (temp->children[code] == NULL) {
                return false;
            }
            temp->children[code]->forwarding = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam sie do nastepnego wezla.
        temp = temp->children[code];

        //  Przesuwam sie do nastepnego wezla.
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
    // Dodaje przekierowania do drzewa przekierowan forwarding ("odwroconego").
    bool ok = phrevAdd(pf->pfRev, copyNum1, num2);

    if (!ok) {
        return false;
    }
    return true;
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

    pnum->allNumbers = insertToList(pnum->allNumbers, num);   // Dodaje od razu num do ciagu wynikowego.
    PhoneReverse *curr = pf->pfRev;

    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char *secondPart = NULL;
    size_t length;
    const char *lastSign = "\0";
    while (*num != '\0') {
        if (*num) {
            curr = curr->children[get_digit(*num)];  // Ide do nastepnogo wierzcholka
        } else {
            break;
        }
        if (curr == NULL) break;
        num++;              // Przesuwam sie do innego znaku
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