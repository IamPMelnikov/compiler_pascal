#include <iostream>
#include<iomanip>
#include<string>
#include <fstream>
#include <memory>
#include <vector>
#include "ToLowerStr.h"
#include "CErrorPrinter.h"
#include "CCompiler.h"

using namespace std;

bool readFile(string path, string& text) {
	text = "";
	string line;
	ifstream in(path);

	if (!in.is_open())
		return false;
	//для всех строк преобразовали текст 
	while (getline(in, line)) {
		text += line + "\n";
	}
	in.close();
	return text == "";
}

int main()
{
	setlocale(LC_ALL, "Russian");
	// запускаем чтение	
	string text;
	if (readFile("./resources/input.txt", text)) {
		printf("Файл оказался пустым");
		return 0;
	} 
	//cout << text << endl; 
	//создаем нужные классы и закидываем в них нужные данные
	unique_ptr<CErrorManager> eManager = make_unique<CErrorManager>("./resources/errors.txt");//создаем умный указатель на объект CErrorManager и выгружаем данные из файла с кодами ошибок.
	unique_ptr<CErrorPrinter> printer = make_unique<CErrorPrinter>("./resources/OutErr.txt"); //создаем умный указатель на объект CErrorPrinter с файлом итоговых ошибок
	unique_ptr<CLexic> lexic = make_unique<CLexic>(eManager.get(), &text);//создание и закидка в объект лексера  текста программы и менеджера ошибок
	unique_ptr<CSemantic> semantic = make_unique<CSemantic>(eManager.get(), lexic.get()); //создание для семантического анализатора фиктивной нулевой области дейсвия и созадем указалетль на обзект сем.анализатора
	unique_ptr<CCompiler> syntax = make_unique<CCompiler>(eManager.get(), lexic.get(), semantic.get());

	syntax->startVer(); //запуск анализатора

	eManager->printErrors(printer.get());//печатаем ошибки
}

