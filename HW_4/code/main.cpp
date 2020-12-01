/*
	������� ������ ����������, ���191
	������� - 18
	������� ������:
	������ � ����������. � ������� ���� ������ ���� ��� ����, �
	������� �� ������� � ��� �� ��� ����, ������ �� ������� ���� ��� �� ���
	����. ������, ��� ������ ������� ��� ���� ��������� ��������� � ������
	�����. ������� ��� ���� �������� ������ ������ ���������� � ���� �����
	������������. �������� ����������� �������� � �������������� �����
	����������. ��������� ������� ������������� ����������, ������� ���
	��������� ����� ��������� ���� ������ � ���� ������� ����������,
	��������� ��������� ��������. ��� ������� ������������ �������
	���������
*/
#include <iostream>
#include <time.h>

using namespace std;

double *percentages;
double *lawyerCalcInheritance;
int inheritance;

//����������� ������� ��� ������������ ����� ����� ��� �������� ����� ���������� ������������ �� ����
void funcTree(int depth, int leftInt, int rightInt) {
    //depth = 3, �� ����� �� 8 ���������, ������������� ����� ����� ������� ������
    if (depth == 3) {
        //������������ ������� �����
        double result = percentages[leftInt] *
                        inheritance; //�� ������ ������� ����� ���������� �����, ������� ����� ���� �� ������� ��������
#pragma omp critical
        {
            if (result - lawyerCalcInheritance[leftInt] > 0.001) {
                cout << "The lawyer deceived the " << leftInt + 1 << " heir for "
                     << result - lawyerCalcInheritance[leftInt] << "$" << endl;
            } else if (lawyerCalcInheritance[leftInt] - result > 0.01) {
                cout << "The lawyer gave " << lawyerCalcInheritance[leftInt] - result << "$ more to " << leftInt + 1
                     << " heir" << endl;
            } else {
                cout << "The lawyer correctly counted the share of " << leftInt + 1 << " heir" << endl;
            }
        }
    } else {
        //�������� � ������� ����������, ������ ��� �������� �� �������
        int intLen = (rightInt - leftInt) / 2;
#pragma omp task shared(intLen, depth, leftInt) default(none)
        funcTree(depth + 1, leftInt, leftInt + intLen);
#pragma omp task shared(intLen, depth, rightInt) default(none)
        funcTree(depth + 1, rightInt - intLen, rightInt);
    }
}

/*
	��������� ���������� ����� ����������� �� ������ "���������" ����.
*/
void generatePercentage() {
    percentages = new double[8];

    double sum = 1;
    double tmp = (rand() % 20) / 100.0 + 0.05;
    percentages[0] = tmp;

    sum -= tmp;
    for (int i = 1; i < 7; i++) {
        tmp = (rand() % (int) (sum * 60)) / 100.0;
        sum -= tmp;
        percentages[i] = tmp;
    }
    percentages[7] = sum;
}

/*
    ��������� �����, ����������� ��������� (� ���� ������ �� ����� �������� �����������)
 */
void generateLawyerCalculatedInheritance() {

    lawyerCalcInheritance = new double[8];
    bool isLawyerLying = rand() % 3 == 0;
    if(!isLawyerLying){
        for (int i = 0; i < 8; i++) {
            lawyerCalcInheritance[i] = percentages[i] * inheritance;
        }
    } else {
        // ���� ������� ����� �������� �����������, �� ����� � ������������ 1/3 �������� � ��� 1/6 ����������,
        // ���� �� ����� ���������� �������� ����� ������������
        if (rand() % 2 == 0) {
            for (int i = 0; i < 8; i++) {
                if (rand() % 3 == 0) {
                    lawyerCalcInheritance[i] = (percentages[i] / 6.0) * inheritance;
                } else {
                    lawyerCalcInheritance[i] = percentages[i] * inheritance;
                }
            }
        } else {
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

    //������� �����, ������� ���������� �������� �� ����, � � ��������
#pragma omp parallel default(none)
    {
#pragma omp single nowait
        funcTree(0, 0, 7);
    }
}
