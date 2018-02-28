// Console_CallBack.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Console_CallBack.h"
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;


//*********Test Function Point*****************************************

int dosum1(int a, int b){
	return (a + b);
}

int dosum2(int a, int b){
	return (a * b);
}

typedef int(*pSum)(int a, int b);

void testFunctionPoint()
{
	pSum pS;
	pS = dosum1;
	printf("testFunctionPoint ps1, %d\r\n", pS(5, 5));

	pS = dosum2;
	printf("testFunctionPoint ps2, %d\r\n", pS(5, 5));

}
//**************************************************


//**************************************************
//Test For Each
void printElem(int& elem) {
	cout << elem << endl;
}

void testForEach()
{
	int ia[] = { 1, 2, 3 };
	vector<int> ivec(ia, ia + sizeof(ia) / sizeof(int));

	for_each(ivec.begin(), ivec.end(), printElem);
}
//**************************************************


//**************************************************
typedef int (WINAPI *PFCALLBACK)(int Param1, int Param2);
int WINAPI CBFunc(int Param1, int Param2)
{
	return 0;
}


class A
{
public:

	static void callback()  //类的成员函数作为回调函数 static方式实现
	{
		cout << "回调函数开始执行了！" << endl;
	}
};

void f(void(*p)())
{
	cout << "now in f" << endl;
	p();
	cout << "f end" << endl;
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf(_T("Fatal Error: MFC initialization failed\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: GetModuleHandle failed\n"));
		nRetCode = 1;
	}


	//testFunctionPoint(); // ok
	//testForEach(); //ok
	void(*p1)();
	p1 = A::callback;
	f(p1);
	//return 0;

	system("pause");
	return nRetCode;
}

