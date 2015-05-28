#include "Vector.h"
#include <iostream>
#include <string>
using namespace std;

template<typename T>
void printVector(const Vector<T>& vec)
{
    std::cout << "===== VECTOR STATUS =====\n";
    std::cout << "capacity = " << vec.capacity() << ", size = " << vec.size()
        << ", isEmpty = " << vec.empty() << "\n";
    for (int i = 0; i < vec.size(); ++i)
    {
        std::cout << vec[i] << "\n";
    }
    std::cout << "\n";
}

class Student
{
public:
    Student() : m_name("Georgi"), m_age(19) {}
    Student(string name, int age) : m_name(name), m_age(age) {}

	friend std::ostream& operator<<(std::ostream& out, const Student& student);
private:
    string m_name;
    int m_age;
};

std::ostream& operator<<(std::ostream& out, const Student& student)
{
	out << "{m_name: " << student.m_name << ", m_age: " << student.m_age << "}";
	return out;
}

int main()
{
    Student st1("Ivan", 20);
    Student st2("Petur", 20);

    Vector<Student> students;
    students.push_back(st1);
    students.push_back(st2);

    Vector<int> v1;
    v1.push_back(1);
    v1.push_back(13);
    v1.push_back(37);
    v1.push_back(73);
    v1.push_back(121);
    v1.push_back(181);
    v1.push_back(253);

    cout << v1.back() << endl; //253
    v1.pop_back();
    cout << v1.back() << endl; //181
    cout << v1[4] << endl; //121
    v1.push_back(777);

    //Извежда 1 13 37 73 121 181 777
    for (int i = 0; i < v1.size(); i++)
    {
        cout << v1[i] << " ";
    }
    cout << endl;

    Vector<string> v2;
    v2.push_back("Aut viam");
    v2.push_back("inveniam");
    v2.push_back("aut faciam");

    //Извежда Aut viam inveniam aut faciam
    for (int i = 0; i < v2.size(); i++)
    {
        cout << v2[i] << " ";
    }
    cout << endl;

    Vector<string> v3;
    v3.push_back("Aut");
    v3.push_back("vincere");
    v3.push_back("mori");
    v3.insert(2, "aut");

    //Извежда Aut vincere aut mori
    for (int i = 0; i < v3.size(); i++)
    {
        cout << v3[i] << " ";
    }
    cout << endl;
}
