/** @file
 * Interfejs klasy przechowujacej przekierowania numerow telefonicznych
 *
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef __PHONE_FORWARD_H__
#define __PHONE_FORWARD_H__

#include <stdbool.h>
#include <stddef.h>

/**
 * To jest struktura przechowujaca przekierowania numerow telefonow.
 */
struct PhoneForward;
/**
 * To jest typ PhoneForward.
 */
typedef struct PhoneForward PhoneForward;

/**
 * To jest struktura przechowujaca ciag numerow telefonow.
 */
struct PhoneNumbers;
/**
 * To jest typ PhoneNumbers.
 */
typedef struct PhoneNumbers PhoneNumbers;

/** @brief Tworzy nowa strukture.
 * Tworzy nowa strukture niezawierajacazadnych przekierowan.
 * @return Wskaznik na utworzona strukture lub NULL, gdy nie udalo sie
 *         alokowac pamieci.
 */
PhoneForward * phfwdNew(void);

/** @brief Usuwa strukture.
 * Usuwa strukture wskazywana przez @p pf. Nic nie robi, jesli wskaznik ten ma
 * wartosc NULL.
 */
void phfwdDelete(PhoneForward *pf);

/** @brief Dodaje przekierowanie.
 * Dodaje przekierowanie wszystkich numerow majacych prefiks @p num1, na numery,
 * w ktorych ten prefiks zamieniono odpowiednio na prefiks @p num2. Kazdy numer
 * jest swoim wlasnym prefiksem. Jesli wczesniej zostalo dodane przekierowanie
 * z takim samym parametrem @p num1, to jest ono zastepowane.
 * Relacja przekierowania numerow nie jest przechodnia.       
 * @return Wartosc @p true, jesli przekierowanie zostalo dodane.
 *         Wartosc @p false, jesli wystapil blad, np. podany napis nie
 *         reprezentuje numeru, oba podane numery sa identyczne lub nie udalo
 *         sie alokowac pamieci.
 */
bool phfwdAdd(PhoneForward *pf, char const *num1, char const *num2);

/** @brief Usuwa przekierowania.
 * Usuwa wszystkie przekierowania, w ktorych parametr @p num jest prefiksem
 * parametru @p num1 uzytego przy dodawaniu. Jesli nie ma takich przekierowan
 * lub napis nie reprezentuje numeru, nic nie robi.
 */
void phfwdRemove(PhoneForward *pf, char const *num);

/** @brief Wyznacza przekierowanie numeru.
 * Wyznacza przekierowanie podanego numeru. Szuka najdluzszego pasujacego
 * prefiksu. Wynikiem jest ciag zawierajacy co najwyzej jeden numer. Jesli dany
 * numer nie zostal przekierowany, to wynikiem jest ciag zawierajacy ten numer.
 * Jesli podany napis nie reprezentuje numeru, wynikiem jest pusty ciag.
 * Alokuje strukture @p PhoneNumbers, ktora musi byc zwolniona za pomoca
 * funkcji @ref phnumDelete.
 * @return Wskaznik na strukture przechowujaca ciag numerow lub NULL, gdy nie
 *         udalo sie alokowac pamieci.
 */
PhoneNumbers * phfwdGet(PhoneForward const *pf, char const *num);

/** @brief Wyznacza kandydatow na przekierowania na dany numer.
 * Wyznacza nastepujacy ciag numerow: jesli istnieje numer @p x, taki ze wynik
 * wywolania @p phfwdGet z numerem @p x zawiera numer @p num, to numer @p x
 * nalezy do wyniku wywolania @ref phfwdReverse z numerem @p num. Dodatkowo ciag
 * wynikowy zawsze zawiera tez numer @p num. Wynikowe numery sa posortowane
 * leksykograficznie i nie moga sie powtarzac. Jesli podany napis nie
 * reprezentuje numeru, wynikiem jest pusty ciag. Alokuje strukture
 * @p PhoneNumbers, ktora musi byc zwolniona za pomoca funkcji @ref phnumDelete.
 * @return Wskaznik na strukture przechowujaca ciag numerow lub NULL, gdy nie
 *         udalo sie alokowac pamieci.
 */
PhoneNumbers * phfwdReverse(PhoneForward const *pf, char const *num);

/** @brief Usuwa strukture.
 * Usuwa strukture wskazywana przez @p pnum. Nic nie robi, jezeli wskaznik ten ma
 * wartosc NULL.
 */
void phnumDelete(PhoneNumbers *pnum);

/** @brief Udostepnia numer.
 * Udostepnia wskaznik na napis reprezentujacy numer. Napisy sa indeksowane
 * kolejno od zera.
 * @return Wskaznik na napis reprezentujacy numer telefonu. Wartosc NULL, jezeli
 *         wskaznik @p pnum ma wartosc NULL lub indeks ma zanadto duza wartorsc.
 */
char const * phnumGet(PhoneNumbers const *pnum, size_t idx);

/**
 * @brief Wyznacza numery przechodzące na podany argument
 * 
 * @param pf - wskaznik na baze przekierowań
 * @param num - wskaznik na numer na który szukamy przekierowanie
 * @return PhoneNumbers* - zwraca posortowaną leksykograficznie listę wszystkich takich numerów telefonów
 * lub NULL, gdy nie udało się alokować pamięci.
 */
PhoneNumbers * phfwdGetReverse(PhoneForward const *pf, char const *num);

#endif /* __PHONE_FORWARD_H__ */