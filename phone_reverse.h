#ifndef PHONE_REVERSE_H
#define PHONE_REVERSE_H

#define CHILDREN_NUMB 12 ///<Rozmiar drzewa
#include <stdbool.h>
#include "phnum.h"



/**
 * @brief Struktura przekierowań odwróconych.
 * Trzymam drzewo odwrócone przekierowań.
 * Skoro przekierowań 'dokąd' może byc kilka,
 * przekierowania przechowuję w liście 'forwarding'.
 */
struct PhoneReverse {
    struct PhoneReverse *children[CHILDREN_NUMB];  ///<"dzieci" wierzchołka drzewa.
    struct PhoneReverse *parent;  ///<Rodzic danego wierzchołka.
    struct List *listOfFrwd;  ///<Przekierowanie.
};
/**
 * @brief To jest typ PhoneReverse
 *
 */
typedef struct PhoneReverse PhoneReverse;


/**
 * @brief Dodaje przekierowanie(odwrócone).
 *
* @param pfRev - wskaźnik na strukturę przechowująca przekierowania
 *                     numerów;
 * @param[in] num1   - wskaźnik na napis reprezentujący prefiks numerów
 *                     przekierowywanych;
 * @param[in] num2   - wskaźnik na napis reprezentujący prefiks numerów
 *
 *  @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. podany napis nie
 *         reprezentuje numeru, oba podane numery sa identyczne lub nie udało
 *         sie alokować pamięci.
 */
bool phrevAdd(PhoneReverse *pfRev, char const *num1, char const *num2);


/**
 * @brief Usuwa przekierowania z drzewa odwróconego
 * Usuwa z drzewa wszystkie przekierowania, które "=="
 * leksykograficznie num2.
 * @param pfRev - wskaźnik na drzewo odwrócone.
 * @param num1 - wskaźnik na numer przekierowania "dokąd".
 * @param num2- wskaźnik na numer przekierowania "skąd".
 */
void phrevRemove(PhoneReverse *pfRev, const char *num1, const char *num2);


/**
 * @brief Usuwa przekierowania (za prefiksem) z drzewa odwróconego
 * Usuwa z drzewa wszystkie przekierowania, zaczynające się podanym prefiksem
 * @param pfRev - wskaźnik na drzewo odwrócone.
 * @param num1- wskaźnik na numer przekierowania "dokąd"
 * @param num2- wskaźnik na numer przekierowania "skąd" (prefiks)
 */
void phrevRemoveNumStartsWithPref(PhoneReverse *pfRev, const char *num1, const char *num2);


/**
 * @brief Tworzy nowa strukturę drzewa odwróconego
 * @return wskaźnik na utworzona strukturę drzewa odwróconego
 */
PhoneReverse *phrevNew(void);


/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywana przez @p phrev. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL. (Funkcja pomocnicza do usuwania drzewa odwróconego).
 * @param[in] phrev - wskaźnik na usuwana strukturę.
 */
void deleteReverseTree(PhoneReverse *phrev);



#endif //PHONE_REVERSE_H