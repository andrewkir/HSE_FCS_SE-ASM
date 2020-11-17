#include <iostream>
#include <thread>
#include <mutex>
#include <stack>


using namespace std;

mutex mutexLock;

double* percentages = new double[8]{ 0,1,2,3,4,5,6,7 };
int inheritance;

void funcTree(int depth, double& result, int leftInt, int rightInt) {
	if (depth == 3) {
		//Параллельный подсчет суммы
		result = percentages[leftInt] * inheritance; //На данной глубине концы интервалов равны, поэтому берем одно из крайних значений
	}
	else {
		double l = 0, r = 0;

		//Передача в функцию интервалов, каждый раз разделяя их пополам
		int intLen = (rightInt - leftInt) / 2;
		thread left(funcTree, depth + 1, ref(l), leftInt, leftInt + intLen);
		thread right(funcTree, depth + 1, ref(r), rightInt - intLen, rightInt);

		//Ожидание завершения работы потоков
		left.join();
		right.join();
		result = l + r;
	}
}

double* generatePercentage() {
	bool isLawyerLies = rand() % 2 == 0; //С вероятностью 50 процентов адвокат обманет семью
	double* result = new double[8];

	double sum = 1;
	double tmp = (rand() % 20) / 100.0;
	result[0] = tmp;
	sum -= tmp;
	for (int i = 1; i < 7; i++)
	{
		tmp = (rand() % (int)(sum * 60)) / 100.0;
		if (isLawyerLies && tmp >= 0.2) {
			tmp -= 0.05; //Если адвокат решил обмануть наследников, он ворует 10 процентов у наследников, у которых доля >= 20%;
		}
		sum -= tmp;
		result[i] = tmp;
	}
	if (isLawyerLies) result[7] = sum / 2;
	else result[7] = sum;

	return result;
}

int main()
{
	srand(static_cast<unsigned int>(time(0)));
	setlocale(LC_ALL, "Russian");

	inheritance = rand() % 500000 + 500000;

	cout << "Дон Энрике оставил в наследство " << inheritance << "$" << endl;

	percentages = generatePercentage();
	cout << "Адвокат, на основе завещания Дона, посчитал следующее процентное распределение наследства среди наследников:" << endl;
	double s = 0;
	for (int i = 0; i < 7; i++)
	{
		cout << percentages[i] * 100 << "%, ";
	}
	cout << percentages[7] * 100 << "%" << endl << endl;

	double resultSum;
	thread main(funcTree, 0, ref(resultSum), 0, 7);
	main.join();
	cout << "Адвокат распределил "<< resultSum<<"$ между наследниками"<< endl;
	if ((int)resultSum == (int)inheritance) {
		cout << "Адвокат честно поработал"<<endl;
	}
	else {
		cout << "Адвокат обманул семью на " << inheritance - resultSum << "$!!!" << endl;
	}
}
