import heapq
from graph import *

def dijkstra_shortest_time(from_city, to_city, allowed_transport_types):
    # Создаем словарь для хранения кратчайшего времени проезда от начального города до остальных городов
    shortest_time = {from_city: 0}
    # Создаем словарь для хранения пути от начального города до остальных городов
    path = {}
    # Создаем список, в который будем добавлять необработанные города
    unprocessed_cities = []
    heapq.heappush(unprocessed_cities, [0, from_city])
    # Пока есть необработанные города
    while unprocessed_cities:
        # Получаем город с минимальным временем проезда
        (current_time, current_city) = heapq.heappop(unprocessed_cities)
        if (current_time > shortest_time[current_city]): continue
        # Если достигли целевого города, возвращаем путь и стоимость проезда
        if current_city == to_city:
            return path, shortest_time[current_city]
        # Для каждого соседнего города
        for neighbor_city, transport in graph[current_city].items():
            for transport_type, (cruise_time, cruise_fare) in transport.items():
                # Если данный тип транспорта не допустим, пропускаем этот город
                if ind2transp[transport_type] not in allowed_transport_types: continue
                # Если новое время проезда до соседнего города меньше, чем ранее известное
                new_time = shortest_time[current_city] + cruise_time
                if (neighbor_city not in shortest_time) or (new_time < shortest_time[neighbor_city]):
                    # Обновляем кратчайшее время проезда и путь до соседнего города
                    shortest_time[neighbor_city] = new_time
                    path[neighbor_city] = (current_city, neighbor_city, transport_type, cruise_time, cruise_fare)
                    # Добавляем соседний город в список необработанных городов
                    heapq.heappush(unprocessed_cities, [shortest_time[neighbor_city], neighbor_city])
    return None

def dijkstra_lowest_cost(from_city, to_city, allowed_transport_types):
    # Создаем словарь для хранения минимальной стоимости проезда от начального города до остальных городов
    lowest_cost = {from_city: 0}
    # Создаем словарь для хранения пути от начального города до остальных городов
    path = {}
    # Создаем список, в который будем добавлять необработанные города
    unprocessed_cities = []
    heapq.heappush(unprocessed_cities, [0, from_city])
    # Пока есть необработанные города
    while unprocessed_cities:
        # Получаем город с минимальной стоимостью проезда
        (current_cost, current_city) = heapq.heappop(unprocessed_cities)
        if (current_cost > lowest_cost[current_city]): continue
        # Если достигли целевого города, возвращаем путь и стоимость проезда
        if current_city == to_city:
            return path, lowest_cost[current_city]
        # Для каждого соседнего города
        for neighbor_city, transport in graph[current_city].items():
            for transport_type, (cruise_time, cruise_fare) in transport.items():
                # Если данный тип транспорта не допустим, пропускаем этот город
                if ind2transp[transport_type] not in allowed_transport_types: continue
                # Вычисляем стоимость проезда до соседнего города через текущий город
                new_cost = current_cost + cruise_fare
                # Если это время проезда до соседнего города меньше, чем ранее известное
                if (neighbor_city not in lowest_cost) or (new_cost < lowest_cost[neighbor_city]):
                    # Обновляем минимальную стоимость проезда и путь до соседнего города
                    lowest_cost[neighbor_city] = new_cost
                    path[neighbor_city] = (current_city, neighbor_city, transport_type, cruise_time, cruise_fare)
                    # Добавляем соседний город в список необработанных городов
                    heapq.heappush(unprocessed_cities, (lowest_cost[neighbor_city], neighbor_city))
    # Если целевой город недостижим из начального, возвращаем None
    return None

def bfs(from_city, to_city, allowed_transport_types):
    # Словарь для хранения посещенных городов
    visited = {from_city: from_city}
    # Список очередности обработки городов
    unprocessed_cities = [from_city]
    while unprocessed_cities:
        # Извлекаем следующий город из очереди
        current_city = unprocessed_cities.pop(0)
        # Если достигли целевого города, возвращаем путь к нему
        if current_city == to_city:
            path = {}
            while current_city is not from_city:
                path[current_city] = visited[current_city]
                current_city = visited[current_city][0]
            return path
        # Просматриваем соседей текущего города
        for neighbor_city, transport in graph[current_city].items(): 
            for transport_type, (cruise_time, cruise_fare) in transport.items():
                # Если данный тип транспорта не допустим, пропускаем этот город
                if ind2transp[transport_type] not in allowed_transport_types: continue
                # Проверяем, что разрешенный вид транспорта встречается в текущем ребре
                if neighbor_city not in visited:
                    visited[neighbor_city] = (current_city, neighbor_city, transport_type, cruise_time, cruise_fare)
                    unprocessed_cities.append(neighbor_city)
    return None

def dijkstra_limited_cost(from_city, allowed_transport_types, limit_cost):
    # Словарь для хранения минимального времени до каждого города
    distances = {city: float('inf') for city in ind2city}
    distances[from_city] = 0
    # Запоминание путей
    path ={}
    # Очередь с приоритетами для хранения пар (время до города, город)
    queue = [(0, from_city)]
    # Цикл, пока очередь с приоритетами не пуста
    while queue:
        # Извлекаем из очереди город с минимальной стоимостью
        current_cost, current_city = heapq.heappop(queue)
        if (current_cost > distances[current_city]): continue
        # Если стоимость до текущего города больше, чем ограничение, то выходим из цикла
        if current_cost > limit_cost: break
        # Обходим всех соседей текущего города
        for neighbor_city, transport in graph[current_city].items():
            for transport_type, (cruise_time, cruise_fare) in transport.items():
                # Если тип транспорта не входит в список разрешенных, то пропускаем город
                if ind2transp[transport_type] not in allowed_transport_types: continue
                # Вычисляем новую стоимость до соседа через текущий город
                new_cost = distances[current_city] + cruise_fare
                if ((distances[neighbor_city] > new_cost) and (new_cost <= limit_cost)):
                    distances[neighbor_city] = new_cost
                    path[neighbor_city] = (current_city, neighbor_city, transport_type, cruise_time, cruise_fare)
                    heapq.heappush(queue,(distances[neighbor_city], neighbor_city))
    return path

def dijkstra_limited_time(from_city, allowed_transport_types, limit_time):
    # Словарь для хранения минимального времени до каждого города
    distances = {city: float('inf') for city in ind2city}
    distances[from_city] = 0
    # Запоминание путей
    path ={}
    # Очередь с приоритетами для хранения пар (время до города, город)
    queue = [(0, from_city)]
    # Цикл, пока очередь с приоритетами не пуста
    while queue:
        # Извлекаем из очереди город с минимальной стоимостью
        current_time, current_city = heapq.heappop(queue)
        if (current_time > distances[current_city]): continue
        # Если стоимость до текущего города больше, чем ограничение, то выходим из цикла
        if current_time > limit_time: break
        # Обходим всех соседей текущего города
        for neighbor_city, transport in graph[current_city].items():
            for transport_type, (cruise_time, cruise_fare) in transport.items():
                # Если тип транспорта не входит в список разрешенных, то пропускаем город
                if ind2transp[transport_type] not in allowed_transport_types: continue
                # Вычисляем новую стоимость до соседа через текущий город
                new_time = distances[current_city] + cruise_time
                if ((distances[neighbor_city] > new_time) and (new_time <= limit_time)):
                    distances[neighbor_city] = new_time
                    path[neighbor_city] = (current_city, neighbor_city, transport_type, cruise_time, cruise_fare)
                    heapq.heappush(queue,(distances[neighbor_city], neighbor_city))
    return path