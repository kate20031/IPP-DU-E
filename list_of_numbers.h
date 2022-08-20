#ifndef LIST_OF_NUMBERS_H
#define LIST_OF_NUMBERS_H
#include "phone_reverse.h"



/**
 * @brief Lista przekierowań.
 * Przechowuje przekierowania (jest używana dla pfRev i w funkcji phfwdGet).
 */
struct List {
    char *forwarding;  ///<przekierowanie(numer).
    struct List *next;  ///<wskaźnik na następny element.
};
/**
 * @brief to jest typ Lista
 *
 */
typedef struct List List;


/**
 * @brief Dodaje przekierowanie (odwrócone) do listy
 * Dodaje przekierowanie (odwrócone) do listy posortowanej leksykograficznie
 * W wierzchołku pod numerem (numeruje od 0 do CHILDREN_NUMB - 1) ostatniej cyfry przekierowania "dokąd" trzymam
 * listę numerów przekierowań "skąd".
 * @param list - wskaźnik na listę z których sa przekierowania
 * @param num  - wskaźnik na napis do którego jest przekierowanie
 */
List *insertToList(List *list, const char *num);


/**
 * @brief Usuwanie z listy przekierowań, które sa takie same jak "num" (leksykograficznie)
 *
 * @param list - wskaźnik na listę.
 * @param num - wskaźnik na szukane przekierowanie.
 */
void deleteFrwdFromList(List **list, const char *num);


/**
 * @brief Usuwanie z listy przekierowań zaczynających sie prefiksem "prefix"
 *
 * @param list - wskaźnik na listę.
 * @param prefix - wskaźnik na prefiks.
 */
void deleteFrwdStartsWthPref(List **list, const char *prefix);


/**
 * @brief Funkcja znajduje i zwraca listę przekierowań
 * Funkcja znajduje i zwraca listę przekierowań w drzewie odwróconym
 * za podanym prefiksem.
 * @param pfRev - wskaźnik na drzewo odwrócone.
 * @param num - wskaźnik na numer przekierowania "skąd".
 * @return lista przekierowań "dokąd"
 */
List **getListOfForwardings(PhoneReverse *pfRev, const char *num);


/**
 * @brief Usuwa listę
 *
 * @param list - wskaźnik na listę
 */
void listDelete(List *list);


#endif //LIST_OF_NUMBERS_H