import sys
import os
import heapq

# Словарь для хранения списка ребер графа
graph = {}
# Словарь для хранения видов транспорта
transport = []
# Словарь для хранения всех городов
cities = []

# Функция для добавления ребер в граф
def add_edge(from_city, to_city, transport_type, cruise_time, cruise_fare):
    # Проверяем, есть ли уже данный город в словаре graph
    if from_city not in graph:
        graph[from_city] = {}
    if to_city not in graph:
        graph[to_city] = {}
    # Добавляем ребро от города отправления к городу прибытия
    graph[from_city][to_city] = (transport_type, cruise_time, cruise_fare)
    
# Чтение графа из файла
def read_graph(filename):
    with open(filename, 'r') as f:
        for line in f:
            # Игнорируем комментарии и пустые строки
            if line.startswith('#') or not line.strip():
                continue
            from_city, to_city, transport_type, cruise_time, cruise_fare = line.strip().split()
            add_edge(from_city.strip('"'), to_city.strip('"'), transport_type.strip('"'), int (cruise_time), int(cruise_fare))
            if transport_type.strip('"') not in transport: transport.append(transport_type.strip('"'))
            if from_city.strip('"') not in cities: cities.append(from_city.strip('"'))
            if to_city.strip('"') not in cities: cities.append(to_city.strip('"'))

def dijkstra_shortest_time(from_city, to_city, allowed_transport_types):
    # Создаем словарь для хранения кратчайшего времени проезда от начального города до остальных городов
    shortest_time = {from_city: 0}
    # Создаем словарь для хранения пути от начального города до остальных городов
    path = {from_city: [from_city]}
    # Создаем список, в который будем добавлять необработанные города
    unprocessed_cities = [(0, from_city)]
    # Пока есть необработанные города
    while unprocessed_cities:
        # Получаем город с минимальным временем проезда
        (current_time, current_city) = heapq.heappop(unprocessed_cities)
        # Если достигли целевого города, возвращаем путь и время проезда
        if current_city == to_city:
            return (path[current_city], shortest_time[current_city])
        # Для каждого соседнего города
        for neighbor_city, (transport_type, cruise_time, cruise_fare) in graph[current_city].items():
            # Если данный тип транспорта не допустим, пропускаем этот город
            if transport_type not in allowed_transport_types:
                continue
            # Вычисляем время проезда до соседнего города через текущий город
            new_time = current_time + cruise_time
            # Если это время проезда до соседнего города меньше, чем ранее известное
            if neighbor_city not in shortest_time or new_time < shortest_time[neighbor_city]:
                # Обновляем кратчайшее время проезда и путь до соседнего города
                shortest_time[neighbor_city] = new_time
                path[neighbor_city] = path[current_city] + [neighbor_city]
                # Добавляем соседний город в список необработанных городов
                heapq.heappush(unprocessed_cities, (new_time, neighbor_city))
    # Если целевой город недостижим из начального, возвращаем None
    return None

def dijkstra_lowest_cost(from_city, to_city, allowed_transport_types):
    # Создаем словарь для хранения минимальной стоимости проезда от начального города до остальных городов
    lowest_cost = {from_city: 0}
    # Создаем словарь для хранения пути от начального города до остальных городов
    path = {from_city: [from_city]}
    # Создаем список, в который будем добавлять необработанные города
    unprocessed_cities = [(0, from_city)]
    # Пока есть необработанные города
    while unprocessed_cities:
        # Получаем город с минимальной стоимостью проезда
        (current_cost, current_city) = heapq.heappop(unprocessed_cities)
        # Если достигли целевого города, возвращаем путь и стоимость проезда
        if current_city == to_city:
            return (path[current_city], lowest_cost[current_city])
        # Для каждого соседнего города
        for neighbor_city, (transport_type, cruise_time, cruise_fare) in graph[current_city].items():
            # Если данный тип транспорта не допустим, пропускаем этот город
            if transport_type not in allowed_transport_types:
                continue
            # Вычисляем стоимость проезда до соседнего города через текущий город
            new_cost = current_cost + cruise_fare
            # Если это время проезда до соседнего города меньше, чем ранее известное
            if neighbor_city not in lowest_cost or new_cost < lowest_cost[neighbor_city]:
                # Обновляем минимальную стоимость проезда и путь до соседнего города
                lowest_cost[neighbor_city] = new_cost
                path[neighbor_city] = path[current_city] + [neighbor_city]
                # Добавляем соседний город в список необработанных городов
                heapq.heappush(unprocessed_cities, (new_cost, neighbor_city))
    # Если целевой город недостижим из начального, возвращаем None
    return None

def bfs(from_city, to_city, allowed_transport_types):
    # Словарь для хранения посещенных городов
    visited = {from_city: None}
    # Список очередности обработки городов
    queue = [from_city]
    while queue:
        # Извлекаем следующий город из очереди
        current_city = queue.pop(0)
        # Если достигли целевого города, возвращаем путь к нему
        if current_city == to_city:
            path = []
            while current_city is not None:
                path.append(current_city)
                current_city = visited[current_city]
            return path[::-1]
        # Просматриваем соседей текущего города
        for neighbor_city, transport in graph[current_city].items(): 
            transport_type, cruise_time, cruise_fare = transport
            # Проверяем, что разрешенный вид транспорта встречается в текущем ребре
            if transport_type in allowed_transport_types and neighbor_city not in visited:
                visited[neighbor_city] = current_city
                queue.append(neighbor_city)
    # Если целевой город не найден, возвращаем None
    return None

def dijkstra_limited_cost(from_city, allowed_transport_types, limit_cost):
    # Словарь для хранения кратчайших расстояний до каждого города
    distances = {city: float('inf') for city in cities}
    distances[from_city] = 0
    # Запоминание путей
    path ={}
    # Очередь с приоритетами для хранения пар (расстояние до города, город)
    queue = [(0, from_city)]
    # Цикл, пока очередь с приоритетами не пуста
    while queue:
        # Извлекаем из очереди город с минимальной стоимостью
        curr_cost, curr_city = heapq.heappop(queue)
        if (curr_cost > distances[curr_city]): 
            continue
        # Если стоимость до текущего города больше, чем ограничение, то выходим из цикла
        if curr_cost > limit_cost: 
            break
        # Обходим всех соседей текущего города
        for neighbor, (transport_type, cruise_time, cruise_fare) in graph[curr_city].items():
            # Если тип транспорта не входит в список разрешенных, то пропускаем город
            if transport_type not in allowed_transport_types: 
                continue
            # Вычисляем новую стоимость до соседа через текущий город
            new_cost = distances[curr_city] + cruise_fare
            if ((distances[neighbor] > new_cost) and (new_cost <= limit_cost)):
                distances[neighbor] = new_cost
                path[neighbor] = [curr_city, neighbor, transport_type, cruise_fare]
                heapq.heappush(queue,[distances[neighbor], neighbor])
    # Выводим кратчайшие пути до всех городов, до которых можно добраться за ограниченную стоимость
    for (to_city, [from_city, to_city, transport_type, cruise_fare]) in path.items():
        print(f'{to_city} : {from_city} -> {to_city}, {transport_type}, {cruise_fare}')

def dijkstra_limited_time(from_city, allowed_transport_types, limit_time):
    # Словарь для хранения минимального времени до каждого города
    distances = {city: float('inf') for city in cities}
    distances[from_city] = 0
    # Запоминание путей
    path ={}
    # Очередь с приоритетами для хранения пар (время до города, город)
    queue = [(0, from_city)]
    # Цикл, пока очередь с приоритетами не пуста
    while queue:
        # Извлекаем из очереди город с минимальной стоимостью
        curr_time, curr_city = heapq.heappop(queue)
        if (curr_time > distances[curr_city]): 
            continue
        # Если стоимость до текущего города больше, чем ограничение, то выходим из цикла
        if curr_time > limit_time: 
            break
        # Обходим всех соседей текущего города
        for neighbor, (transport_type, cruise_time, cruise_fare) in graph[curr_city].items():
            # Если тип транспорта не входит в список разрешенных, то пропускаем город
            if transport_type not in allowed_transport_types: 
                continue
            # Вычисляем новую стоимость до соседа через текущий город
            new_time = distances[curr_city] + cruise_time
            if ((distances[neighbor] > new_time) and (new_time <= limit_time)):
                distances[neighbor] = new_time
                path[neighbor] = [curr_city, neighbor, transport_type, cruise_time]
                heapq.heappush(queue,[distances[neighbor], neighbor])
    # Выводим кратчайшие пути до всех городов, до которых можно добраться за ограниченную стоимость
    for (to_city, [from_city, to_city, transport_type, cruise_time]) in path.items():
        print(f'{to_city} : {from_city} -> {to_city}, {transport_type}, {cruise_time}')

# Читаем граф из файла
filename = os.getcwd() + '/' + sys.argv[1]
read_graph(filename)

print("Режимы работы программы:\n 1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости.\n 2. Среди минимальных по стоимоcти путей между двумя городами найти кратчайший по времени путь.\n 3. Найти путь между двумя городами, минимальный по числу посещенных городов.\n 4. Найти множество городов, достижимых из города отправления не более чем за ограниченную сумму денег.\n 5. Найти множество городов, достижимых из города отправления не более чем за lограниченное количество времени.\n")
print("Выберите режим работы программы, введите цифру")
mode = input()
if mode=="1":
    print("Введите город отправления: ")
    city_from = input()
    print("Введите город прибытия: ")
    city_to = input()
    print("Введите разрешенные виды транспорта через пробел, если хотите разрешить все виды, введите ""все"":")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = transport
    else: allowed_transport = input_transport
 
    result = dijkstra_shortest_time(city_from, city_to, allowed_transport)
    if result is None:
        print("Целевой город недостижим при заданных параметрах")
    else:
        path, time = result
        print("Путь:", " -> ".join(path))
        print("Время: ", time)
        
elif mode=="2": 
    print("Введите город отправления: ")
    city_from = input()
    print("Введите город прибытия: ")
    city_to = input()
    print("Введите разрешенные виды транспорта через пробел, если хотите разрешить все виды, введите ""все"":")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = transport
    else: allowed_transport = input_transport

    result = dijkstra_lowest_cost(city_from, city_to, allowed_transport)
    if result is None:
        print("Целевой город недостижим при заданных параметрах")
    else:
        path, cost = result
        print("Путь:", " -> ".join(path))
        print("Стоимость: ", cost)

elif mode=="3":
    print("Введите город отправления: ")
    city_from = input()
    print("Введите город прибытия: ")
    city_to = input()
    print("Введите разрешенные виды транспорта через пробел, если хотите разрешить все виды, введите ""все"":")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = transport
    else: allowed_transport = input_transport

    result = bfs(city_from, city_to, allowed_transport)
    if result is None:
        print("Целевой город недостижим при заданных параметрах")
    else:
        print("Путь:", " -> ".join(result))

elif mode=="4":
    print("Введите город отправления: ")
    city_from = input()
    print("Введите ограничение по стоимости: ")
    cost_limit = input()
    print("Введите разрешенные виды транспорта через пробел, если хотите разрешить все виды, введите ""все"":")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = transport
    else: allowed_transport = input_transport

    dijkstra_limited_cost(city_from, allowed_transport, cost_limit)

elif mode=="5":
    print("Введите город отправления: ")
    city_from = input()
    print("Введите ограничение по времени: ")
    time_limit = input()
    print("Введите разрешенные виды транспорта через пробел, если хотите разрешить все виды, введите ""все"":")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = transport
    else: allowed_transport = input_transport

    dijkstra_limited_time(city_from, allowed_transport, time_limit)
