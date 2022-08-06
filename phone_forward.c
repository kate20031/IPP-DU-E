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
 * Przekierowanie 'dakad' przechowywam w forward. (Znajduje sie w synie najmniej
 * znaczacej cyfry przekierowania 'skad'.
 * W pfRev przechowywam drzewo przekierowan odwrotnych (Reverse).
 */
struct PhoneForward {
    struct PhoneForward *children[SIZE]; ///<"dzieci" wierzcholka drzewa.
    struct PhoneForward *parent;    ///<rodzic danego wierzcolka.
    char *forward;  ///<przekierowanie.
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
struct List{
    char* reverse;  ///<przekierowanie(numer).
    struct List* next;  ///<wskaznik na nastepny element.
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
 * przekierowania przechowywam w liscie 'forward'.
 */
struct PhoneReverse {
    struct PhoneReverse *children[SIZE];  ///<"dzieci" wierzcholka drzewa.
    struct PhoneReverse *parent;  ///<Rodzic danego wierzcolka.
    struct List *forward;  ///<Przekierowanie.
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



/** @brief Tworzy nowa strukture (Reverse).
* Tworzy nowa strukture  niezawierajaca zadnych przekierowan.
* @return Wskaznik na utworzona strukture lub NULL, gdy nie udalo sie
*         alokowac pamieci.
*/
PhoneReverse *phrevNew(void) {
    PhoneReverse* phrev = (PhoneReverse*)malloc(sizeof(PhoneReverse));
    if (phrev != NULL) {
        for (int i = 0; i < SIZE; i++) {
            phrev->children[i] = NULL;
        }
        phrev->parent = NULL;
        phrev->forward = NULL;
    }
    return phrev;
}



/** @brief Tworzy nowa strukture.
* Tworzy nowa strukture niezawierajaca zadnych przekierowan.
* @return Wskaznik na utworzona strukture lub NULL, gdy nie udalo sie
*         alokowac pamieci.
*/
PhoneForward *phfwdNew(void) {
    PhoneForward* pf = (PhoneForward*)malloc(sizeof(PhoneForward));
    if (pf != NULL) {
        for (int i = 0; i < SIZE; i++) {
            pf->children[i] = NULL;
        }
        pf->parent = NULL;
        pf->forward = NULL;
        pf->pfRev = phrevNew();
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
        while (current != NULL)
        {
            next = current->next;
            if (current->reverse ) {
                free(current->reverse);
                current->reverse = NULL;
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
static int get_digit(char c){
    if (c == '*') return 10;
    if (c == '#') return 11;
    if ((c == ';') || (c == ':')) return -1;
    return c - '0';
}



/**
 * @brief Porownuje napisy leksykograficznie
 * Zmodyfikowana funkcja strcmp
 * @param p1 - wskaznik na pierwszy napis
 * @param p2 - wskaznik na drugi napis
 * @return int - liczba dodatnia/ujemna/zero w zaleznosci od wyniku porownania
 */
static int compare (const char *p1, const char *p2)
{
    const unsigned char *s1 = (const unsigned char *) p1;
    const unsigned char *s2 = (const unsigned char *) p2;
    unsigned char c1, c2;
    do
    {
        c1 = (unsigned char) *s1++;
        c2 = (unsigned char) *s2++;
        if (c1 == '\0') {
            return get_digit(c1) - get_digit(c2);
        }
    }
    while (get_digit(c1) == get_digit(c2));
    return get_digit(c1) - get_digit(c2);
}


/**
 * @brief Sprawdza czy podany numer zaczyna siÄ™ prefiksem
 *
 * @param number â€“ wskaÅºnik na numer(przekierowanie).
 * @param prefix â€“ wskaÅºnik na prefiks.
 * @return true  â€“ jeÅ›li numer zaczyna siÄ™ przefiksem.
 * @return false â€“  jeÅ›li numer nie zaczyna siÄ™ przefiksem.
 */
static bool hasaPrefix(char* number, const char* prefix) {
    if (strlen(prefix) > strlen(number)) {
        return false;
    }
    char* prefixTemp = (char*)prefix;
    char* numberTemp = number;
    int digit1 = get_digit(*prefixTemp);
    int digit2 = get_digit(*numberTemp);

    while (*prefixTemp != '\0' && (digit1 == digit2)) {

        prefixTemp++;
        numberTemp++;
        digit1 = get_digit(*prefixTemp);
        digit2 = get_digit(*numberTemp);
    }

    if (*prefixTemp != '\0') {
        return false;
    }
    return true;
}


/**
 * @brief Usuwanie z listy przekierowaÅ„, ktÃ³re sÄ… takie same jak "num" (leksykograficznie)
 *
 * @param list â€“ wskaÅ¼nik na listÄ™.
 * @param num â€“ wskaÅºnik na szukane przekierowanie.
 */
static void deleteReverse(List **list, const char* num)
{
    // Usuwam element na poczÄ…tku.
    if (*list && (compare((*list)->reverse, num) == 0))
    {
        List* tmp = *list;
        *list = (*list)->next;
        if (tmp->reverse != NULL) {
            free(tmp->reverse);
            tmp->reverse = NULL;
            free(tmp);
            tmp = NULL;
        }
        return;
    }

    // Usuwam element w Å›rodku \ na koÅ„cu listy.
    for (List * current = *list; current != NULL; current = current->next)
    {
        if (current->next != NULL && current->next->reverse && (compare(current->next->reverse, num) == 0))
        {

            List * tmp = current->next;
            current->next = tmp->next;
            if (tmp->reverse != NULL) {
                free(tmp->reverse);
                tmp->reverse = NULL;
                free(tmp);
                tmp = NULL;
            }
            break;
        }
    }
}


/**
 * @brief Usuwanie z listy przekierowaÅ„ zaczynajÄ…cych siÄ™ przefiksem "num"
 *
 * @param list â€“ wskaÅ¼nik na listÄ™.
 * @param num â€“ wskaÅºnik na przefiks.
 */
static void deleteElemListReverse(List **list, const char* num)
{
    // Usuwam elementy na poczÄ…tku.
    while (*list && hasaPrefix((*list)->reverse, num))
    {
        List * tmp = *list;
        *list = (*list)->next;
        free(tmp->reverse);
        tmp->reverse = NULL;
        free(tmp);
        tmp = NULL;
    }

    // Usuwam element w Å›rodku \ na koÅ„cu listy.
    for (List * current = *list; current != NULL; current = current->next)
    {
        while (current->next != NULL && hasaPrefix(current->next->reverse, num))
        {
            List * tmp = current->next;
            current->next = tmp->next;
            free(tmp->reverse);
            tmp->reverse = NULL;
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
void phrevDelete(PhoneReverse *phrev) {
    if (phrev != NULL) {
        for (int i = 0; i < SIZE; i++) {
            if (phrev->children[i]) phrevDelete(phrev->children[i]);
        }
        // Usuwanie przekierowania (listy)
        listDelete(phrev->forward);
        free(phrev);
        phrev = NULL;
    }
}



/**
 * @brief Rekyrencyjne usuwanie struktury PhoneForward.
 * (Funkcja pomocnicza)
 * @param pf - wskaznik na usuwana strukture.
 */
static void phfwdDeleteRek(PhoneForward *pf) {
    if (pf != NULL) {
        for (int i = 0; i < SIZE; i++) {
            if (pf->children[i]) phfwdDeleteRek(pf->children[i]);
        }
        if (pf->forward != NULL) {
            free(pf->forward);
            pf->forward = NULL;
        }
        free(pf);
        pf = NULL;
    }
}



/** @brief Usuwa strukture PhoneForward.
 * Usuwa strukture wskazywana przez @p pf. Nic nie robi, jesli wskaznik ten ma
 * wartosc NULL.
 * @param[in] pf - wskaznik na usuwana strukture.
 */
void phfwdDelete(PhoneForward *pf) {
    if (pf != NULL) {
        phrevDelete(pf->pfRev);
        phfwdDeleteRek(pf);
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
static int isNumber(const char *num) {
    int length = 0;
    if ((num == NULL) || (num[0] == '\0')) return 0;  // Napis jest pusty albo wskazuje na NULL.

    while (num[length] != '\0') {
        if (isdigit(num[length]) || (get_digit(num[length]) == 10) || (get_digit(num[length]) == 11)) {
            length++;
        } else {          // Natrafilismy na znak rozny od cyfry
            return 0;
        }
    }
    return 1;
}



/**
 * @brief Tworzy i zwraca nowy wierzcholek.
 *
 * @return PhoneForward* nowy wierzcholek.
 */
static PhoneForward* newNode()
{
    PhoneForward* node = (struct PhoneForward*)malloc(sizeof(PhoneForward));
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
static PhoneReverse* newNodeReverse()
{
    PhoneReverse* node = (struct PhoneReverse*)malloc(sizeof(PhoneReverse));
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
static List* insertToList(List *list, const char* num) {
    List *ptr = malloc(sizeof(List));
    if (ptr == NULL) {
        if (*num) {
            free((char*)num);
        }
        return NULL;
    }
    ptr->reverse = malloc(sizeof(char) * (strlen(num) + 1));
    if (ptr->reverse == NULL) {
        free(ptr);
        if (*num) {
            free((char*)num);
        }
        return NULL;
    }
    memcpy(ptr->reverse, (char*)num, sizeof(char) * (strlen(num) + 1));
    ptr->next = NULL;

    if (list == NULL) {
        ptr->next = list;
        return ptr;
    } else if (compare(num, list->reverse) < 0)
    {
        ptr->next = list;
        return ptr;
    } else {
        List *cur = list;
        while (cur->next != NULL && compare(num, cur->next->reverse) >= 0) {
            if (compare(num, cur->next->reverse) == 0)
            {
                free(ptr->reverse);
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
* @param pf - wskaznik na strukture przechowujaca przekierowania
 *                     numerow;
 * @param[in] num1   - wskaznik na napis reprezentujacy prefiks numerow
 *                     przekierowywanych;
 * @param[in] num2   - wskaznik na napis reprezentujacy prefiks numerow
 */
static void phrevAdd(PhoneReverse *pf, char const *num1, char const *num2) {
    struct PhoneReverse* temp = pf;
    while (*num2) {
        int code = get_digit(*num2);
        // Tworze nowy wezel, jesli sciezka nie istnieje
        if (temp->children[code] == NULL) {
            temp->children[code] = newNodeReverse();
            if (temp->children[code] == NULL) return;
            temp->children[code]->forward = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam sie do nastepnego wezla.
        temp = temp->children[code];
        //  Przesuwam sie do nastepnej literki.
        num2++;
    }
    pf = temp;
    pf->forward = insertToList(pf->forward, num1);
}



/**
 * @brief Usuwa przekierowania (za prefiksem) z drzewa odwroconego
 * Usuwa z drzewa wszystkie przekierowania, które "=="
 * leksykograficznie num2.
 * @param pf - wskaznik na drzewo odwrocone.
 * @param num- wskaznik na numer przekierowania "dokad".
 * @param num2- wskaznik na numer przekierowania "skad".
 */
static void phrevRemoveConcrete(PhoneReverse *pf, const char* num, const char* num2) {
    if (pf != NULL) {
        PhoneReverse *curr = pf;
        size_t numberLength = strlen(num);
        for (size_t i = 0; i < numberLength; i++) {
            if (!curr->children[get_digit(*num)]) {
                return;
            }
            curr = curr->children[get_digit(*num)];
            num++;
        }
        if (*num != '\0' && curr->children[get_digit(*num)]) curr = curr->children[get_digit(*num)];
        deleteReverse(&curr->forward, num2);
    }


}



/**
 * @brief Usuwa przekierowania (za prefiksem) z drzewa odwroconego
 * Usuwa z drzewa wszystkie przekierowania, zaczynajÄ…ce siÄ™ podanym prefiksem
 * @param pf - wskaznik na drzewo odwrocone.
 * @param num- wskaznik na numer przekierowania "dokad"
 * @param num2- wskaznik na numer przekierowania "skad" (przefiks)
 */
static void phrevRemove(PhoneReverse *pf, const char* num, const char* num2) {
    if (pf != NULL) {
        PhoneReverse *curr = pf;
        size_t numberLength = strlen(num);
        for (size_t i = 0; i < numberLength; i++) {
            if (!curr->children[get_digit(*num)]) {
                return;
            }
            curr = curr->children[get_digit(*num)];
            num++;
        }
        deleteElemListReverse(&curr->forward, num2);
    }


}



/**
 * @brief Rekursywne usuwanie przekierowanie z drzewa zwyklego
 * Usuwam rekursywnie przekierowanie z drzewa zwyklego,
 * wywoluje w tej funkcji usuwanie przekierowania z drzewa odwroconego.
 */
static void phfwdRemoveRek(PhoneForward *pf, PhoneForward *const_pf, char const* num, List** listOfRemoves) {
    if (pf != NULL) {
        for (int i = 0; i < SIZE; i++) {
            if (pf->children[i]) {
                phfwdRemoveRek(pf->children[i], const_pf, num, listOfRemoves);
            }
            if (pf->forward != NULL) {
                phrevRemove(const_pf->pfRev, pf->forward, num);
                free(pf->forward);
                pf->forward = NULL;
            }
        }
    }
}



/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w ktorych parametr @p num jest prefiksem
 * parametru @p num1 uzytego przy dodawaniu. Jesli nie ma takich przekierowan
 * lub napis nie reprezentuje numeru, nic nie robi.
 * @param[in,out] pf - wskaznik na strukture przechowujaca przekierowania
 *                     numerow;
 * @param[in] num    - wskaznik na napis reprezentujacy prefiks numerow.
 */
void phfwdRemove(PhoneForward *pf, char const *num)
{
    if ((pf != NULL) && (num != NULL) && isNumber(num)) {
        PhoneForward* curr = pf;
        char* tempNum = (char*)num;
        size_t  numberLength = strlen(tempNum);
        for (size_t i = 0; i < numberLength; i++) {
            if (!curr->children[get_digit(*tempNum)]) {
                return;
            }
            curr = curr->children[get_digit(*tempNum)];
            tempNum++;
        }

        List* listOfRemoves = NULL;

        for (int i = 0; i < SIZE; i++) {
            if (curr->children[i] != NULL && curr->children[i]->forward != NULL) {
                phrevRemove(pf->pfRev, curr->children[i]->forward, num);
                free(curr->children[i]->forward);
                curr->children[i]->forward = NULL;
            }
        }

        phfwdRemoveRek(curr, pf, num, &listOfRemoves);
        listDelete(listOfRemoves);
    }
}



/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdluzszego pasujacego
 * prefiksu. Wynikiem jest ciag zawierajacy co najwyzej jeden numer. Jesli dany
 * numerphnumGet(pnum, 0) nie zostal przekierowany, to wynikiem jest ciag zawierajacy ten numer.
 * Jesli podany napis nie reprezentuje numeru, wynikiem jest pusty ciag.
 * Alokuje strukture @p PhoneNumbers, ktora musi byc zwolniona za pomoca
 * funkcji @ref phnumDelete.
 * @param[in] pf  - wskaznik na strukture przechowujaca przekierowania numerow;
 * @param[in] num - wskaznik na napis reprezentujacy numer.
 * @return Wskaznik na strukture przechowujaca ciag numerow lub NULL, gdy nie
 *         udalo sie alokowac pamieci.*/
PhoneNumbers *phfwdGet(PhoneForward const *pf, char const *num) {

    PhoneNumbers *pnum = (PhoneNumbers*)malloc(sizeof(PhoneNumbers));
    if (pnum == NULL) return NULL;
    pnum->allNumbers = NULL;

    if (pf == NULL) {
        free(pnum);
        return NULL;
    }

    if (isNumber(num) == 0) {   // Podany napis nie reprezentuje numeru.
        return pnum;
    }
    // Postepuje analogicznie jak w  phfwdReverse (tam jest krotko opisana metoda szukania przekierowania).
    PhoneForward const* curr = pf;
    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char *maxForward = NULL; // Najdluzsze dotychczasowe przekierowanie/
    char* secondPart = NULL;
    size_t length;
    const char* lastSign = "\0";
    char* numCopy = (char*)malloc(sizeof(char) * (strlen(num) + 1));
    if (numCopy == NULL) return NULL;
    memcpy(numCopy, num, sizeof(char) * (strlen(num)));
    memcpy(numCopy + strlen(num), lastSign, sizeof(char));
    maxForward = (char*)malloc(sizeof(char));
    memcpy(maxForward, "\0", sizeof(char));
    if (maxForward == NULL) return NULL;
    while (*num != '\0') {

        // Ide do nastepnogo wierzcholka
        curr = curr->children[get_digit(*num)];
        if (curr == NULL) {
            break;
        }
        // Przesuwam sie do innego znaku
        num++;
        length = strlen(num)  + 1;
        secondPart = realloc(secondPart, (sizeof(char) * length));
        memcpy(secondPart , (char*)num, sizeof(char) * (length - 1));
        memcpy(secondPart + length - 1, lastSign, sizeof(char));

        if (curr->forward) {
            if (!secondPart) {
                length =  strlen(curr->forward) + 1;
            } else {
                length = strlen(secondPart) + strlen(curr->forward) + 1;
            }
            free(lastForward);
            lastForward = (char*)malloc(sizeof(char) * length);
            memcpy(lastForward, curr->forward, strlen(curr->forward) * sizeof(char));
            if (secondPart) {
                memcpy(lastForward + strlen(curr->forward), secondPart, strlen(secondPart) * sizeof(char));
            }
            memcpy(lastForward + length - 1 , lastSign, sizeof(char));
        }
        if (lastForward != NULL) {
            if (strlen(lastForward) >= strlen(maxForward)) {
                length = strlen(lastForward) + 1;
                free(maxForward);
                maxForward = malloc(sizeof(char) * length);
                memcpy(maxForward, lastForward, length);
            }
        }
    }
    // Wstawiam do listy wynikowej
    if (lastForward != NULL) {
        pnum->allNumbers = insertToList(pnum->allNumbers, lastForward);
    } else {
        pnum->allNumbers = insertToList(pnum->allNumbers, numCopy);
    }
    if (secondPart)  free(secondPart);
    if (lastForward) free(lastForward);
    if (maxForward)  free(maxForward);
    free(numCopy);

    return pnum;
}



/** @brief Usuwa strukture.
 * Usuwa strukture wskazywana przez @p pnum. Nic nie robi, jesli wskaznik ten ma
 * wartosc NULL.
 * @param[in] pnum - wskaznik na usuwana strukture.
 */
void phnumDelete(PhoneNumbers *pnum)
{
    if (pnum != NULL) {
        // Usuwanie przekierowania (listy)
        listDelete(pnum->allNumbers);
        // Usuwam podstrukture i strukture.
        free(pnum);
        pnum = NULL;
    }
}



/** @brief Udostepnia numer.
 * Udostepnia wskaznik na napis reprezentujacy numer. Napisy sa indeksowane
 * kolejno od zera.
 * @param[in] pnum - wskaznik na strukture przechowujaca ciag numerow telefonow;

 * @param[in] idx  - indeks numeru telefonu.
 * @return Wskaznik na napis reprezentujacy numer telefonu. Wartosc NULL, jesli
 *         wskaznik @p pnum ma wartosc NULL lub indeks ma za duza wartosc.
 */
char const *phnumGet(PhoneNumbers const *pnum, size_t idx)
{
    if ((pnum != NULL)) {
        char const *p = NULL;
        if (pnum->allNumbers != NULL) {
            List *current = pnum->allNumbers;
            int i = 0;
            while (current != NULL && i <= (int)idx)
            {
                if (i == (int)idx) {
                    p = current->reverse;
                    break;
                }
                current = current->next;
                i++;
            }
        }
        return p;
    }
    else {
        return NULL;
    }
}



/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerow majacych prefiks @p num1, na numery,
 * w ktorych ten prefiks zamieniono odpowiednio na prefiks @p num2. Kazdy numer
 * jest swoim wlasnym prefiksem. Jesli wczesniej zostalo dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastepowane.
 * Relacja przekierowania numerow nie jest przechodnia.
 * @param[in,out] pf - wskaznik na strukture przechowujaca przekierowania
 *                     numerow;
 * @param[in] num1   - wskaznik na napis reprezentujacy prefiks numerow
 *                     przekierowywanych;
 * @param[in] num2   - wskaznik na napis reprezentujacy prefiks numerow,
 *                     na ktore jest wykonywane przekierowanie.
 * @return Wartosc @p true, jesli przekierowanie zostalo dodane.
 *         Wartosc @p false, jesli wystapil blad, np. podany napis nie
 *         reprezentuje numeru, oba podane numery sa identyczne lub nie udalo
 *         sie alokowac pamieci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2)
{
    if ((pf == NULL) || (num1 == NULL) || (num2 == NULL)) {
        return false;
    }
    if ((isNumber(num1) == 0) || (isNumber(num2) == 0)) {  // Ktorys z napisow nie jest numerem.
        return false;
    }
    if (strcmp(num1, num2) == 0) {   // Podane numery sa identyczne.
        return false;
    }
    struct PhoneForward* temp = pf;
    char const *copyNum1 = (char const*)malloc(sizeof(char) * (strlen(num1) + 100000000));
    memcpy((char*)copyNum1, num1, sizeof(char) * (strlen(num1) + 1));
    while (*num1) {
        int code = get_digit(*num1);
        // Tworze nowy wezel, jesli sciezka nie istnieje.
        if (temp->children[code] == NULL) {
            temp->children[code] = newNode();
            if (temp->children[code] == NULL) {
                return false;
            }
            temp->children[code]->forward = NULL;
            temp->children[code]->parent = temp;
        }
        // Przesuwam sie do nastepnego wezla.
        temp = temp->children[code];

        //  Przesuwam sie do nastepnego wezla.
        num1++;
    }

    if (temp->forward) {
        phrevRemoveConcrete(pf->pfRev, temp->forward, copyNum1);
        free(temp->forward);
        temp->forward = NULL;
    }
    temp->forward = (char*)malloc(sizeof(char)*(strlen(num2)+1));
    if (temp->forward == NULL){
        copyNum1 = NULL;
        return false;
    }
    strcpy(temp->forward, num2);
    phrevAdd(pf->pfRev, copyNum1, num2);   // Dodaje przekierowania do drzewa przekierowan reverse ("odwroconego").
    free((char*)copyNum1);
    return true;
}



/** @brief Wyznacza przekierowania na dany numer.
 * Wyznacza nastepujacy ciag numerow: jesli istnieje numer @p x, taki ze wynik
 * wywolania @p phfwdGet z numerem @p x zawiera numer @p num, to numer @p x
 * nalezy do wyniku wywolania @ref phfwdReverse z numerem @p num. Dodatkowo ciag
 * wynikowy zawsze zawiera tez numer @p num. Wynikowe numery sa posortowane
 * leksykograficznie i nie moga sie powtarzac. Jesli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciag. Alokuje strukture
 * @p PhoneNumbers, ktora musi byc zwolniona za pomoca funkcji @ref phnumDelete.
 * @param[in] pf  - wskaznik na strukture przechowujaca przekierowania numerow;
 * @param[in] num -
         pnum->allNumbers[idx] wskaznik na napis reprezentujacy numer.
 * @return Wskaznik na strukture przechowujaca ciag numerow lub NULL, gdy nie
 *         udalo sie alokowac pamieci.
 */
PhoneNumbers* phfwdReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *pnum = (PhoneNumbers*)malloc(sizeof(PhoneNumbers));

    if (pnum == NULL) return NULL;
    pnum->allNumbers = NULL;

    if (pf == NULL) {
        free(pnum);
        return NULL;
    }
    if (isNumber(num) == 0) {   // Podany napis nie reprezentuje numeru.
        return pnum;
    }
    // Wynikowe przekierowanie reverse sklada sie z 2 czesci :
    // druga "secondPart" - odpowiednia poczatkowa czesc "num",
    // pierwsza - zamieniona na (jesli znaleziono) przekierowanie
    // odpowiednia poczatkowa czesc "num".
    pnum->allNumbers = insertToList(pnum->allNumbers, num);   // Dodaje od razu num do ciagu wynikowego.
    PhoneReverse * curr = pf->pfRev;
    char *lastForward = NULL; // Ostatnie znalezione przekierowanie.
    char* secondPart = NULL;
    size_t length;
    const char* lastSign = "\0";
    while (*num != '\0') {
        // Ide do nastepnogo wierzcholka
        if (*num) {
            curr = curr->children[get_digit(*num)];
        } else {
            break;
        }
        if (curr == NULL) break;
        // Przesuwam sie do innego znaku
        num++;
        length = strlen(num)  + 1;
        secondPart = realloc(secondPart, (sizeof(char) * length));
        memcpy(secondPart, (char*)num, sizeof(char) * (length - 1));
        memcpy(secondPart + length - 1, lastSign, sizeof(char));
        List* currList = curr->forward;
        while (currList) {
            if (!secondPart) {
                length =  strlen(currList->reverse) + 1;
            } else {
                length = strlen(secondPart) + strlen(currList->reverse) + 1;
            }
            free(lastForward);
            lastForward = (char*)malloc(sizeof(char) * length);
            memcpy(lastForward, currList->reverse, strlen(currList->reverse) * sizeof(char));
            if (secondPart) {
                memcpy(lastForward + strlen(currList->reverse), secondPart, strlen(secondPart) * sizeof(char));
            }
            memcpy(lastForward + length - 1 , lastSign, sizeof(char));
            currList = currList->next;
            if (lastForward != NULL) {
                pnum->allNumbers = insertToList(pnum->allNumbers, lastForward);
            }
        }
    }
    if (secondPart)  free(secondPart);
    if (lastForward) free(lastForward);

    return pnum;
}



/**
 * @brief Wyznacza numery przechodzÄ…ce na podany argument
 *
 * @param pf - wskaznik na baze przekierowaÅ„
 * @param num - wskaznik na numer na ktÃ³ry szukamy przekierowanie
 * @return PhoneNumbers* - zwraca posortowanÄ… leksykograficznie listÄ™ wszystkich takich numerÃ³w telefonÃ³w
 * lub NULL, gdy nie udaÅ‚o siÄ™ alokowaÄ‡ pamiÄ™ci.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num) {
    PhoneNumbers *pnum = (PhoneNumbers*)malloc(sizeof(PhoneNumbers));
    if (pnum == NULL) return NULL;
    pnum->allNumbers = NULL;

    if (pf == NULL) {
        free(pnum);
        return NULL;
    }
    if (isNumber(num) == 0) {   // Podany napis nie reprezentuje numeru.
        return pnum;
    }

    PhoneNumbers* pnum1;
    pnum1 = phfwdReverse(pf, num);
    List* temp  = pnum1->allNumbers;

    while (temp != NULL) {
        PhoneNumbers *pnum2;
        pnum2 = phfwdGet(pf, temp->reverse);
        List* curr = temp->next;
        if (strcmp(pnum2->allNumbers->reverse, num) != 0) {
            if (pnum1->allNumbers) {
                deleteReverse(&(pnum1->allNumbers), temp->reverse);
            }
        }
        temp = curr;
        phnumDelete(pnum2);
    }
    pnum->allNumbers = pnum1->allNumbers;
    free(pnum1);

    return pnum;
}