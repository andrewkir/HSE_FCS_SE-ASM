/*
    Кирюхин Андрей Алексеевич, БПИ191
    Вариант - 18
    Условие задачи:
    Задача о наследстве. У старого дона Энрике было два сына, у
    каждого из сыновей – еще по два сына, каждый из которых имел еще по два
    сына. Умирая, дон Энрике завещал все свое богатство правнукам в разных
    долях. Адвокат дон Хосе выполнил задачу дележа наследства в меру своих
    способностей. Правнуки заподозрили адвоката в укрывательстве части
    наследства. Требуется создать многопоточное приложение, которое при
    известных сумме завещания дона Энрике и доле каждого наследника,
    проверяет честность адвоката. При решении использовать принцип
    дихотомии
*/
#include <iostream>
#include <time.h>

using namespace std;

double* percentages;
double* lawyerCalcInheritance;
int inheritance;

//Рекурсивная функция для формирования древа семьи для подсчета суммы полученной наследниками от Дона
void funcTree(int depth, int leftInt, int rightInt) {
    //depth = 3, мы дошли до 8 правнуков, останавливаем вызов новых функций дальше
    if (depth == 3) {
        //Параллельный подсчет суммы
        double result = percentages[leftInt] *
            inheritance; //На данной глубине концы интервалов равны, поэтому берем одно из крайних значений
#pragma omp critical
        {
            if (result - lawyerCalcInheritance[leftInt] > 0.001) {
                cout << "The lawyer deceived the " << leftInt + 1 << " heir for "
                    << result - lawyerCalcInheritance[leftInt] << "$" << endl;
            }
            else if (lawyerCalcInheritance[leftInt] - result > 0.01) {
                cout << "The lawyer gave " << lawyerCalcInheritance[leftInt] - result << "$ more to " << leftInt + 1
                    << " heir" << endl;
            }
            else {
                cout << "The lawyer correctly counted the share of " << leftInt + 1 << " heir" << endl;
            }
        }
    }
    else {
        //Передача в функцию интервалов, каждый раз разделяя их пополам
        int intLen = (rightInt - leftInt) / 2;
#pragma omp task shared(intLen, depth, leftInt) default(none)
        funcTree(depth + 1, leftInt, leftInt + intLen);
#pragma omp task shared(intLen, depth, rightInt) default(none)
        funcTree(depth + 1, rightInt - intLen, rightInt);
    }
}

/*
    Генерация процентных долей наследников на основе "завещания" Дона.
*/
void generatePercentage() {
    percentages = new double[8];

    double sum = 1;
    double tmp = (rand() % 20) / 100.0 + 0.05;
    percentages[0] = tmp;

    sum -= tmp;
    for (int i = 1; i < 7; i++) {
        tmp = (rand() % (int)(sum * 60)) / 100.0;
        sum -= tmp;
        percentages[i] = tmp;
    }
    percentages[7] = sum;
}

/*
    Генерация долей, посчитанных адвокатом (в этот момент он может обмануть наследников)
 */
void generateLawyerCalculatedInheritance() {

    lawyerCalcInheritance = new double[8];
    bool isLawyerLying = rand() % 3 == 0;
    if (!isLawyerLying) {
        for (int i = 0; i < 8; i++) {
            lawyerCalcInheritance[i] = percentages[i] * inheritance;
        }
    }
    else {
        // Если адвокат решит обмануть наследников, он может с вероятностью 1/3 забирать у них 1/6 наследства,
        // либо он может перепутает проценты между наследниками
        if (rand() % 2 == 0) {
            for (int i = 0; i < 8; i++) {
                if (rand() % 3 == 0) {
                    lawyerCalcInheritance[i] = (percentages[i] / 6.0) * inheritance;
                }
                else {
                    lawyerCalcInheritance[i] = percentages[i] * inheritance;
                }
            }
        }
        else {
            for (int i = 0; i < 8; i++) {
                lawyerCalcInheritance[i] = percentages[7 - i] * inheritance;
            }
        }
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    inheritance = rand() % 500000 + 500000;

    cout << "Don left an inheritance of " << inheritance << "$" << endl;

    generatePercentage();
    cout << "Don's will contained the following percentage distributions of inheritance among heirs:" << endl;
    for (int i = 0; i < 7; i++) {
        cout << percentages[i] * 100 << "%, ";
    }
    cout << percentages[7] * 100 << "%" << endl << endl;

    generateLawyerCalculatedInheritance();
    cout << "According to the lawyer's calculations, heirs should receive:" << endl;
    for (int i = 0; i < 7; i++) {
        cout << lawyerCalcInheritance[i] << "$, ";
    }
    cout << lawyerCalcInheritance[7] << "$" << endl << endl;

    //Подсчет суммы, которую наследники получили на руки, и её проверка
#pragma omp parallel default(none)
    {
#pragma omp single
        funcTree(0, 0, 7);
    }
}
