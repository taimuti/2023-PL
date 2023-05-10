# Словарь для хранения списка ребер графа
graph = {}
# Словарь для хранения всех городов и их индексов
city2ind = {}
ind2city = {}
# Словарь для хранения всех видов транспорта и их индексов
transp2ind = {}
ind2transp = {}

# Функция для добавления ребер в граф
def add_edge(from_city, to_city, transport_type, cruise_time, cruise_fare):
    # Проверяем, есть ли уже данный город в словаре graph
    if from_city not in graph:
        graph[from_city] = {}
    if to_city not in graph:
        graph[to_city] = {}
    if to_city not in graph[from_city]:
        graph[from_city][to_city] = {}
    graph[from_city][to_city][transport_type] = {}
    # Добавляем ребро от города отправления к городу прибытия
    graph[from_city][to_city][transport_type] = (cruise_time, cruise_fare)

# Функция для индексации транспорта
def get_transport_index(transp, count):
    if (transp not in transp2ind):  
        transp2ind[transp] = count
        ind2transp[count] = transp
        count += 1
    return count

# Функция для индексации городов
def get_city_index(start, count):
    if (start not in city2ind):  
        city2ind[start] = count
        ind2city[count] = start
        count += 1
    return count

# Чтение графа из файла
def read_graph(filename):
    with open(filename, 'r') as f:
        count_id = 0
        count_tr = 0
        for line in f:
            # Игнорируем комментарии и пустые строки
            if line.startswith('#') or not line.strip():
                continue
            from_city_not, to_city_not, transport_type_not, cruise_time, cruise_fare = line.strip().split()
            transport_type = transport_type_not.strip('"')
            from_city = from_city_not.strip('"')
            to_city = to_city_not.strip('"')

            count_tr = get_transport_index(transport_type, count_tr)
            count_id = get_city_index(from_city, count_id)
            count_id = get_city_index(to_city, count_id)
            
            add_edge(city2ind[from_city], city2ind[to_city], transp2ind[transport_type], int (cruise_time), int(cruise_fare))