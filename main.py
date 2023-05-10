import sys
import os
import curses

from algorithms import *
from graph import *

OPTIONS_NUM = 6

MINCOST_MINTIME_MODE = 0
MINCOST_MODE = 1
MINSTATIONSNUM_MODE = 2
LIMITCOST_MODE = 3
LIMITTIME_MODE = 4
WANT_TO_EXIT = 5

def main(stdscr):
    filename = os.getcwd() + '/' + sys.argv[1]
    read_graph(filename)
    all_transport = [n for n in transp2ind]

    want_to_exit = False

    stdscr.scrollok(True)
    stdscr.keypad(True)

    while not want_to_exit:

        current_item_index = 0
        choice_made = False

        curses.noecho()

        choices = ["1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости.",
           "2. Среди минимальных по стоимоcти путей между двумя городами найти кратчайший по времени путь.",
           "3. Найти путь между двумя городами, минимальный по числу посещенных городов.",
           "4. Найти множество городов, достижимых из города отправления не более чем за ограниченную сумму денег.",
           "5. Найти множество городов, достижимых из города отправления не более чем за ограниченное количество времени.",
           "Выйти из программы"]

        while not choice_made:
            stdscr.clear()
            curses.curs_set(0)
            stdscr.addstr("Выберите режим работы программы:\n\n")
            stdscr.refresh()

            for i in range(OPTIONS_NUM):
                if i == current_item_index:
                    stdscr.attron(curses.A_STANDOUT)
                    stdscr.addstr(f"{choices[i]}\n")
                    stdscr.attroff(curses.A_STANDOUT)
                else:
                    stdscr.addstr(f"{choices[i]}\n")
                stdscr.refresh()
            # Ожидаем ввод пользователя
            key = stdscr.getch() 
            # Обрабатываем ввод пользователя
            if key == curses.KEY_UP:
                if current_item_index > 0:
                    current_item_index -= 1
                else:
                    current_item_index = OPTIONS_NUM - 1
            elif key == curses.KEY_DOWN:
                if current_item_index < OPTIONS_NUM - 1:
                    current_item_index += 1
                else:
                    current_item_index = 0
            elif key == curses.KEY_ENTER or key == 10 or key == 13: 
                choice_made = True

        if current_item_index >= 0 and current_item_index <= OPTIONS_NUM - 2: # Запрашиваем разрешенные виды транспорта
            flag_0 = False
            was_transport_error = False
            while flag_0 == False:
                stdscr.clear()
                if was_transport_error:
                    stdscr.addstr(f"Такого транспорта нет, попробуйте еще раз.\n")
                was_transport_error = False
                stdscr.addstr("Введите разрешенные виды транспорта через пробел. Если хотите разрешить все виды транспорта, нажмите ENTER:\n\n")
                stdscr.refresh()
                curses.curs_set(1)
                curses.echo()

                # Считываем строку и разбиваем ее на разрешенные виды транспорта
                allowed_transport_str = str(stdscr.getstr(), "utf-8", errors="ignore")               
                allowed_transport = allowed_transport_str.split(" ")
                if allowed_transport == [""]:
                    allowed_transport = all_transport
                    flag_0 = True
                else:
                    # Проверяем, что каждый вид транспорта существует
                    for transport_type in allowed_transport:
                        if transport_type not in all_transport:
                            was_transport_error = True
                            break
                    if not was_transport_error: # Если введено неверно, то повторно запрашиваем ввод транспорта
                        flag_0 = True

        curses.echo()

        # Выбран 1 режим
        if current_item_index == MINCOST_MINTIME_MODE:
            flag_1 = False
            was_city_from_error = False
            while flag_1 == False:
                stdscr.clear()
                if was_city_from_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город отправления:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_from = str(stdscr.getstr(), "utf-8", errors="ignore")  
                # Проверяем существет ли город             
                if city_from not in city2ind: 
                    was_city_from_error = True
                else:
                    flag_1 = True
                    was_city_from_error = False

            flag_2 = False
            was_city_to_error = False
            while flag_2 == False:
                stdscr.clear()
                if was_city_to_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город прибытия:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_to = str(stdscr.getstr(), "utf-8", errors="ignore")                
                # Проверяем существет ли город 
                if city_to not in city2ind: 
                    was_city_to_error = True
                else:
                    flag_2 = True
                    was_city_to_error = False

            # АЛГОРИТМ 1
            result = dijkstra_shortest_time(city2ind[city_from], city2ind[city_to], allowed_transport)
            # Очищаем экран и выводим результат первого алгоритма
            stdscr.clear() 
            if result is None:
                stdscr.addstr("Целевой город недостижим при заданных параметрах.\n")
            else:
                stdscr.addstr("Путь минимальной стоимости среди кратчайших по времени путей:\n")
                stdscr.addstr(f"Время: {result[1]}\n")
                path = result[0]
                cruise = []
                curver = city2ind[city_to]
                while (curver != city2ind[city_from]):
                    cruise.insert(0, (path[curver]))
                    curver = path[curver][0]
                for (from_city, to_city, transport_type, cruise_time, cruise_fare) in cruise:
                    stdscr.addstr(f'{ind2city[from_city]} -- {ind2transp[transport_type]} {cruise_time} {cruise_fare} --> ')
                if to_city is not None:
                    stdscr.addstr(f'{ind2city[to_city]} \n')

            stdscr.addstr("Нажмите любую клавишу для возвращения в главное меню\n")
            stdscr.refresh()
            curses.curs_set(0)
            stdscr.getch()
        
        # Выбран 2 режим
        elif current_item_index == MINCOST_MODE:
            flag_1 = False
            was_city_from_error = False
            while flag_1 == False:
                stdscr.clear()
                if was_city_from_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город отправления:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_from = str(stdscr.getstr(), "utf-8", errors="ignore")               
                if city_from not in city2ind:
                    was_city_from_error = True
                else:
                    flag_1 = True
                    was_city_from_error = False

            flag_2 = False
            was_city_to_error = False
            while flag_2 == False:
                stdscr.clear()
                if was_city_to_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город прибытия:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_to = str(stdscr.getstr(), "utf-8", errors="ignore")                  
                if city_to not in city2ind:
                    was_city_to_error = True
                else:
                    flag_2 = True
                    was_city_to_error = False

            # АЛГОРИТМ 2
            result = dijkstra_lowest_cost(city2ind[city_from], city2ind[city_to], allowed_transport)
            
            stdscr.clear()
            if result is None:
                stdscr.addstr("Целевой город недостижим при заданных параметрах.\n")
            else:
                stdscr.addstr("Кратчайший по времени путь среди путей минимальной стоимости:\n")
                stdscr.addstr(f"Стоимость: {result[1]}\n")
                path = result[0]
                cruise = []
                curver = city2ind[city_to]
                while (curver != city2ind[city_from]):
                    cruise.insert(0, (path[curver]))
                    curver = path[curver][0]
                for (from_city, to_city, transport_type, cruise_time, cruise_fare) in cruise:
                    stdscr.addstr(f'{ind2city[from_city]} -- {ind2transp[transport_type]} {cruise_time} {cruise_fare} --> ')
                if to_city is not None:
                    stdscr.addstr(f'{ind2city[to_city]} \n')
            
            stdscr.addstr("Нажмите любую клавишу для перехода в меню\n")
            stdscr.refresh()
            curses.curs_set(0)
            stdscr.getch()
        
        # Выбран 3 режим
        elif current_item_index == MINSTATIONSNUM_MODE:
            flag_1 = False
            was_city_from_error = False
            while flag_1 == False:
                stdscr.clear()
                if was_city_from_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город отправления:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_from = str(stdscr.getstr(), "utf-8", errors="ignore")              
                if city_from not in city2ind:
                    was_city_from_error = True
                else:
                    flag_1 = True
                    was_city_from_error = False

            flag_2 = False
            was_city_to_error = False
            while flag_2 == False:
                stdscr.clear()
                if was_city_to_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город прибытия:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_to = str(stdscr.getstr(), "utf-8", errors="ignore")               
                if city_to not in city2ind:
                    was_city_to_error = True
                else:
                    flag_2 = True
                    was_city_to_error = False

            # АЛГОРИТМ 3
            result = bfs(city2ind[city_from], city2ind[city_to], allowed_transport)
            stdscr.clear()
            stdscr.addstr(f"Путь между двумя городами, минимальный по числу посещенных городов:\n")
            
            cruise = []
            curver = city2ind[city_to]
            while (curver != city2ind[city_from]):
                if ((result is None) or (curver not in result)): 
                    stdscr.addstr('Целевой город недостижим при заданных параметрах.\n')
                    break
                cruise.insert(0, (result[curver]))
                curver = result[curver][0]
            for (from_city, to_city, transport_type, cruise_time, cruise_fare) in cruise:
                stdscr.addstr(f'{ind2city[from_city]} -- {ind2transp[transport_type]} {cruise_time} {cruise_fare} --> ')
            if to_city is not None:
                stdscr.addstr(f'{ind2city[to_city]} \n')

            stdscr.addstr("Нажмите любую клавишу для перехода в меню\n")
            stdscr.refresh()
            curses.curs_set(0)
            stdscr.getch()

        # Выбран 4 режим
        elif current_item_index == LIMITCOST_MODE:
            flag_1 = False
            was_city_from_error = False
            while flag_1 == False:
                stdscr.clear()
                if was_city_from_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город отправления:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_from = str(stdscr.getstr(), "utf-8", errors="ignore")                
                if city_from not in city2ind:
                    was_city_from_error = True
                else:
                    flag_1 = True
                    was_city_from_error = False
                             
            stdscr.clear()
            stdscr.addstr("Введите ограничение по стоимости:\n") 
            stdscr.refresh()
            curses.curs_set(1)
            # Ввод ограничения по стоимости и обработка возможных ошибок
            while True: 
                try:
                    limit_cost = int(str(stdscr.getstr(), "utf-8", errors="ignore"))
                    if limit_cost < 0: 
                        raise ValueError()
                    break
                except ValueError:
                    stdscr.clear()
                    stdscr.addstr("Некорректный ввод. Ожидется целое неотрицательным числом. Попробуйте еще раз:\n")
                    stdscr.refresh()
                    continue
            
            # АЛГОРИТМ 4            
            result = dijkstra_limited_cost(city2ind[city_from], allowed_transport, limit_cost)                     

            stdscr.clear()
            if not result:
                stdscr.addstr(f"Нет городов, достижимых из {city_from} за {limit_cost} рублей, c использованием указанных доступных видов транспорта\n")
            else:
                stdscr.addstr(f"Города, достижимые из {city_from} за {limit_cost} рублей:\n")
                cruise = []
                for city in result: 
                    stdscr.addstr(f"Город : {ind2city[city]}\n")
                    cruise = []
                    while (city != city2ind[city_from]):
                        cruise.insert(0, (result[city]))
                        city = result[city][0]
                    for (from_city, to_city, transport_type, cruise_time, cruise_fare) in cruise:
                        stdscr.addstr(f'{ind2city[from_city]} -- {ind2transp[transport_type]} {cruise_time} {cruise_fare} --> ')
                    if to_city is not None:
                        stdscr.addstr(f'{ind2city[to_city]} \n')
            stdscr.addstr("Нажмите любую клавишу для перехода в меню\n")
            stdscr.refresh()
            curses.curs_set(0)
            stdscr.getch()

        # Выбран 5 режим                
        elif current_item_index == LIMITTIME_MODE:
            flag_1 = False
            was_city_from_error = False
            while flag_1 == False:
                stdscr.clear()
                if was_city_from_error:
                    stdscr.addstr("Такого города нет, попробуйте еще раз.\n")
                stdscr.addstr("Введите город отправления:\n\n")
                stdscr.refresh()
                curses.curs_set(1)

                city_from = str(stdscr.getstr(), "utf-8", errors="ignore")                 
                if city_from not in city2ind:
                    was_city_from_error = True
                else:
                    flag_1 = True
                    was_city_from_error = False
	
            stdscr.clear()
            stdscr.addstr("Введите ограничение по времени:\n")
            stdscr.refresh()
            curses.curs_set(1)
            # Ввод ограничения по времени и обработка возможных ошибок
            while True: 
                try:
                    limit_time = int(str(stdscr.getstr(), "utf-8", errors="ignore"))
                    if limit_time < 0: 
                        raise ValueError()
                    break
                except ValueError:
                    stdscr.clear()
                    stdscr.addstr("Некорректный ввод. Ожидется целое неотрицательным числом. Попробуйте еще раз:\n")
                    stdscr.refresh()
                    continue
            
            # АЛГОРИТМ 5
            result = dijkstra_limited_time(city2ind[city_from], allowed_transport, limit_time)

            stdscr.clear()
            if not result:
                stdscr.addstr(f"Нет городов, достижимых из {city_from} за время {limit_cost}, c использованием указанных доступных видов транспорта\n")
            else:
                stdscr.addstr(f"Города, достижимые из {city_from} за время {limit_time} :\n")
                for city in result:
                    stdscr.addstr(f"{city}: {result[city][0]} (время: {result[city][1]})\n")
                cruise = []
                for city in result: 
                    stdscr.addstr(f"Город : {ind2city[city]}  \n")
                    cruise = []
                    while (city != city2ind[city_from]):
                        cruise.insert(0, (result[city]))
                        city = result[city][0]
                    for (from_city, to_city, transport_type, cruise_time, cruise_fare) in cruise:
                        stdscr.addstr(f'{ind2city[from_city]} -- {ind2transp[transport_type]} {cruise_time} {cruise_fare} --> ')
                    if to_city is not None:
                        stdscr.addstr(f'{ind2city[to_city]} \n')
            stdscr.addstr("Нажмите любую клавишу для перехода в меню\n")
            stdscr.refresh()
            curses.curs_set(0)
            stdscr.getch()
        
        # Выход из программы
        elif current_item_index == WANT_TO_EXIT:
            want_to_exit = True                

        curses.endwin()


if __name__ == "__main__":
    curses.wrapper(main)