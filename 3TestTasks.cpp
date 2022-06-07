// 3TestTasks.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstdio>
#include <unordered_map>


/*
* Зинченко Андрей Иванович
* 02.06.2022г
* 1 задание ~ 15 мин
* 2 задание ~ 15 мин
* 3 задание ~ 2 часа
Все три задания были отлажены(т.е. время включает затраты на отладку кода)
*/


/*
* 1. Напишите функцию, которая принимает на вход знаковое целое число и
печатает его двоичное представление, не используя библиотечных классов или функций.
*/

void getAndPrintBinary(int snumber)
{
    uint32_t number = (uint32_t) snumber;
    uint32_t mask = 0x80000000;
    while (mask)
    {
        if (mask <= number)
        {            
            //std::cout << ((mask & number) ? 1 : 0);//C++
            printf("%d",(mask & number) ? 1 : 0);//C          
        } 
        mask >>= 1;
    }
    //std::cout << std::endl;
    printf("\r\n");
}

/*
* 2. Напишите функцию, удаляющую последовательно дублирующиеся символы в строке:
void RemoveDups(char* str);
// пример использования
char data[] = “AAA BBB AAA”;
RemoveDups(data);
printf(“%s\n”, data); // “A B A”
*/
void RemoveDups(char* str)
{
    char currentChar = str[0];
    size_t indexDest = 0;
    size_t N = strlen(str);
    size_t i = 0;    
    while (i < N)
    {        
        if ((currentChar != str[i]) || ((i+1) >= N))
        {
            str[indexDest++] = currentChar;
            currentChar = str[i];
        }
        i++;
    }
    str[indexDest] = 0;

}


/*
* 3. Реализуйте функции сериализации и десериализации двусвязного списка
в бинарном формате в файл. Алгоритмическая сложность решения должна быть меньше
квадратичной.
*/
// структуру ListNode модифицировать нельзя
struct ListNode {
    ListNode* prev;
    ListNode* next;
    ListNode* rand; // указатель на произвольный элемент данного списка, либо NULL
    std::string data;
};

class List {
public:
    void Serialize(FILE* file); // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
    void Deserialize(FILE* file); // загрузка из файла (файл открыт с помощью fopen(path, "rb"))
    List() :head{ NULL }, tail{ NULL }, count{}
    {

    }
    void clear();
    ListNode* add(const std::string & str, ListNode* rand = NULL);

    ~List()
    {
        clear();
    }
private:
    ListNode* head;//Указатель на голову
    ListNode* tail;//Указатель на хвост
    int count;//Количество элементов списка
};


/*Структура BIN-файла:
* uint32_t количество элементов сохраненного списка
* 
* Элемент 1:
* uint32_t пересчитанный в индексы элементов списка указатель RAND
* uint16_t длина строки string (без 0 терминатора)
* char[N] - массив символов строки
* 
* * Элемент 2:
* uint32_t пересчитанный в индексы элементов списка указатель RAND
* uint16_t длина строки string (без 0 терминатора)
* char[N] - массив символов строки
* 
* ...................
* 
* * Элемент Last:
* uint32_t пересчитанный в индексы элементов списка указатель RAND
* uint16_t длина строки string (без 0 терминатора)
* char[N] - массив символов строки
*/


//Сохраняем список с данными и связями в файл.Сложность ~O(n).
//(Благодаря использованию unordered_map,имеющую сложность O(1))
void List::Serialize(FILE* file)
{
    std::unordered_map<ListNode*, int> linkMap{};    
    //Первый проход:
    //Создаем массив пар ключ:значение, где ключ - это указатели на все ноды,
    //а значение - порядковый номер ноды:
    ListNode* currentNode = head;
    for (int i = 1;i<= count;i++)
    {
        linkMap[currentNode] = i;//0-й индекс резервируем для rand ==NULL
        currentNode = currentNode->next;
    }
    //Сохраним в файле общее кол-во элементов списка:    
    fwrite(&count, sizeof(count), 1, file);    

    //Второй проход: Получаем индексы элементов списка,на которые ссылается 
    //RAND и сохраняем в бинарном файле поэлементно.
    currentNode = head;
    for (int i = 0; i < count; i++)
    {
        uint32_t rand{0};
        if (currentNode->rand != NULL)
        {
            rand = linkMap[currentNode->rand];//Получаем индекс (1..N) элемента из unordered_map
        }
        //Сохраним в файле rand:    
        fwrite(&rand, sizeof(rand), 1, file);

        //Сохраним в файле строку:
        //длину:
        uint16_t strLength = currentNode->data.length();
        fwrite(&strLength, sizeof(strLength), 1, file);
        fwrite(currentNode->data.c_str(), strLength, 1, file);
        
        currentNode = currentNode->next;
    }
    
}

//Восстанавливаем список с данными и связями из файла.Сложность ~O(n).
void List::Deserialize(FILE* file)
{    
    //Считаем из файла общее кол-во сохраненных элементов:
    int tmpCount;
    fread(&tmpCount, sizeof(tmpCount), 1, file);
        

    std::vector<ListNode*> vecNodes;    
    //Первый проход- создаем список поэлементно,сохраняя массив новых указателей
    //на элементы для быстрого доступа:
    ListNode* currentNode{};
    vecNodes.push_back(NULL);
    if (tmpCount == count)
    {
    //Используем текущий список, если считываемый из файла (список) содержит 
    // такое же кол-во элементов, ибо выделения памяти для нового списка могут быть
    //времязатратными
        currentNode = head;
        for (int i = 0; i < tmpCount; i++)
        {                     
            vecNodes.push_back(currentNode);
            currentNode = currentNode->next;
        }        
    }
    else
    {
        //Если список другого размера,то удалим текущий и создадим новый.
        // (Можно было бы для оптимизации скорости уравнять длины списков 
        // путем добавления/удаления элементов,
        // однако для простоты удалим старый и создадим новый)
        //Удаляем старый список:
        clear();

        //Выделим память для каждого из элементов,
        //а также создадим массив для быстрого доступа к ним:
        for (int i = 0; i < tmpCount; i++)
        {
            currentNode = new ListNode;
            //Если первый элемент, то это -голова,настроим prev:
            if (!i)
            {
                head = currentNode;
                head->prev = NULL;
            }
            vecNodes.push_back(currentNode);
        }
        //Если последний элемент, то это -хвост,настроим next:
        tail = currentNode;
        tail->next = NULL;

    }

    

    //Второй проход- создаем связи элементов списка, восстанавливаем из
    //файла строки данных:
    currentNode = head;
    for (int i = 1; i <= tmpCount; i++)
    {
        //Если используем уже существующий список,то связи в нем уже есть,
        //Однако для нового списка нужно расставить связи:
        if (tmpCount != count)
        {
            if (i < tmpCount)
                currentNode->next = vecNodes[i + 1];//ссылаемся на следующий
            if (i > 1)            
                currentNode->prev = vecNodes[i - 1];//ссылаемся на предыдущий
            
        }
        //Прочитаем из файла rand:
        uint32_t rand{};
        fread(&rand, sizeof(rand), 1, file);
        currentNode->rand = vecNodes[rand];//rand будет указывать на тот же элемент,что и до сериализации

        //Прочитаем данные строки:        
        //длину:
        uint16_t strLength{};
        fread(&strLength, sizeof(strLength), 1, file);
        //саму строку:
        currentNode->data.clear();
        char ch{};
        for (int i = 0;  i < strLength; i++)
        {
            fread(&ch, sizeof(ch), 1, file);
            currentNode->data += ch;
        }
        currentNode = currentNode->next;
    }
    count = tmpCount;
}

//освобождаем память,удаляем список:
void List::clear()
{    
    if (count)
    {
        ListNode* currentNode = head;
        for (int i = 0; i < count; i++)
        {
            ListNode* tmpcurrentNode = currentNode;
            currentNode = currentNode->next;
            delete tmpcurrentNode;
        }
        count = 0;
        head = tail = NULL;
    }
}

//Метод,добавляющий элемент в список(добавлен для тестовых целей):
ListNode* List::add(const std::string& str, ListNode* rand)
{
    ListNode* currentNode = new ListNode;
    
    //Добавляем новый элемент в конец списка с перенастройкой 
    //соответствующих связей:
    if (!count)
    {//Если список пустой:
        head = tail = currentNode;
        head->prev = tail->prev = NULL;
        head->next = tail->next = NULL;        
    }
    else
    {
        tail->next = currentNode;
        currentNode->prev = tail;
        currentNode->next = NULL;
        tail = currentNode;        
    }
    currentNode->rand = rand;
    currentNode->data = str;
    count++;    
    return currentNode;
}



int main()
{
    //1-е задание
    getAndPrintBinary(0xAA);

    //2-е задание
    char data[] = "AAA BBB AAA";
    RemoveDups(data);
    printf("%s\n", data); // “A B A”

    //3-е задание
    //Создаем список
    List lst;
    //Заполняем его данными:
    ListNode* pnode1 = lst.add("Hello!");//rand 1-го элемента списка пусть будет равен NULL
    ListNode* pnode2 = lst.add("my name",pnode1);//rand 2-го элемента списка ссылается на первый
    ListNode* pnode3 = lst.add("is Andrew.",pnode2);//rand 3-го элемента списка ссылается на второй

    //Сохраняем в файл наш список с данными и связями rand:
    FILE* file = fopen("data.bin", "wb");
    lst.Serialize(file);
    fclose(file);

    //Очищаем текущий список(можно закомментировать),
    //тогда класс List будет использовать текущий список для загрузки:
    lst.clear();


    //Загружаем(восстанавливаем) из файла данные списка со связями,
    //Текущий список идентичен с точки зрения связей и данных с тем,который 
    //был сохранен в файл:
    file = fopen("data.bin", "rb");
    lst.Deserialize(file);
    fclose(file);
    
    //Для проверки.Выполняем повторную сериализацию в другой файл.Так как список был
    //очищен, то в случае безупречной сериализации/десериализации
    //мы должны получить два абсолютно одинаковых файла:
    file = fopen("data2.bin", "wb");
    lst.Serialize(file);
    fclose(file);
}

