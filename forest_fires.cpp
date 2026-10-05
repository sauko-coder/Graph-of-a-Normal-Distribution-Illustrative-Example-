#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <vector>
#include <utility>
#include <random>
#include <queue>


struct PairHash { // структура описания нового сбоника комманд для хеш0-таблицы
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const noexcept { // делаем ссылку по T1, T2 как в связных списках
        auto h1 = std::hash<T1>{}(p.first); // через auto достаем значение пары
        auto h2 = std::hash<T2>{}(p.second); // а чрез operator() мы вызываем его как функцию
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2)); // комбинирование *взял из инета)
    }
};


int random_gen(int max) { // рандомизатор чисел 
        static std::mt19937 gen{std::random_device{}()}; // источники энтропии
        std::uniform_int_distribution<int> dist(1, max); // диопозон
    return dist(gen);
}


void clear_screen() { // кросс платформенная очистка терминала
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


int main() {
    std::unordered_map<std::pair<int, int>, int, PairHash> map;

    std::vector<std::pair<int, int>> directions = {
        {0, 1},   // вверх
        {0, -1},  // вниз
        {1, 0},   // вправо
        {-1, 0}   // влево
    };

    clear_screen();
    std::cout << "Define the coordinate plane size: \n";

    int height;
    std::cin >> height;
    int width = (height * 3 + 28); // тк у "%" длина примерно в 2 раза меньше ширины

    // генератор деревьев 1
    for (int y = 1; y <= height; ++y) {
        for (int x = 1; x <= width; ++x) {
            map[{x, y}] = 1;
        }
    }


    while (true) {
        clear_screen();

        // молния
        std::pair<int, int> start = {random_gen(width), random_gen(height)};

        // если пиксель есть то выполняем возгорание
        if (map[start] == 1) {
            map[start] = 2;   // 2 равна горению те во круг загораются все деревья 
        }

        // динамическое расширение
        bool fire_spread = true;

        while (fire_spread) { // начинаем очень ресурсно затратный цикл динамического расширения возгорания
            fire_spread = false;

            // все горящие клетки на плоскости
            std::vector<std::pair<int, int>> currently_burning; // объявляем вектор горящих сейчас деревьев "2"

            for (int y = 1; y <= height; ++y) { // этот цикл встречается слишком часто чтобы его объяснять
                for (int x = 1; x <= width; ++x) {
                    if (map[{x, y}] == 2) { // если пиксель горит то засовываем его в вектор
                        currently_burning.push_back({x, y});
                    }
                }
            }

            // проверяем возможен ли побжог по 4 сторонам вокруг пикселя => поджигаем
            for (const auto& cell : currently_burning) { // берем точку xy по константной ссылке 
                for (const auto& dir : directions) { // range-based for пока не закончатся элементы
                    int nx = cell.first  + dir.first;
                    int ny = cell.second + dir.second;

                    if (nx >= 1 && nx <= width && ny >= 1 && ny <= height) {
                        if (map[{nx, ny}] == 1) {          // живое дерево
                            map[{nx, ny}] = 2;             // загорается
                            fire_spread = true;
                        }
                        }
            }
            }
        } // завершааем цикл

        // присваиваем 0 вместо 2, объекты догорели
        for (int y = 1; y <= height; ++y) {
            for (int x = 1; x <= width; ++x) {
                if (map[{x, y}] == 2) {
                    map[{x, y}] = 0;   
                }
            }
        }


    for (int i = 1; i <= 5; i++) { // чрез этот цикл можно понижать или увеличивать рост деревьев обчно 1
        // еще деревья
        int grow_count = random_gen(width / 3 + 1);

        for (int i = 0; i < grow_count; ++i) {
            int gx = random_gen(width);
            int gy = random_gen(height);
            map[{gx, gy}] = 1;
        }
    }

        // отрисовка через цикл for *лучше я пока не чего не придумывал
        for (int y = 1; y <= height; ++y) { // меняем строку
            for (int x = 1; x <= width; ++x) { //рисуем строку проверяя каждый пиксель
                int state = map[{x, y}];
                if (state == 1) {
                    std::cout << '%';          // дерево
                } else if (state == 2) {
                    std::cout << '*';          // горящее *мы его по факту не отрисовываем, но если произойдет ошибка
                } else {                       // то все будет ОК
                    std::cout << ' ';          // пропускаем если нет дерева, ну или уже сгорело
                }
            }
            std::cout << std::endl; // меняем стрку
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(600)); // задержка *для отрисовки в 1.6 кадров сек
    }

    return 0;
}
