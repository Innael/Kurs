#include <iostream>
#include <windows.h>
#include "AllroadBoots.h"
#include "Camel.h"
#include "Centavr.h"
#include "FastCamel.h"
#include "Broom.h"
#include "Eagle.h"
#include "FlyCarpet.h"
#include "Racer.h"
#include "Race.h"

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int tg = 0, trassa = 0, b = 0, itter = 0;
    int uchastniki[7]{};

    std::cout << "Добро пожаловать, в гоночный симулятор!\n 1. Гонка для наземного транспорта \n 2. Гонка для воздушного транспорта \n 3. Гонка для наземного и воздушного транспорта\n";
    for (int f = 0;;) {
        for (int i = 0; i < 2;) {
            std::cout << "Выберите тип гонки: ";
            std::cin >> tg;
            if (tg > 0 && tg < 4)
                i = 2;
        }

        for (int i = 0; i < 2;) {
            std::cout << "Укажите длину дистанции (должна быть положительной): ";
            std::cin >> trassa;
            if (trassa > 0)
                i = 2;
        }

        std::cout << "Должно быть зарегестрировано, хотя бы 2 транспортных средства\n 1. Зарегестрировать транспорт\n";

        for (int i = 0; i < 2;) {
            int a = 0;
            std::cout << "Выберите действие: ";
            std::cin >> a;
            if (a == 1)
                i = 2;
        }

        for (int r = 0;; ++r) {
            int v = 0;
            switch (tg) {
            case 1: std::cout << "Гонка для наземного транспорта. Расcтояние " << trassa << "\n"; break;
            case 2: std::cout << "Гонка для воздушного транспорта. Расcтояние " << trassa << "\n"; break;
            case 3: std::cout << "Гонка для наземного и воздушного транспорта. Расстояние " << trassa << "\n"; break;
            }
                                  
            if (r > 0) {
                std::cout << "Зарегестрированные транспортные средства:";
            }

            for (int i = 0; i < r; ++i) {
                switch (uchastniki[i]) {
                case 1: std::cout << " Ботинки-вездеходы"; break;
                case 2: std::cout << " Метла"; break;
                case 3: std::cout << " Верблюд"; break;
                case 4: std::cout << " Кентавр"; break;
                case 5: std::cout << " Орёл"; break;
                case 6: std::cout << " Верблюд-быстроход"; break;
                case 7: std::cout << " Ковёр-самолёт"; break;
                }
                if (r > 0 &&  i < r - 1) std::cout << ",";               
            }

            if ( r > 0) std::cout << "\n";

            std::cout << " 1. Ботинки-вездеходы \n 2. Метла \n 3. Верблюд \n 4. Кентавр \n 5. Орёл \n 6. Верблюд-быстроход \n 7. Ковёр-самолёт \n 0. Закончить регистрацию \n";

            for (int i = 0; i < 2;) {
                std::cout << "Выберите транспорт или 0 : ";
                std::cin >> b;
                if (b == 0 && r < 2) {
                    std::cout << "В гонке должно быть, хотя бы 2 участника!\n";
                }
                else if (b == 0 && r > 1) {
                    v = 10; break;
                }
                else
                {
                    if (b < 0 && b > 7) {
                        std::cout << "Недопустимое значение!\n";
                    }
                    else {
                        if (tg == 1) {
                            if (b == 1 || b == 3 || b == 4 || b == 6) {
                                if (b > 0 && b < 8 && b != uchastniki[0] && b != uchastniki[1] && b != uchastniki[2] && b != uchastniki[3] && b != uchastniki[4] && b != uchastniki[5]) {
                                    i = 2;
                                }
                                else {
                                    switch (b) {
                                    case 1: std::cout << "\nБотинки-вездеходы уже зарегестрированы!\n"; break;
                                    case 3: std::cout << "\nВерблюд уже зарегестрирован!\n"; break;
                                    case 4: std::cout << "\nКентавр уже зарегестрирован!\n"; break;
                                    case 6: std::cout << "\nВерблюд-быстроход уже зарегестрирован!\n"; break;
                                    }
                                }
                            }
                            else std::cout << "\nПопытка зарегестрировать неправильный тип транспортного средства!\n";
                        }
                        else if (tg == 2) {
                            if (b == 2 || b == 5 || b == 7) {
                                if (b > 0 && b < 8 && b != uchastniki[0] && b != uchastniki[1] && b != uchastniki[2] && b != uchastniki[3] && b != uchastniki[4] && b != uchastniki[5]) {
                                    i = 2;
                                }
                                else {
                                    switch (b) {
                                    case 2: std::cout << "\nМетла уже зарегестрирована!\n"; break;
                                    case 5: std::cout << "\nОрёл уже зарегестрирован!\n"; break;
                                    case 7: std::cout << "\nКовёр-самолёт уже зарегестрирован!\n"; break;
                                    }
                                }
                            }
                            else std::cout << "\nПопытка зарегестрировать неправильный тип транспортного средства!\n";
                        }
                        else {
                            if (b > 0 && b < 8 && b != uchastniki[0] && b != uchastniki[1] && b != uchastniki[2] && b != uchastniki[3] && b != uchastniki[4] && b != uchastniki[5]) {
                                i = 2;
                            }
                            else {
                                switch (b) {
                                case 1: std::cout << "\nБотинки-вездеходы уже зарегестрированы!\n"; break;
                                case 2: std::cout << "\nМетла уже зарегестрирована!\n"; break;
                                case 3: std::cout << "\nВерблюд уже зарегестрирован!\n"; break;
                                case 4: std::cout << "\nКентавр уже зарегестрирован!\n"; break;
                                case 5: std::cout << "\nОрёл уже зарегестрирован!\n"; break;
                                case 6: std::cout << "\nВерблюд-быстроход уже зарегестрирован!\n"; break;
                                case 7: std::cout << "\nКовёр-самолёт уже зарегестрирован!\n"; break;
                                }
                            }
                        }
                    }
                }
            }

            if (b != 0) uchastniki[r] = b;

            if (b != 0) ++itter;

            switch (b) {
            case 1: std::cout << "\nБотинки-вездеходы успешно зарегестрированы!\n\n"; break;
            case 2: std::cout << "\nМетла успешно зарегестрирована!\n\n"; break;
            case 3: std::cout << "\nВерблюд успешно зарегестрирован!\n\n"; break;
            case 4: std::cout << "\nКентавр успешно зарегестрирован!\n\n"; break;
            case 5: std::cout << "\nОрёл успешно зарегестрирован!\n\n"; break;
            case 6: std::cout << "\nВерблюд-быстроход успешно зарегестрирован!\n\n"; break;
            case 7: std::cout << "\nКовёр-самолёт успешно зарегестрирован!\n\n"; break;
            }

            if (v == 10) {
                for (int i = 0; i < 2;) {
                    int e = 0;
                    std::cout << "\n Выберите действие: \n1. Зарегестрировать транспорт \n2. Начать гонку\n";
                    std::cin >> e;
                    if (e == 1) {
                        --r;
                        i = 2;
                        v = 0;
                    }
                    else if (e == 2) break;
                }
            }
            if (v == 10) break;
        }

        Racer* racer_arr = new Racer[itter];

        for (int i = 0; i < itter; ++i) {

            racer_arr[i].reg(uchastniki[i], trassa);
        }

        marafon(racer_arr, itter);

        std::cout << "\nРезультаты гонки: \n";

        for (int i = 0; i < itter; ++i) {
            std::cout << i + 1 << ". " << racer_arr[i].Get_name() << ". Время: " << racer_arr[i].Get_time() << "\n";
        }
               

        delete[] racer_arr;
        itter = 0;
        trassa = 0;
        b = 0;
        tg = 0;
        for (int i = 0; i < 7; ++i) {
            uchastniki[i] = 0;
        }

        std::cout << "\n 1. Провести ещё одну гонку. \n 2. Выйти \n";
        for (int i = 0; i < 2;) {
            int a = 0;
            std::cout << "Выберите действие: ";
            std::cin >> a;
            if (a == 1) i = 2;
            else return 0;

        }
        std::cout << "\n 1. Гонка для наземного транспорта \n 2. Гонка для воздушного транспорта \n 3. Гонка для наземного и воздушного транспорта\n";

    }
    return 0;
}