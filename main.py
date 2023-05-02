import heapq

# Словарь для хранения списка ребер графа
graph = {}

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
        current_time, current_city = min(unprocessed_cities)
        unprocessed_cities.remove((current_time, current_city))
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
                i = 0
                while i < len(unprocessed_cities) and unprocessed_cities[i][0] <= new_time:
                    i += 1
                unprocessed_cities.insert(i, (new_time, neighbor_city))
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
        current_cost, current_city = min(unprocessed_cities)
        unprocessed_cities.remove((current_cost, current_city))
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
                i = 0
                while i < len(unprocessed_cities) and unprocessed_cities[i][0] <= new_cost:
                    i += 1
                unprocessed_cities.insert(i, (new_cost, neighbor_city))
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
    # Создаем словарь для хранения кратчайших путей
    shortest_paths = {from_city: (None, 0)}
    # Словарь для хранения путей
    path = {}
    # Список для хранения непроверенных городов
    unvisited_cities = [from_city]
    while unvisited_cities:
        # Выбираем город с наименьшей стоимостью
        current_city = min([(shortest_paths[c][1], c) for c in unvisited_cities])[1]
        # Проходим по всем городам, соседним с текущим
        for neighbor_city in graph[current_city]:
            # Проверяем, разрешено ли использование транспорта, которым можно добраться до города-соседа
            if graph[current_city][neighbor_city][0] in allowed_transport_types:
                # Вычисляем стоимость пути от текущего города до города-соседа
                cost = shortest_paths[current_city][1] + graph[current_city][neighbor_city][2]
                # Проверяем, не превышает ли стоимость ограничение limit_cost
                if limit_cost is None or cost <= limit_cost:
                    # Если это лучший путь до города-соседа или если путь еще не найден
                    if neighbor_city not in shortest_paths or cost < shortest_paths[neighbor_city][1]:
                        # Обновляем стоимость пути до города-соседа
                        shortest_paths[neighbor_city] = (current_city, cost)
                        # Обновляем путь до города-соседа
                        path[neighbor_city] = graph[current_city][neighbor_city][0]
                    # Добавляем город-соседа в список непроверенных городов
                    if neighbor_city not in unvisited_cities:
                        unvisited_cities.append(neighbor_city)
        # Удаляем текущий город из списка непроверенных городов
        unvisited_cities.remove(current_city)
    # Формируем результат
    result = {}
    for city in shortest_paths:
        if shortest_paths[city][0] is not None:
            result[city] = {'cost': shortest_paths[city][1], 'path': []}
            # Добавляем города в путь от текущего до стартового города
            while city is not None:
                result[city]['path'].append(path.get(city))
                city = shortest_paths[city][0]
            # Разворачиваем путь в правильном порядке и удаляем первый элемент (стартовый город)
            result[city]['path'] = result[city]['path'][::-1][1:]
    return result
    
# Читаем граф из файла
read_graph("input.txt")

print("Режимы работы программы:\n 1. Среди кратчайших по времени путей между двумя городами найти путь минимальной стоимости.\n 2. Среди минимальных по стоимочти путей между двумя городами найти кратчайший по времени путь.\n 3. Найти путь между двумя городами, минимальный по числу посещенных городов.\n")
print("Выберите режим работы программы, введите цифру")
mode = input()
if mode=="1":
    print("Введите город отправления: ")
    city_from = input()
    print("Введите город прибытия: ")
    city_to = input()
    print("Введите разрешенные виды транспорта через пробел (все / автобус, поезд, самолёт): ")
    input_transport = input().split()
    if input_transport == "все":
        allowed_transport = ['автобус', 'поезд', 'самолёт']
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
    print("Введите разрешенные виды транспорта через пробел (все / автобус, поезд, самолёт): ")
    input_transport = input().split()
    if input_transport == "все":
        allowed_transport = ['автобус', 'поезд', 'самолёт']
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
    print("Введите разрешенные виды транспорта через пробел (все / автобус, поезд, самолёт): ")
    input_transport = input().split()
    if input_transport == ["все"]:
        allowed_transport = ['автобус', 'поезд', 'самолёт']
    else: allowed_transport = input_transport
    print(allowed_transport)
    result = bfs(city_from, city_to, allowed_transport)
    if result is None:
        print("Целевой город недостижим при заданных параметрах")
    else:
        print("Путь:", " -> ".join(result))
