/** @file
 * Interfejsu opisujuący strukturę, i operacji z nią związane,
 * która przechowuje otrzymane przekierowania numerów telefonicznych.
 *
 * @author Kateryna Pavlichenko <marpe@mimuw.edu.pl>
 * @author Marcin Peczarski <marpe@mimuw.edu.pl>
 * @copyright Uniwersytet Warszawski
 * @date 2022
 */

#ifndef PHNUM_H
#define PHNUM_H
#include <stddef.h>



/**
 * @brief Przechowuje strukturę numerów przekierowanych.
 * Przechowuję wszystkie wynikowe przekierowania w liście
 */
struct PhoneNumbers {
    struct List *allNumbers;  ///<wskaźnik na listę numerów.
};
/**
 * @brief To jest typ PhoneNumbers.
 *
 */
typedef struct PhoneNumbers PhoneNumbers;


/** @brief Udostępnia numer.
 * Udostępnia wskaźnik na napis reprezentujący numer. Napisy sa indeksowane
 * kolejno od zera.
 * @param[in] pnum - wskaźnik na strukturę przechowująca ciąg numerów telefonów;

 * @param[in] idx  - indeks numeru telefonu.
 * @return Wskaźnik na napis reprezentujący numer telefonu. Wartość NULL, jeśli
 *         wskaźnik @p pnum ma wartość NULL lub indeks ma za duża wartość.
 */
char const *phnumGet(PhoneNumbers const *pnum, size_t idx);


/** @brief Usuwa strukturę.
 * Usuwa strukturę wskazywana przez @p pnum. Nic nie robi, jeżeli wskaźnik ten ma
 * wartość NULL.
 */
void phnumDelete(PhoneNumbers *pnum);


#endif //PHNUM_H
