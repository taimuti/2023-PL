#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <fstream>
#include <limits>
#include <chrono>
#include <sys/resource.h>
#include <ncurses.h>
#include <locale.h>


using namespace std;


class edge
{
public:
    unsigned long from;
    unsigned long to;
    unsigned long transport_type;
    unsigned long cruise_time;
    unsigned long cruise_fare;
    edge(unsigned long a=0,unsigned long b=0,unsigned long c=0, unsigned long d=0, unsigned long e=0)
    {
        from=a;
        to=b;
        transport_type=c;
        cruise_time=d;
        cruise_fare=e;
    }
};

class track
{
public:
    vector<edge> way;
    unsigned long time=0;
    unsigned long cost=0;
    track(vector <edge> a, unsigned long b=0, unsigned long c=0)//Конуструктор пути из вектора ребер
    {
        way=a;
        if (b==0 && c==0)
        {
            for (vector<edge>::iterator it=a.begin(); it!=a.end(); it++)
            {
                b+=it->cruise_time;
                c+=it->cruise_fare;
            }
        }
        time=b;
        cost=c;
    }
    track operator+(track other)//сложение путей
    {
        vector<edge> newway={};
        newway.insert(newway.end(), way.begin(), way.end());
        newway.insert(newway.end(), other.way.begin(), other.way.end());
        return track(newway, time+other.time, cost+other.cost);
    }
    track operator+(edge other) //reverse +
    {
        vector<edge> newway;
        newway.push_back(other);
        newway.insert(newway.end(), way.begin(), way.end());
        return track(newway, time+other.cruise_time, cost+other.cruise_fare);
    }
    edge operator[](unsigned long i)//Доступ к ребру в пути
    {
        return way.at(i);
    }
};

typedef map<unsigned long, map<unsigned long, vector<edge>>> gr_type;
typedef vector<unsigned long> numvec;
typedef set<unsigned long> numset;
typedef pair<unsigned long, unsigned long> numpair;
typedef tuple<unsigned long, unsigned long, unsigned long> troika;
typedef map<unsigned long, edge> edgemap;

edgemap algo1(gr_type &graph, unsigned long n_cities, numset &transport, unsigned long start)
{
	unsigned long MAX_UL = numeric_limits<unsigned long>::max();
	vector<numpair> d(n_cities, {MAX_UL, MAX_UL});//d=[{время, стоимость}] Лучшие* в этот город
	edgemap p;//p[куда]=ребро - Запоминание путей
	d[start].first=0;//Время
	d[start].second=0;//Стоимость
	priority_queue<troika, vector<troika>, greater<troika>> q;//Куча {время, стоимость, номер вершины}
	q.push({0,0,start});
	while(!q.empty())
	{
		troika elem = q.top();
		unsigned long cur_time = get<0>(elem);
		unsigned long cur_cost = get<1>(elem);
		unsigned long cur_num = get<2>(elem);
		q.pop();
		if (cur_time > d[cur_num].first) continue;
		if ((cur_time == d[cur_num].first) && (cur_cost > d[cur_num].second)) continue;
		map<unsigned long, vector<edge>> ways = graph[cur_num];
		for (map<unsigned long, vector<edge>>::iterator it=ways.begin(); it!=ways.end(); it++)
		{
			for (vector<edge>::iterator it2=(it->second).begin();it2!=(it->second).end();it2++)
			{
				if (transport.count(it2->transport_type)==0) continue;
				if (d[it->first].first > d[cur_num].first+(it2->cruise_time))
					{
					d[it->first].first = d[cur_num].first+(it2->cruise_time);
					d[it->first].second = d[cur_num].second+(it2->cruise_fare);
					p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
					q.push({d[it->first].first, d[it->first].second, it->first});
					}
				else if ((d[it->first].first == d[cur_num].first+(it2->cruise_time)) && 
				(d[it->first].second > d[cur_num].second +(it2->cruise_fare)))
					{
					d[it->first].first = d[cur_num].first+(it2->cruise_time);
					d[it->first].second = d[cur_num].second+(it2->cruise_fare);
					p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
					q.push({d[it->first].first, d[it->first].second, it->first});
					}
			}
		}
	}

	return p;
}

edgemap algo2(gr_type &graph, unsigned long n_cities, numset &transport, unsigned long start)
{
     unsigned long MAX_UL = numeric_limits<unsigned long>::max();
     vector<numpair> d(n_cities, {MAX_UL, MAX_UL});//d=[{стоимость, время}] Лучшие* в этот город
     edgemap p;//p[куда]=ребро - Запоминание путей
     d[start].first=0;//Стоимость
     d[start].second=0;//Время
     priority_queue<troika, vector<troika>, greater<troika>> q;//Куча {стоимость, время, номер вершины}
     q.push({0,0,start});
     while(!q.empty())
     {
         troika elem = q.top();
         unsigned long cur_time = get<1>(elem);
         unsigned long cur_cost = get<0>(elem);
         unsigned long cur_num = get<2>(elem);
         q.pop();
         if (cur_cost > d[cur_num].first) continue;
         if ((cur_cost == d[cur_num].first) && (cur_time > d[cur_num].second)) continue;
         map<unsigned long, vector<edge>> ways = graph[cur_num];
         for (map<unsigned long, vector<edge>>::iterator it=ways.begin(); it!=ways.end(); it++)
         {
             for (vector<edge>::iterator it2=(it->second).begin();it2!=(it->second).end();it2++)
             {
                 if (transport.count(it2->transport_type)==0) continue;
                 if (d[it->first].first > d[cur_num].first+(it2->cruise_fare))
                 {
                     d[it->first].first = d[cur_num].first+(it2->cruise_fare);
                     d[it->first].second = d[cur_num].second+(it2->cruise_time);
                     p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
                     q.push({d[it->first].first, d[it->first].second, it->first});
                 }
                 else if ((d[it->first].first == d[cur_num].first+(it2->cruise_fare)) &&
                           (d[it->first].second > d[cur_num].second +(it2->cruise_time)))
                 {
                     d[it->first].first = d[cur_num].first+(it2->cruise_fare);
                     d[it->first].second = d[cur_num].second+(it2->cruise_time);
                     p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
                     q.push({d[it->first].first, d[it->first].second, it->first});
                 }
             }
         }
     }

     return p;
}

edgemap algo3(gr_type &graph, unsigned long n_cities, numset &transport, unsigned long start)
{
	 unsigned long MAX_UL = numeric_limits<unsigned long>::max();
	 queue<unsigned long> q;
	 q.push(start);
     numvec d(n_cities, MAX_UL);
     edgemap p;//p[куда]=ребро - Запоминание путей
     d[start]=0;
     while(!q.empty())
     {
         unsigned long v = q.front();
         q.pop();
         map<unsigned long, vector<edge>> ways = graph[v];
         for (map<unsigned long, vector<edge>>::iterator it=ways.begin(); it!=ways.end(); it++)
         {
			 if (d[it->first]==MAX_UL)
			 {
				 for (vector<edge>::iterator it2=(it->second).begin();it2!=(it->second).end();it2++)
				 {
					 if (transport.count(it2->transport_type)==0) continue;
					 d[it->first] = d[v]+1;
					 p[it->first] = edge(v, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
					 q.push(it->first);
					 break;
				 }
			 }
         }
     }

     return p;
}

edgemap algo4(gr_type &graph, unsigned long n_cities, numset &transport, unsigned long start, unsigned long maxcost)
{
     unsigned long MAX_UL = numeric_limits<unsigned long>::max();
     numvec d(n_cities, MAX_UL);//d=[{мин стоимость до города}]
     edgemap p;//p[куда]=ребро - Запоминание путей
     d[start]=0;
     priority_queue<numpair, vector<numpair>, greater<numpair>> q;//Куча {стоимость, номер вершины}
     q.push({0,start});
     while(!q.empty())
     {
         numpair elem = q.top();
         unsigned long cur_cost = elem.first;
         unsigned long cur_num = elem.second;
         q.pop();
         if (cur_cost > d[cur_num]) continue;
         if (cur_cost > maxcost) break;
         map<unsigned long, vector<edge>> ways = graph[cur_num];
         for (map<unsigned long, vector<edge>>::iterator it=ways.begin(); it!=ways.end(); it++)
         {
             for (vector<edge>::iterator it2=(it->second).begin();it2!=(it->second).end();it2++)
             {
                 if (transport.count(it2->transport_type)==0) continue;
                 if ((d[it->first] > d[cur_num]+it2->cruise_fare) && (d[cur_num]+it2->cruise_fare <= maxcost))
                 {
                     d[it->first] = d[cur_num]+it2->cruise_fare;
                     p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
                     q.push({d[it->first], it->first});
                 }
             }
         }
     }

     return p;
}

edgemap algo5(gr_type &graph, unsigned long n_cities, numset &transport, unsigned long start, unsigned long maxtime)
{
     unsigned long MAX_UL = numeric_limits<unsigned long>::max();
     numvec d(n_cities, MAX_UL);//d=[{время до города}]
     edgemap p;//p[куда]=ребро - Запоминание путей
     d[start]=0;
     priority_queue<numpair, vector<numpair>, greater<numpair>> q;//Куча {время, номер вершины}
     q.push({0,start});
     while(!q.empty())
     {
         numpair elem = q.top();
         unsigned long cur_time = elem.first;
         unsigned long cur_num = elem.second;
         q.pop();
         if (cur_time > d[cur_num]) continue;
         if (cur_time > maxtime) break;
         map<unsigned long, vector<edge>> ways = graph[cur_num];
         for (map<unsigned long, vector<edge>>::iterator it=ways.begin(); it!=ways.end(); it++)
         {
             for (vector<edge>::iterator it2=(it->second).begin();it2!=(it->second).end();it2++)
             {
                 if (transport.count(it2->transport_type)==0) continue;
                 if ((d[it->first] > d[cur_num]+it2->cruise_time) && (d[cur_num]+it2->cruise_time <= maxtime))
                 {
                     d[it->first] = d[cur_num]+it2->cruise_time;
                     p[it->first] = edge(cur_num, it->first, it2->transport_type, it2->cruise_time, it2->cruise_fare);
                     q.push({d[it->first], it->first});
                 }
             }
         }
     }

     return p;
}

float get_rss() {
        struct rusage mem;
        getrusage(RUSAGE_SELF, &mem);
        return mem.ru_maxrss;
    }

void print_track(track &tr, map<unsigned long, string> &ind2tr, map<unsigned long, string> &ind2city)
{
	if (tr.way.size()==0)
        {
            addstr("Такого пути не существует\n");
            addstr("=================================================\n");
            addstr("\n");
            return;
        }
    for (vector<edge>::iterator it = tr.way.begin();it!=tr.way.end();it++)
    {
        addstr(ind2tr[it->transport_type].c_str());
        addstr(" из ");
        addstr(ind2city[it->from].c_str());
        addstr(" в ");
        addstr(ind2city[it->to].c_str());
        addstr(" Время ");
		addstr(to_string(it->cruise_time).c_str());
		addstr(" мин; Стоимость ");
		addstr(to_string(it->cruise_fare).c_str());
		addstr(" руб\n");
    }
    addstr("Общее время в пути ");
	addstr(to_string(tr.time).c_str());
	addstr(" мин\n");
    addstr("Общая стоимость поездки ");
	addstr(to_string(tr.cost).c_str());
	addstr(" руб\n");
}

int main(int argc, char** argv)
{
	//setlocale(LC_ALL, "ru_RU.UTF-8");
	ofstream log;
	log.open("log.txt", ios::out | ios::trunc);
	log<<"start prog max rss = "<<get_rss()<<" KB"<<endl;
    const char* input_file=argv[1];
    ifstream input(input_file);

    map<string, unsigned long> city2ind;
    map<unsigned long, string> ind2city;
    unsigned long count_id1=0;

    map<string, unsigned long> tr2ind;
    map<unsigned long, string> ind2tr;
    unsigned long count_id2=0;

    gr_type graph;
    for( string line; getline( input, line ); )
    {
        if (!line.length() || line[0]=='#') continue;
        string deli = "\"";
        string from_city;
        string to_city;
        string transport_type;
        string ct;
        string cf;
        int n1=0;
        int n2=line.find(deli);
        n1=n2+deli.size();
        n2=line.find(deli, n1);
        from_city=line.substr(n1, n2-n1);
        n1=n2+deli.size();
        n2=line.find(deli, n1);
        n1=n2+deli.size();
        n2=line.find(deli, n1);
        to_city=line.substr(n1, n2-n1);
        n1=n2+deli.size();
        n2=line.find(deli, n1);
        n1=n2+deli.size();
        n2=line.find(deli, n1);
        transport_type=line.substr(n1, n2-n1);
        n1=n2+deli.size()+1;
        n2=line.find(deli, n1);
        //Переход к делению по пробелу
        string line2=line.substr(n1, n2-n1);
        deli=" ";
        n1=0;
        n2=line2.find(deli);
        ct=line2.substr(n1, n2-n1);
        n1=n2+deli.size();
        n2=line2.find(deli, n1);
        cf=line2.substr(n1, n2-n1);
        //Заполнение идентификаторов
        unsigned long id_from=0;
        unsigned long id_to=0;
        unsigned long id_tr=0;
        if (city2ind.count(from_city)>0) id_from=city2ind[from_city];
        else
        {
            id_from=count_id1;
            city2ind[from_city]=count_id1;
            ind2city[count_id1]=from_city;
            count_id1++;
        }
        if (city2ind.count(to_city)>0) id_to=city2ind[to_city];
        else
        {
            id_to=count_id1;
            city2ind[to_city]=count_id1;
            ind2city[count_id1]=to_city;
            count_id1++;
        }
        if (tr2ind.count(transport_type)>0) id_tr=tr2ind[transport_type];
        else
        {
            id_tr=count_id2;
            tr2ind[transport_type]=count_id2;
            ind2tr[count_id2]=transport_type;
            count_id2++;
        }
        //Заполнение графа
        unsigned long cruise_time = strtoul(ct.c_str(), nullptr, 10);
        unsigned long cruise_fare = strtoul(cf.c_str(), nullptr, 10);
        edge ed=edge(id_from, id_to, id_tr, cruise_time, cruise_fare);
        graph[id_from][id_to].push_back(ed);
    }
	log<<"after graph uploading max rss = "<<get_rss()<<" KB"<<endl;
	setlocale(LC_ALL, "");
	initscr();
	scrollok(stdscr, 1);
    start_color();
	init_pair(1, COLOR_BLACK, COLOR_RED);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_GREEN, COLOR_WHITE);
    while(1)//Основной цикл программы
    {
		noecho();
		int mode = 0;
		bool chosen = false;
		const char *choices[6] = {"Найти кратчайший по времени путь (и самый дешевый из них)",
                                    "Найти путь минимальной стоимости (и самый быстрый из них)",
                                    "Найти путь с наименьшим числом пересадок",
                                    "Найти города, достижимые за заданное количество денег",
                                    "Найти города, достижимые за заданное время",
                                    "EXIT"};
		while(!chosen)
		{
			clear();
			curs_set(0);
			keypad(stdscr, true);
			addstr("Выберите режим работы\n\n");
			refresh();
			for (int i = 0; i < 6; i++)
            {
				if ((i == 5) && (i != mode))
				{
					attron(COLOR_PAIR(2));
                    printw("%s\n", choices[i]);
                    attroff(COLOR_PAIR(2));
				}
				else if ((i == 5) && (i == mode))
				{
					attron(COLOR_PAIR(1));
                    printw("%s\n", choices[i]);
                    attroff(COLOR_PAIR(1));
				}
                else if (i == mode)
                {
                    attron(A_STANDOUT);
                    printw("%s\n", choices[i]);
                    attroff(A_STANDOUT);
                }
                else
                {
                    printw("%s\n", choices[i]);
                }
                refresh();
            }
			switch (getch())
            {
                case KEY_UP:
                {
                    if (mode > 0) mode--;
                    break;
                }

                case KEY_DOWN:
                {
                    if (mode < 5) mode++;
					break;
                }
                    
                case (int)'\n':
                {
                    chosen = true;
                    break;
                }

                default: break;
            }
        }
        keypad(stdscr, false);
        if (mode==5) break;
        set<unsigned long> en_transp={}; //Разрешенный транспорт
		int tp=0;
		chosen = false;
		const char *choices_tr[4] = {"Ввести виды транспорта, на которых разрешено ехать",
                                    "Ввести виды транспорта на которых запрещено ехать",
                                    "Нет ограничений на виды транспорта",
									"Назад в меню"};
		while(!chosen)
		{
			clear();
			curs_set(0);
			keypad(stdscr, true);
			addstr("Выберите тип фильтрации транспорта\n\n");
			refresh();
			for (int i = 0; i < 4; i++)
            {
                if (i == tp)
                {
                    attron(A_STANDOUT);
                    printw("%s\n", choices_tr[i]);
                    attroff(A_STANDOUT);
                }
                else
                {
                    printw("%s\n", choices_tr[i]);
                }
                refresh();
            }
			switch (getch())
            {
                case KEY_UP:
                {
                    if (tp > 0) tp--;
                    break;
                }

                case KEY_DOWN:
                {
                    if (tp < 3) tp++;
					break;
                }
                    
                case (int)'\n':
                {
                    chosen = true;
                    break;
                }

                default: break;
            }
        }
        keypad(stdscr, false);
		if (tp==3) continue;
		if (tp==0)//Ввод разрешенных видов транспорта
		{
			char trtypec[20];
			clear();
			curs_set(1);
			keypad(stdscr, true);
			addstr("Введите названия разрешенных видов транспорта со строчной буквы через Enter\n");
			addstr("По окончании нажмите дважды Enter\n");
			echo();
			getstr(trtypec);
			string trtype = trtypec;
			while (1)
			{
				if (trtype=="") break;
				if (tr2ind.count(trtype)>0)
				{
					en_transp.insert(tr2ind[trtype]);
				}
				else
				{
					addstr("Не найден данный вид транспорта. Продолжайте ввод\n");
				}
				getstr(trtypec);
				trtype = trtypec;
			}
		}
		else if (tp==1)//Ввод запрещенных видов транспорта
		{
			char trtypec[20];
			clear();
			curs_set(1);
			keypad(stdscr, true);
			set<unsigned long> zapr;
			addstr("Введите названия запрещенных видов транспорта со строчной буквы через Enter\n");
			addstr("По окончании нажмите дважды Enter\n");
			echo();
			getstr(trtypec);
			string trtype = trtypec;
			while (1)
			{
				if (trtype=="") break;
				if (tr2ind.count(trtype)>0)
				{
					zapr.insert(tr2ind[trtype]);
				}
				else
				{
					addstr("Не найден данный вид транспорта. Продолжайте ввод\n");
				}
				getstr(trtypec);
				trtype = trtypec;
			}
			for(map<unsigned long, string>::iterator it = ind2tr.begin(); it != ind2tr.end(); ++it) {
				if (zapr.find(it->first) == zapr.end()) en_transp.insert(it->first);
			}
		}
		else if (tp==2)//Все разрешены
		{
			for(map<unsigned long, string>::iterator it = ind2tr.begin(); it != ind2tr.end(); ++it) {
				en_transp.insert(it->first);
			}
		}
		else break;
        string from;
        string to;
		char fromc[50];
		char toc[50];
        unsigned long from_id=0;
        unsigned long to_id=0;
		clear();
		keypad(stdscr, true);
		echo();
        addstr("Введите город отправления\n");
        while(1)
        {
            getstr(fromc);
			from = fromc;
            bool flag=false;
            if (city2ind.find(from) !=  city2ind.end())
			{
				from_id=city2ind[from];
				flag=true;
			}
            if (flag) break;
            addstr("Такого города не найдено в базе данных. ");
			addstr("Введите другой город или введите 0 чтобы выйти из программы\n");
        }
        if (mode==0) // 1 РЕЖИМ
        {
			addstr("Введите город прибытия\n");
            while(1)
            {
                getstr(toc);
				to = toc;
                bool flag=false;
				if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=true;
				}
                if ((to_id == from_id) && (flag==true))
                {
                    addstr("Город прибытия совпадает с городом отправления. ");
					addstr("Введите другой город или 0 чтобы выйти из программы\n");
                    continue;
                }
                if (flag==true) break;
                addstr("Такого города не найдено в базе данных. ");
				addstr("Введите другой город или введите 0 чтобы выйти из программы\n");
            }
			auto begin_time = chrono::high_resolution_clock::now();
            //Алгоритм Дейкстры 1 режима
            edgemap res = algo1(graph, count_id1, en_transp, from_id);
            //Проход по пути с его выводом
            track cruise=track({});
            unsigned long curver=to_id;
            while (curver!=from_id)
            {
				if (res.count(curver)==0) break;
                cruise=cruise+res[curver];
                curver=res[curver].from;
            }
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			addstr("\n");
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 1"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
        }
        else if (mode==1)//2 РЕЖИМ
        {
			addstr("Введите город прибытия\n");
            while(1)
            {
                getstr(toc);
				to = toc;
                bool flag=false;
                if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=true;
				}
                if ((to_id == from_id) && (flag==true))
                {
                    addstr("Город прибытия совпадает с городом отправления. ");
					addstr("Введите другой город или 0 чтобы выйти из программы\n");
                    continue;
                }
                if (flag==true) break;
                addstr("Такого города не найдено в базе данных. ");
				addstr("Введите другой город или введите 0 чтобы выйти из программы\n");
            }
			auto begin_time = chrono::high_resolution_clock::now();
            //Алгоритм Дейкстры 2 режима
            edgemap res = algo2(graph, count_id1, en_transp, from_id);
            //Проход по пути с его выводом
            track cruise=track({});
            unsigned long curver=to_id;
            while (curver!=from_id)
            {
				if (res.count(curver)==0) break;
                cruise=cruise+res[curver];
                curver=res[curver].from;
            }
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			addstr("\n");
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 2"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
        }
        else if (mode==2)
        {
			addstr("Введите город прибытия\n");
            while(1)
            {
                getstr(toc);
				to = toc;
                bool flag=false;
                if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=true;
				}
                if ((to_id == from_id) && (flag==true))
                {
                    addstr("Город прибытия совпадает с городом отправления. ");
					addstr("Введите другой город или 0 чтобы выйти из программы\n");
                    continue;
                }
                if (flag==true) break;
                addstr("Такого города не найдено в базе данных. ");
				addstr("Введите другой город или введите 0 чтобы выйти из программы\n");
            }
			auto begin_time = chrono::high_resolution_clock::now();
            //Алгоритм Дейкстры 3 режима
            edgemap res = algo3(graph, count_id1, en_transp, from_id);
            //Проход по пути с его выводом
            track cruise=track({});
            unsigned long curver=to_id;
            while (curver!=from_id)
            {
				if (res.count(curver)==0) break;
                cruise=cruise+res[curver];
                curver=res[curver].from;
            }
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			addstr("\n");
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 3"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
        }
        else if (mode==3)
        {
			bool ifcity=false;
            char maxcost_str[10];
            addstr("Введите максимальную стоимость поездки руб ");
            getstr(maxcost_str);
			addstr("\n");
            unsigned long maxcost = strtoul(maxcost_str, nullptr, 10);
			auto begin_time = chrono::high_resolution_clock::now();
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			begin_time = chrono::high_resolution_clock::now();
            //Алгоритм поиска в ширину 4 режима
            edgemap res = algo4(graph, count_id1, en_transp, from_id, maxcost);
            unsigned long i = 0;
            while(i<count_id1)
            {
                if (res.find(i) == res.end())
                {
                    i++;
                    continue;
                }
				ifcity=true;
                track cruise=track({});
                unsigned long curver=i;
                while (curver!=from_id)
                {
					if (res.count(curver)==0) break;
                    cruise=cruise+res[curver];
                    curver=res[curver].from;
                }
				end_time = chrono::high_resolution_clock::now();
				elapsed_mcs += chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
				attron(COLOR_PAIR(3));
				addstr(ind2city[i].c_str());
				attroff(COLOR_PAIR(3));
				addstr("\n");
                print_track(cruise, ind2tr, ind2city);
                addstr("Чтобы продолжить нажмите Enter, для выхода 0\n");
                char w[1];
                getstr(w);
				begin_time = chrono::high_resolution_clock::now();
                if (w=="0") break;
                i++;
            }
			end_time = chrono::high_resolution_clock::now();
			elapsed_mcs += chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
            if (ifcity==false) addstr("Таких городов нет\n\n");
            else addstr("===============Конец==============\n\n");
			log<<"calling algorithm 4"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;

        }
        else
        {
			bool ifcity=0;
            char maxtime_str[10];
            addstr("Введите максимальное время поездки мин\n");
            getstr(maxtime_str);
			addstr("\n");
            unsigned long maxtime=strtoul(maxtime_str, nullptr, 10);
			auto begin_time = chrono::high_resolution_clock::now();
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			begin_time = chrono::high_resolution_clock::now();
            //Алгоритм поиска в ширину 5 режима
            edgemap res = algo5(graph, count_id1, en_transp, from_id, maxtime);
            unsigned long i = 0;
            while(i<count_id1)
            {
                if (res.find(i) == res.end())
                {
                    i++;
                    continue;
                }
				end_time = chrono::high_resolution_clock::now();
				elapsed_mcs += chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
				attron(COLOR_PAIR(3));
                addstr(ind2city[i].c_str());
				attroff(COLOR_PAIR(3));
				ifcity=true;
                track cruise=track({});
                unsigned long curver=i;
                while (curver!=from_id)
                {
					if (res.count(curver)==0) break;
                    cruise=cruise+res[curver];
                    curver=res[curver].from;
                }
				addstr("\n");
                print_track(cruise, ind2tr, ind2city);
                addstr("Чтобы продолжить нажмите Enter, для выхода 0\n");
                char w[1];
                getstr(w);
				begin_time = chrono::high_resolution_clock::now();
                if (w=="0") break;
                i++;
            }
			if (ifcity==false) addstr("Таких городов нет\n\n");
            else addstr("===============Конец==============\n\n");
			log<<"calling algorithm 5"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
        }
		addstr("\n");
        addstr("Для продолжения нажмите любую клавишу\n");
        getch();
    }
	endwin();
	log.close();
    return 0;
}
