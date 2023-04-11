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
        else if (c==0)
        {
            cout<<"Случай не предусмотрен"<<endl;
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
    cout<<endl;
	if (tr.way.size()==0)
        {
            cout<<"Такого пути не существует"<<endl;
            cout<<"================================================="<<endl;
            cout<<endl;
            return;
        }
    for (vector<edge>::iterator it = tr.way.begin();it!=tr.way.end();it++)
    {
        cout<<ind2tr[it->transport_type];
        cout<<" из ";
        cout<<ind2city[it->from];
        cout<<" в ";
        cout<<ind2city[it->to];
        cout<<" Время "<<it->cruise_time<<" мин; Стоимость "<<it->cruise_fare<<" руб"<<endl;
    }
    cout<<"Общее время в пути "<<tr.time<<" мин"<<endl;
    cout<<"Общая стоимость поездки "<<tr.cost<<" руб"<<endl;
    cout<<"================================================="<<endl;
    cout<<endl;
}

int main(int argc, char** argv)
{
	ofstream log;
	log.open("log.txt", ios::out | ios::trunc);
	log<<"start prog max rss = "<<get_rss()<<" KB"<<endl;
    const char* input_file=argv[1];
    setlocale(0, "");
    //SetConsoleOutputCP(65001);

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
    while(1)//Основной цикл программы
    {
        cout<<endl;
        cout<<"Главное меню\nВыберите режим работы"<<endl;
        cout<<"1 - Найти кратчайший по времени путь"<<endl;
        cout<<"2 - Найти путь минимальной стоимости"<<endl;
        cout<<"3 - Найти путь с наименьшим числом пересадок"<<endl;
        cout<<"4 - Найти города, достижимые за заданное количество денег"<<endl;
        cout<<"5 - Найти города, достижимые за заданное время"<<endl;
        cout<<"0 - EXIT"<<endl;
        cout<<">> ";
        int mode;
        cin>>mode;
        cout<<endl;
        if (mode==0) exit(0);
        set<unsigned long> en_transp={}; //Разрешенный транспорт
        cout<<"1 - Ввести виды транспорта на которых разрешено ехать"<<endl;
        cout<<"2 - Ввести виды транспорта на которых запрещено ехать"<<endl;
        cout<<"3 - Нет ограничений на виды транспорта"<<endl;
        cout<<">> ";
        string tp;
		cin.clear();
        while(1)
        {
			cin.clear();
            cin>>tp;
			cout<<tp<<endl;
            cout<<endl;
            if (tp=="1")//Ввод разрешенных видов транспорта
            {
                cout<<"Введите названия разрешенных видов транспорта со строчной буквы через Enter"<<endl;
                cout<<"По окончании введите 0"<<endl;
                string trtype;
                cin.ignore();
                getline(cin, trtype);
                while (1)
                {
                    if (trtype=="0") break;
                    if (tr2ind.count(trtype)>0)
                    {
                        en_transp.insert(tr2ind[trtype]);
                    }
                    else
                    {
                        cout<<"Не найден данный вид транспорта. Продолжайте ввод"<<endl;
                    }
                    getline(cin, trtype);
                }
                break;
            }
            else if (tp=="2")//Ввод запрещенных видов транспорта
            {
                set<unsigned long> zapr;
                cout<<"Введите названия запрещенных видов транспорта со строчной буквы через Enter"<<endl;
                cout<<"По окончании введите 0"<<endl;
                string trtype;
                cin.ignore();
                getline(cin, trtype);
                while (1)
                {
                    if (trtype=="0") break;
                    if (tr2ind.count(trtype)>0)
                    {
                        zapr.insert(tr2ind[trtype]);
                    }
                    else
                    {
                        cout<<"Не найден данный вид транспорта. Продолжайте ввод"<<endl;
                    }
                    getline(cin, trtype);
                }
                for(map<unsigned long, string>::iterator it = ind2tr.begin(); it != ind2tr.end(); ++it) {
                    if (zapr.find(it->first) == zapr.end()) en_transp.insert(it->first);
                }
                break;
            }
            else if (tp=="3")//Все разрешены
            {
                cout<<"Разрешены все известные виды транспорта"<<endl;
                cout<<endl;
                cin.ignore();
                for(map<unsigned long, string>::iterator it = ind2tr.begin(); it != ind2tr.end(); ++it) {
                    en_transp.insert(it->first);
                }
                break;
            }
            else
            {
                cout<<"Вы Ошиблись. Введите цифру заново"<<endl;
				cout<<">> ";
            }
        }
        string from;
        string to;
        unsigned long from_id=0;
        unsigned long to_id=0;
        cout<<"Введите город отправления: "<<endl;
        while(1)
        {
            getline(cin, from);
            if (from=="0") exit(0);
            short flag=0;
            if (city2ind.find(from) !=  city2ind.end())
			{
				from_id=city2ind[from];
				flag=1;
			}
            if (flag==1) break;
            cout<<"Такого города не найдено в базе данных. ";
			cout<<"Введите другой город или введите 0 чтобы выйти из программы"<<endl;
        }
        if (mode==1) // 1 РЕЖИМ
        {
            cout<<"Введите город прибытия: "<<endl;
            while(1)
            {
                getline(cin, to);
                if (to=="0") exit(0);
                short flag=0;
				if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=1;
				}
                if ((to_id == from_id) && (flag==1))
                {
                    cout<<"Город прибытия совпадает с городом отправления. ";
					cout<<"Введите другой город или 0 чтобы выйти из программы"<<endl;
                    continue;
                }
                if (flag==1) break;
                cout<<"Такого города не найдено в базе данных. ";
				cout<<"Введите другой город или введите 0 чтобы выйти из программы"<<endl;
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
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 1"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
			cout<<endl;
        }
        else if (mode==2)//2 РЕЖИМ
        {
            cout<<"Введите город прибытия: "<<endl;
            while(1)
            {
                getline(cin, to);
                if (to=="0") exit(0);
                short flag=0;
                if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=1;
				}
                if ((to_id == from_id) && (flag==1))
                {
                    cout<<"Город прибытия совпадает с городом отправления. ";
					cout<<"Введите другой город или введите 0 чтобы выйти"<<endl;
                    continue;
                }
                if (flag==1) break;
                cout<<"Такого города не найдено в базе данных. ";
				cout<<"Введите другой город или введите 0 чтобы выйти из программы"<<endl;
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
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 2"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
			cout<<endl;
        }
        else if (mode==3)
        {
            cout<<"Введите город прибытия: "<<endl;
            while(1)
            {
                getline(cin, to);
                if (to=="0") exit(0);
                short flag=0;
                if (city2ind.find(to) !=  city2ind.end())
				{
					to_id=city2ind[to];
					flag=1;
				}
                if ((to_id == from_id) && (flag==1))
                {
                    cout<<"Город прибытия совпадает с городом отправления. ";
					cout<<"Введите другой город или 0 чтобы выйти"<<endl;
                    continue;
                }
                if (flag==1) break;
                cout<<"Такого города не найдено в базе данных. ";
				cout<<"Введите другой город или 0 чтобы выйти"<<endl;
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
            print_track(cruise, ind2tr, ind2city);
			log<<"calling algorithm 3"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
			cout<<endl;
        }
        else if (mode==4)
        {
			short ifcity=0;
            string maxcost_str;
            cout<<"Введите максимальную стоимость поездки руб: ";
            cin>>maxcost_str;
            unsigned long maxcost = strtoul(maxcost_str.c_str(), nullptr, 10);
			auto begin_time = chrono::high_resolution_clock::now();
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			begin_time = chrono::high_resolution_clock::now();
            //Алгоритм поиска в ширину 4 режима
            edgemap res = algo4(graph, count_id1, en_transp, from_id, maxcost);
            cout<<endl;
            unsigned long i = 0;
            while(i<count_id1)
            {
                if (res.find(i) == res.end())
                {
                    i++;
                    continue;
                }
                cout<<ind2city[i]<<endl;
				ifcity=1;
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
                print_track(cruise, ind2tr, ind2city);
                cout<<"Чтобы вывести новый город введите 1, чтобы завершить алгоритм, введите 0"<<endl;
                cout<<">> ";
                string w;
                cin>>w;
				cout<<endl;
				begin_time = chrono::high_resolution_clock::now();
                if (w=="0") break;
                i++;
            }
			end_time = chrono::high_resolution_clock::now();
			elapsed_mcs += chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
            if (ifcity==0) cout<<"Таких городов нет"<<endl;
            else cout<<"===============Конец=============="<<endl;
			cout<<endl;
			log<<"calling algorithm 4"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
			cout<<endl;

        }
        else
        {
			short ifcity=0;
            string maxtime_str;
            cout<<"Введите максимальное время поездки мин: ";
            cin>>maxtime_str;
            unsigned long maxtime=strtoul(maxtime_str.c_str(), nullptr, 10);
			auto begin_time = chrono::high_resolution_clock::now();
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_mcs = chrono::duration_cast<chrono::microseconds>(end_time - begin_time);
			begin_time = chrono::high_resolution_clock::now();
            //Алгоритм поиска в ширину 5 режима
            edgemap res = algo5(graph, count_id1, en_transp, from_id, maxtime);
            cout<<endl;
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
                cout<<ind2city[i]<<endl;
				ifcity=1;
                track cruise=track({});
                unsigned long curver=i;
                while (curver!=from_id)
                {
					if (res.count(curver)==0) break;
                    cruise=cruise+res[curver];
                    curver=res[curver].from;
                }
                print_track(cruise, ind2tr, ind2city);
                cout<<"Чтобы вывести новый город введите 1, чтобы завершить алгоритм, введите 0"<<endl;
                cout<<">> ";
                string w;
                cin>>w;
				cout<<endl;
				begin_time = chrono::high_resolution_clock::now();
                if (w=="0") break;
                i++;
            }
			if (ifcity==0) cout<<"Таких городов нет"<<endl;
            else cout<<"===============Конец=============="<<endl;
			cout<<endl;
			log<<"calling algorithm 5"<<endl;
			log<<"algo time elapsed "<<elapsed_mcs.count()<<" mcs"<<endl;
			log<<"max rss = "<<get_rss()<<" KB"<<endl;
			log<<"========================"<<endl;
			cout<<endl;
        }

        cout<<"Для продолжения введите 1, для выхода 0"<<endl;
        short num;
        cin>>num;
        cout<<endl;
        if (num==0) exit(0);
    }
    return 0;
}
