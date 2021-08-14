#include <QCoreApplication>
#include <stdint.h>
#include <string>
#include <iostream>

#define N_DESK 8 // Размерность доски
#define MAX_MOVES 64 // Максимальное количество ходов конём
#define MAX_WARRING_MOVES 32 // Максимальное найденное количество ходов, после которого у пользователя спрашивается, нужно ли продолжать расчёт
#define SYSTEM_ERROR -1 // Ошибка работы главного тела программы
#define CALCULATION_ERROR -2 // Ошибка при вычислении
#define NOT_RESULT_ERROR -3 // Не был вычислен результат
#define GET_NUMBER(pos)  ((pos[0] - notate[0])*8 + pos[1]-48) // Преоводит число из символа в ASII  в целочисленное однобайтовое значение
#define GET_WIDTH(k)  (k % 8) // Вычисляет из порядкового номера клетки её номер в текущей строке
#define GET_HEIGHT(k)  (k/8) // Вчисляет номер строки
#define to8b(str) QString::fromUtf8(str).toLocal8Bit().data() // Переводи строку в utf-8
uint8_t is_warring; // Флаг, говорящий о том, что глубина рекурсии достигла того уровня, когда у пользователя срашивается продолжать ли вычисление
const char  notate[8] = { 'a','b','c','d','e','f','g','h' };  // таблица символов столбцов шахматной доски


/*
Функция возвращает переменную с битовыми флагами (n), говорящих о том,какой ход возможен для коня
uint8_t k - номер клетки, в которой находится конь
uint8_t &n - возвращаемое значение битовых флагов, которые определяют номера ходов по порядку от 1 - го до 8-го.

  2   3
1      4
    k
5      7
 6    8
*/
void getCorrectMove(uint8_t k, uint8_t &n) {
    uint8_t k_w = GET_WIDTH(k);
    uint8_t k_h = GET_HEIGHT(k);
    n = 0;
    if (k_w > 2 && k_h < 8)
        n |= 1 << 1;
    if (k_w > 1 && k_h < 7)
        n |= 1 << 2;
    if (k_w < 8 && k_h < 7)
        n |= 1 << 3;
    if (k_w < 7 && k_h < 8)
        n |= 1 << 4;
    if (k_w > 2 && k_h >1)
        n |= 1 << 5;
    if (k_w > 1 && k_h >2)
        n |= 1 << 6;
    if (k_w < 8 && k_h >2)
        n |= 1 << 7;
    if (k_w < 7 && k_h < 1)
        n |= 1 << 8;
}


/*
Рекурсивная функция вычисления кротчайшего пути
uint8_t k1 - порядковый номер текущего положения коня
uint8_t k2 - конечное положение коня
uint64_t k_flags - битовые флаги порядковых номеров, в которых конь уже был
uint8_t count - текущий ход конём
uint8_t cur_min - переменная, хранящая в себе текущий минимум
*/
uint8_t calk_knight(uint8_t k1, uint8_t k2, uint64_t k_flags, uint8_t count, uint8_t cur_min) {
    if (k1 == k2)
        return count;
    if (!is_warring && count > cur_min) {
        char o;
        std::cout <<to8b( "Вычисление может занять долгое время, хотите продолжить?(y/n):");
        std::cin >> o;
        if (o == 'n' || o == 'N') {
            is_warring = 1;
            return cur_min;
        }
        is_warring = 2;
    }

    if (is_warring && count > MAX_WARRING_MOVES) {
        return cur_min;
    }

    if (count > cur_min)
        return count;
    k_flags |= 1 << k1;
    if (count >= MAX_MOVES)
        return 0xFF;
    uint8_t n_flags;
    getCorrectMove(k1, n_flags);
    uint8_t hods[8];
    memset(hods, 0xFF, 8);
    if (n_flags & (1 << 1) && ~(k_flags & (k1 + 8 - 2)))
        hods[0] = calk_knight(k1 + 8 - 2, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[0]) cur_min = hods[0];
    if (n_flags & (1 << 2) && ~(k_flags & (k1 + 8 * 2 - 1)))
        hods[1] = calk_knight(k1 + 8 * 2 - 1, k2, k_flags, count + 1,cur_min);
    if(cur_min > hods[1]) cur_min = hods[1];
    if (n_flags & (1 << 3) && ~(k_flags & (k1 + 8 * 2 + 1)))
        hods[2] = calk_knight(k1 + 8 * 2 + 1, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[2]) cur_min = hods[2];
    if (n_flags & (1 << 4) && ~(k_flags & (k1 + 8 + 2)))
        hods[3] = calk_knight(k1 + 8 + 2, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[3]) cur_min = hods[3];
    if (n_flags & (1 << 5) && ~(k_flags & (k1 - 8 - 2)))
        hods[4] = calk_knight(k1 - 8 - 2, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[4]) cur_min = hods[4];
    if (n_flags & (1 << 6) && ~(k_flags & (k1 - 8 * 2 - 1)))
        hods[5] = calk_knight(k1 - 8 * 2 - 1, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[5]) cur_min = hods[5];
    if (n_flags & (1 << 7) && ~(k_flags & (k1 - 8 * 2 + 1)))
        hods[6] = calk_knight(k1 - 8 * 2 + 1, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[6]) cur_min = hods[6];
    if (n_flags & (1 << 8) && ~(k_flags & (k1 - 8 + 2)))
        hods[7] = calk_knight(k1 - 8 + 2, k2, k_flags, count + 1,cur_min);
    if (cur_min > hods[7]) cur_min = hods[7];

    return cur_min;

}
/*
    Функция вычисления минимального количества ходов из pos1 в  pos2
    std::string pos1 - текущее положение коня
    std::string pos2 - конечное положение коня
*/
int knight(std::string pos1, std::string pos2) {
    try {
        uint8_t k1 = GET_NUMBER(pos1);
        uint8_t k2 = GET_NUMBER(pos2);
        uint64_t k_flags = 0;
        k_flags |= 1 << k2;
        is_warring = false;
        return calk_knight(k1, k2, k_flags, 0,0xFF);

    }
    catch (...) {
        return CALCULATION_ERROR;
    }
}

/*
Функция проверки на корректность введённой строки координат на шахматной доске
pos - строка с координатами на доске
*/
bool isCorrectCoordinate(std::string pos) {
    if (pos.size() != 2)
        return false;
    if (pos[1] - 48 < 1 || pos[1] - 48 > 8)
        return false;
    for (uint8_t i = 0; i<8; i++)
        if (pos[0] == notate[i])
            return true;
    return false;

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    setlocale(LC_ALL, "");
    std::string pos1, pos2;
    int er = 0;
    try {
        while (true) {
            std::cout << to8b("Введите координаты коня:");
            std::cin >> pos1;
            if (!isCorrectCoordinate(pos1)) {
                std::cout << to8b("Введены не корректные координаты на шахматной доске! Попробуйте снова.")<< std::endl;
                continue;
            }
            std::cout << to8b("Введите координаты конечного положения коня:");
            std::cin >> pos2;
            if (!isCorrectCoordinate(pos2)) {
                std::cout << to8b("Введены не корректные координаты на шахматной доске! Попробуйте снова.")<< std::endl;
                continue;
            }
            int out = knight(pos1, pos2);
            if (out < 0) {
                if (out == NOT_RESULT_ERROR) {
                    std::cout << to8b("Не удалось выислить результат.")<< std::endl;
                    continue;
                }
                if (out == CALCULATION_ERROR) { // По хорошему, если выстрелело исключение при вычислении, то надо завершать работу. Это может быть исключение при работе со стеками.
                    er = out;
                    break;
                }
            }
            if (out != 0xFF)
                std::cout << to8b( "Минимальное количество ходов: ")<< out<< std::endl;
            else
                std::cout << to8b("Решение не было найдено.")<< std::endl;
            char o;
            std::cout << to8b("Хотите продолжить?(y/n):");
            std::cin >> o;
            if (o == 'n' || o == 'N')
                break;
        }
    }

    catch (...) {
        er = SYSTEM_ERROR;
    }

    switch (er) {
    case CALCULATION_ERROR: std::cout <<  "Произошла ошибка при вычислении.";
    case SYSTEM_ERROR: std::cout <<  "Произошла ошибка работы программы.";
    }

    return a.exec();
}
