/** @file
 * Interfejs klasy przechowującej przekierowania numerów telefonicznych
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__
#include <stdbool.h>
#include <stddef.h>
#include "phone_reverse.h"



/**
 * @brief Struktura do przechowywania przekierowań.
 *  Przechowuję przekierowania w drzewie tries.
 * Drzewo ma 12 dzieci (od 0 do 11). 10 - to '*', 11 - to '*',
 * a pozostałe cyferki sa sa odpowiednikami cyfr w numerze.
 * Przekierowanie 'dokąd' przechowuję w forwarding. (Znajduje sie w synie najmniej
 * znaczącej cyfry przekierowania 'skąd'.
 * W pfRev przechowuje drzewo przekierowań odwrotnych (Reverse).
 */
struct PhoneForward {
    struct PhoneForward *children[CHILDREN_NUMB]; ///<"dzieci" wierzchołka drzewa.
    struct PhoneForward *parent;    ///<rodzic danego wierzchołka.
    char *forwarding;  ///<przekierowanie.
    struct PhoneReverse *pfRev; ///<struktura przekierowań odwróconych (Reverse).
};
/**
 * @brief to jest typ PhoneForward
 *
 */
typedef struct PhoneForward PhoneForward;


/** @brief Tworzy nowa strukturę.
 * Tworzy nowa strukturę niezawierająca żadnych przekierowań.
 * @return Wskaźnik na utworzona strukturę lub NULL, gdy nie udało sie
 *         alokować pamięci.
 */
PhoneForward * phfwdNew(void);


/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywana przez @p pf. Nic nie robi, jeśli wskaźnik ten ma
 * wartość NULL.
 */
void phfwdDelete(PhoneForward *pf);


/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerów mających prefiks @p num1, na numery,
 * w których ten prefiks zamieniono odpowiednio na prefiks @p num2. Każdy numer
 * jest swoim własnym prefiksem. Jeśli wcześniej zostało dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastępowane.
 * Relacja przekierowania numerów nie jest przechodnia.
 * @return Wartość @p true, jeśli przekierowanie zostało dodane.
 *         Wartość @p false, jeśli wystąpił błąd, np. podany napis nie
 *         reprezentuje numeru, oba podane numery sa identyczne lub nie udało
 *         sie alokować pamięci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);


/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w których parametr @p num jest prefiksem
 * parametru @p num1 użytego przy dodawaniu. Jeśli nie ma takich przekierowań
 * lub napis nie reprezentuje numeru, nic nie robi.
 */
void phfwdRemove(PhoneForward *pf, char const *num);


/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdłuższego pasującego
 * prefiksu. Wynikiem jest ciąg zawierający co najwyżej jeden numer. Jeśli dany
 * numer nie został przekierowany, to wynikiem jest ciąg zawierający ten numer.
 * Jeśli podany napis nie reprezentuje numeru, wynikiem jest pusty ciąg.
 * Alokuje strukturę @p PhoneNumbers, która musi byc zwolniona za pomocą
 * funkcji @ref phnumDelete.
 * @return Wskaźnik na strukturę przechowująca ciąg numerów lub NULL, gdy nie
 *         udało sie alokować pamięci.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num);


/** @brief Wyznacza kandydatów na przekierowania na dany numer.
 * Wyznacza następujący ciąg numerów: jeśli istnieje numer @p x, taki ze wynik
 * wywołania @p phfwdGet z numerem @p x zawiera numer @p num, to numer @p x
 * należy do wyniku wywołania @ref phfwdReverse z numerem @p num. Dodatkowo ciąg
 * wynikowy zawsze zawiera tez numer @p num. Wynikowe numery sa posortowane
 * leksykograficznie i nie mogą sie powtarzać. Jeśli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciąg. Alokuje strukturę
 * @p PhoneNumbers, która musi byc zwolniona za pomocą funkcji @ref phnumDelete.
 * @return Wskaźnik na strukturę przechowująca ciąg numerów lub NULL, gdy nie
 *         udało sie alokować pamięci.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num);


/** @brief Wyznacza numery przechodzące na podany argument
 * @param pf - wskaźnik na bazę przekierowań.
 * @param num - wskaźnik na numer na który szukamy przekierowanie.
 * @return PhoneNumbers* - zwraca posortowana leksykograficznie listę wszystkich takich numerów telefonów
 * lub NULL, gdy nie udało sie alokować pamięci.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num);


/**
 * @brief Zwraca liczbowa postać znaku.
 *
 * @param c - znak.
 * @return int liczbowa postać znaku.
 */
int get_digit(char c);

/**
 * @brief Sprawdza czy napis jest numerem
 * Sprawdza czy podany napis składa sie tylko z cyfr.
 * @param[in] num  -  Sprawdzany napis.
 * @return int 0, jeżeli napis nie jest numerem (zawiera inny znak niz cyfra
 *                  lub jest pusty).
 *         int 1, jeżeli napis jest numerem.
 */
bool isStringAPhoneNumber(const char *num);


/**
 * @brief Funkcja tworzy przekierowanie, kopiuję go zawartość do "lastForward"
 * @param firstPart – pierwsza część tworzonego przekierowania.
 * @param secondPart – druga część tworzonego przekierowania.
 * @param lastForward – ostatnie znalezione przekierowanie.
 */
void createAForward(char *const *firstPart, char **secondPart, char **lastForward);



#endif /* __PHONE_FORWARD_H__ */