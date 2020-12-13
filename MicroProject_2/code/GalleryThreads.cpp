/*
    ������� ������ ����������, ���191
    ������� - 18
    ������� ������:
    ������ � ��������� �������. ������ ������ �� ���, ����� �
    ��������� ������� ���� �� ����� 50 �����������. ��� ���������
    ������������ 5 ������. ���������� ����� �� ������� � �������, � ���� ��
    ������� �������� ����� ��� ������ �����������, �� ����� � ������� � ����,
    ���� ����� �������� ������� ������� �� ������ ������. ���������� �����
    �������� �������. ������� ������������� ����������, ������������
    ������ ��������� �������
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

//����� ���������� ��������� �������
class Visitor {
public:
    //��� �������� ����������, � ���� � ��������� ������� ������������ ���� ������ ���� ������ � �������.
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

    //��������� ��������� ������� ��� ���������
    int getNextPainting() {
        if (path.empty()) return -1;
        int painting = path.back();
        path.pop_back();
        return painting;
    }

    void lookAtCurrentPainting();

    void moveToNextPainting();
};

//����� ��������� �������
class Gallery {
public:
    //��������� ���������� ����������� ��������� ������������ �������
    void paintingAddVisitor(int index) {
        locker.lock();
        paintings[index]++;
        locker.unlock();
    }

    //���� ���������� �� ���������� �������
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

    //��������� ���������� �����������, ��������� ������������ �������
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
    //������ ���������� � �������:
    //���� � ������� ������ 50 �����������, �� ��������� ����� ����� ��� ���������� � �� �������� ����� �������
    void getNextPersonFromQueue() {
        if (!visitorsQueue.empty() && amountInGallery < 50) {
            Visitor visitor = visitorsQueue.front();
            thread v(&Visitor::startExploring, visitor, ref(*this));
            v.detach();
            visitorsQueue.pop();
            amountInGallery++;
        }
    }

    //���������� ���������� � ������� � �������
    void addVisitorToQueue(Visitor &visitor) {
        locker.lock();
        visitorsQueue.push(visitor);
        cout << visitor.GetName() + " in the entrance queue" << endl;
        locker.unlock();
        getNextPersonFromQueue();
    }

    //�������� �� �������� �� ����������� � ������� � � ����� �������, ����������� ������������� ����
    //����� �������� ��������, ����� ���������� ���� �� ������� � ��� �� ����� � �������, � ��������� ��� �����������
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

    //����������� �������, ��� ���������� ������� �������
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

//������ ������� �� ��������� �������
void Visitor::startExploring(WatchMan &_watchMan) {
    watchMan = &_watchMan;
    locker.lock();
    cout << GetName() << " started exploring the gallery" << endl;
    locker.unlock();

    moveToNextPainting();
}

//����� ��������� ������� ������� ����������� � ������� � ��������� �������
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

//������� ���������� � ��������� �������, ���� �� ������� ������� ������ 10 �������,
//�� �� ���� � ������� ���� ���������� ��������� �� ����� < 10
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

    //������� �����, ����������� ������ 5 ������, �� �������� �� ��� ���� � ������� ��� � �������, ��� ����������
    //����� ������ ����� ��������� ���������.
    thread watchManThread(&WatchMan::isActivePeople, ref(watchMan));
    watchManThread.join();
}
