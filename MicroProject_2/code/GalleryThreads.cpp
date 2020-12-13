/*
    Кирюхин Андрей Алексеевич, БПИ191
    Вариант - 18
    Условие задачи:
    Задача о картинной галерее. Вахтер следит за тем, чтобы в
    картинной галерее было не более 50 посетителей. Для обозрения
    представлены 5 картин. Посетитель ходит от картины к картине, и если на
    картину любуются более чем десять посетителей, он стоит в стороне и ждет,
    пока число желающих увидеть картину не станет меньше. Посетитель может
    покинуть галерею. Создать многопоточное приложение, моделирующее
    работу картинной галереи
*/
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <time.h>
#include <condition_variable>


using namespace std;

unsigned seed = chrono::system_clock::now().time_since_epoch().count();
auto rng = default_random_engine{seed};
mutex locker;
std::condition_variable cv[5];


class WatchMan;

//Класс посетителя картинной галереи
class Visitor {
public:
    //При создании посетителя, у него в случайном порядке генерируется путь обхода всех картин в галерее.
    Visitor(string _name, int _id) {
        name = _name;
        id = _id;
        shuffle(std::begin(path), std::end(path), rng);
    }

    string GetName() {
        return name + "(id: " + to_string(id) + ")";
    }

    void startExploring(WatchMan &watchMan);
    bool isWaiting = false;
private:
    string name;
    int id;
    int currentPainting;
    vector<int> path = {0, 1, 2, 3, 4};
    WatchMan *watchMan;

    //Получение следующей картины для посещения
    int getNextPainting() {
        if (path.empty()) return -1;
        int painting = path.back();
        path.pop_back();
        return painting;
    }

    void lookAtCurrentPainting();

    void moveToNextPainting();
};

//Класс картинной галереи
class Gallery {
public:
    //Инкремент количества посетителей смотрящих определенную картину
    void paintingAddVisitor(int index) {
        locker.lock();
        paintings[index]++;
        locker.unlock();
    }

    //Уход посетителя от конкретной картины
    void leavePaintingVisitor(int index) {
        locker.lock();
        if (paintings[index] != 0) {
            paintings[index]--;
        }
        if(!paintingsQueue[index].empty()){
            paintingsQueue[index].front()->isWaiting = false;
            paintingsQueue[index].pop();
        }
        locker.unlock();
        cv[index].notify_all();
    }

    //Получение количество посетителей, смотрящих определенную картину
    int getVisitorsNumAt(int index) {
        locker.lock();
        int result = paintings[index];
        locker.unlock();
        return result;
    }

    void addVisitorToPaintingQueue(Visitor &visitor, int index){
        locker.lock();
        paintingsQueue[index].push(&visitor);
        locker.unlock();
    }

private:
    int paintings[5] = {0};
    queue<Visitor*> paintingsQueue[5];
};

Gallery mainGallery;

class WatchMan {
public:
    //Запуск посетителя в галерею:
    //Если в галерее меньше 50 посетителей, то создается новый поток для посетителя и он начинает обход галереи
    void getNextPersonFromQueue() {
        if (!visitorsQueue.empty() && amountInGallery < 50) {
            Visitor visitor = visitorsQueue.front();
            thread v(&Visitor::startExploring, visitor, ref(*this));
            v.detach();
            visitorsQueue.pop();
            amountInGallery++;
        }
    }

    //Добавление посетителя в очередь в галерею
    void addVisitorToQueue(Visitor &visitor) {
        locker.lock();
        visitorsQueue.push(visitor);
        cout << visitor.GetName() + " in the entrance queue" << endl;
        locker.unlock();
        getNextPersonFromQueue();
    }

    //Проверка не осталось ли посетителей в очереди и в самой галереи, установлено пятисекундное окно
    //чтобы избежать ситуации, когда посетитель ушел из очереди и ещё не попал в галерею, а программа уже завершилась
    void isActivePeople() {
        while (true) {
            locker.lock();
            if (visitorsQueue.empty() && amountInGallery == 0){
                this_thread::sleep_for(chrono::seconds(2));
                if (visitorsQueue.empty() && amountInGallery == 0) return;
            }
            locker.unlock();
            this_thread::sleep_for(chrono::seconds(5));
        }
    }

    //Уведомление вахтера, что посетитель покинул галерею
    void notifyLeft() {
        locker.lock();
        amountInGallery--;
        if (amountInGallery < 50) getNextPersonFromQueue();
        locker.unlock();
    }

private:
    queue<Visitor> visitorsQueue;
    int amountInGallery = 0;
};

//Начало прохода по картинной галерее
void Visitor::startExploring(WatchMan &_watchMan) {
    watchMan = &_watchMan;
    locker.lock();
    cout << GetName() << " started exploring the gallery" << endl;
    locker.unlock();

    moveToNextPainting();
}

//Метод просмотра текущей картины посетителем и переход к следующей картине
void Visitor::lookAtCurrentPainting() {
    mainGallery.paintingAddVisitor(currentPainting);

    locker.lock();
    cout << GetName() << " now looking at painting N" << to_string(currentPainting) << endl;
    locker.unlock();
    this_thread::sleep_for(chrono::seconds(3));

    mainGallery.leavePaintingVisitor(currentPainting);

    locker.lock();
    cout << GetName() << " walked away from painting N" << to_string(currentPainting) << endl;
    locker.unlock();
    moveToNextPainting();
}

//Переход посетителя к следующей картине, если на картину смотрят больше 10 человек,
//то он ждет в стороне пока количество смотрящих не будет < 10
void Visitor::moveToNextPainting() {
    if (path.empty()) {
        locker.lock();
        cout << GetName() << " left the gallery" << endl;
        locker.unlock();
        watchMan->notifyLeft();
        return;
    }
    currentPainting = getNextPainting();
    if (mainGallery.getVisitorsNumAt(currentPainting) > 10) {
        locker.lock();
        cout << GetName() << " in the queue to painting N" << currentPainting << endl;
        locker.unlock();

        isWaiting = true;
        mainGallery.addVisitorToPaintingQueue(*this, currentPainting);

        unique_lock<std::mutex> lck(locker);
        while(isWaiting) {
            cv[currentPainting].wait(lck);
        }
        lck.unlock();
        lookAtCurrentPainting();
    } else {
        lookAtCurrentPainting();
    }
}

int main() {
    cout<<"Enter amount of visitors:"<<endl;
    int amount;
    while(true){
        cin >> amount;
        if(amount <= 0) cout<<"Try again, amount must be > 0"<<endl;
        else break;
    }
    cout<<endl;
    srand(static_cast<unsigned int>(time(0)));
    string names[] = {"Adam", "John", "Max", "Andrew", "Alex", "Bill", "Mary", "Ann", "Kate", "William", "Ella", "Lucy",
                      "Kinsley", "Kendall"};
    WatchMan watchMan;
    for (int i = 0; i < amount; ++i) {
        Visitor visitor(names[rand() % names->length()], i);
        watchMan.addVisitorToQueue(visitor);
    }

    //Создаем поток, проверяющий каждые 5 секунд, не остались ли ещё люди в очереди или в галерее, при завершении
    //этого потока можно завершать программу.
    thread watchManThread(&WatchMan::isActivePeople, ref(watchMan));
    watchManThread.join();
}
