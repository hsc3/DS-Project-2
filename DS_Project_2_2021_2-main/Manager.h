#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "VaccinationData.h"
#include "AVLTree.h"
#include "BpTree.h"
#include <algorithm>
using namespace std;

class Manager{
private:
	AVLTree		*avl;		// AVL tree
	BpTree		*bp;		// B+ tree
	vector<VaccinationData*> Print_vector;
	ifstream fin;
	ofstream fout;
public:
	Manager(int bpOrder);	// b+ node map size == bpOrder
	~Manager();				// memory free only on newly allocated avl and bp

	void	run(const char* command_txt);
	bool	LOAD();
	bool	VLOAD();
	bool	ADD(char* UserName, char* VaccineName, int age, char* LocationName);
	bool	SEARCH_BP(string name);
	bool	SEARCH_BP(string start, string end);
	bool	SEARCH_AVL(string name);
	bool	VPRINT(string type_);
	bool	PRINT_BP();
        void    EXIT();
	void	printSuccess(const char* act);
	void	printErrorCode(int n);
};

