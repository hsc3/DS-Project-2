#include "Manager.h"
Manager::Manager(int bpOrder) 
{
    this->bp = NULL;
    this->avl = new AVLTree;
}

Manager::~Manager() 
{
}

void Manager::run(const char* command_txt) 
{
    fin.open(command_txt); // Open command file 
    
    if (!fin) // When fail to open command file
    {
        fout.open("log.txt", ios::app); 
        fout << "File Open Error" << endl;
        return;
    }

    char cmd[100];

    while (!fin.eof())
    {
        // Get a line of command file data and Tokenize the command data
        fin.getline(cmd, 100); 
        char* p[5] = { "\0", };
        int i = 0;
        char* ptr = strtok(cmd, " ");
        while (ptr != NULL)
        {
            p[i] = ptr;
            i++;
            ptr = strtok(NULL, " ");
        }

        // Compare command string and Execute that function 
        if (strcmp(p[0], "LOAD") == 0)
        {
            if (!LOAD())
            {
                printErrorCode(100); 
            }
            else printSuccess("LOAD");
        }
        else if (strcmp(p[0], "VLOAD") == 0)
        {
            if (!VLOAD())
            {
                printErrorCode(200);
            }
            else printSuccess("VLOAD");
        }
        else if (strcmp(p[0], "ADD") == 0)
        {
            if (!ADD(p[1], p[2], atoi(p[3]), p[4]))
            {
                printErrorCode(300);
            }
        }
        else if (strcmp(p[0], "SEARCH_BP") == 0)
        {
            if (p[2])
            {
                if (!SEARCH_BP(p[1], p[2]))
                {
                    printErrorCode(400);
                }
            }
            else
            {
                if (!SEARCH_BP(p[1]))
                {
                    printErrorCode(400);
                }
            }
        }
        else if (strcmp(p[0], "SEARCH_AVL") == 0)
        {
            if (!SEARCH_AVL(p[1]))
            {
                printErrorCode(500);
            }
        }
        else if (strcmp(p[0], "VPRINT") == 0) 
        {
            if (!VPRINT(p[1]))
            {
                printErrorCode(600);
            }
        }
        else if (strcmp(p[0], "PRINT_BP") == 0)
        {
            if (!PRINT_BP())
            {
                printErrorCode(700);
            }
        }
        else if (strcmp(p[0], "EXIT") == 0)
        {
            EXIT(); 
            printSuccess("EXIT");
        }
        else
        {
            if(strcmp(p[0], "\0") == 0) continue;
            printErrorCode(800);
        }
    }
    fin.close();
    fout.close();
    return;
}

bool Manager::LOAD() 
{
    ifstream fin_data;
    fin_data.open("input_data.txt");
    // Error 1 and 2: Input data file is not exist or B+ Tree is already exist
    if (!fin_data || this->bp != NULL) return false;
    // Make a new B+ Tree
    this->bp = new BpTree;
    // Read the Input data file
    while (!fin_data.eof())
    {
        char load_data[100];
        fin_data.getline(load_data, 100);

        // Tokenize the input data
        char* information[5] = { "\0", };
        int i = 0;
        char* ptr = strtok(load_data, " ");
        while (ptr != NULL)
        {
            information[i] = ptr;
            i++;
            ptr = strtok(NULL, " ");
        }
        // Error 3: Input data is not fully valid. Pass to the next line of data
        if (!information[0] || !information[1] || information[2] == NULL || information[3] == NULL || !information[4]) continue;
        // Save the information in the newly allocated address space
        char* user_name     = new char[20];
        char* vaccine_name  = new char[20];
        char* location_name = new char[20];
        strcpy(user_name,     information[0]);
        strcpy(vaccine_name,  information[1]);
        strcpy(location_name, information[4]);  
        
        // Make a VaccionationData and Insert to B+ Tree
        VaccinationData* v = new VaccinationData;
        v->SetUserName(user_name);
        v->SetVaccineName(vaccine_name);
        v->SetTimes(atoi(information[2]));
        v->SetAge(atoi(information[3]));
        v->SetLocationName(location_name);
        this->bp->Insert(v);
    }

    fin_data.close();
    return true;   
}

bool Manager::VLOAD() {
    // Load all data in AVL tree to Print_vector
	this->avl->GetVector(Print_vector);
    // Error: There is no data in AVL Tree
    if (Print_vector.size() == 0) return false; 
    return true;
}

bool Manager::ADD(char* UserName, char* VaccineName, int age, char* LocationName)
{
    // Error 1: Input data is not fully valid
    if (!UserName || !age || !VaccineName || !LocationName )
        return false;
    
    // Case 1: Already existed user name
    BpTreeNode* userName_node = this->bp->searchDataNode(UserName);
    if (userName_node != NULL) {
        map<string, VaccinationData*>::iterator itr = userName_node->getDataMap()->find(UserName);
        // Error 2: If the user is in the AVL Tree (count is already full)
        if (this->avl->Search(UserName) != NULL)
            return false;
        // Increase the count +1
        itr->second->SetTimes(itr->second->GetTimes() + 1); 
        // Now, user's vaccine count is full, insert to AVL Tree
        this->avl->Insert(itr->second); 
    }
    // Case 2: Not existed. Make a new vaccination data and Insert to B+ Tree
    else {
        char* new_userName = new char[20];
        char* new_vaccineName = new char[20];
        char* new_locationName = new char[20];
        strcpy(new_userName, UserName);
        strcpy(new_vaccineName, VaccineName);
        strcpy(new_locationName, LocationName);

        VaccinationData* v = new VaccinationData;
        v->SetUserName(new_userName);
        v->SetVaccineName(new_vaccineName);
        v->SetTimes(0);
        v->SetAge(age);
        v->SetLocationName(new_locationName);
        this->bp->Insert(v);
    }
    fout.open("log.txt", ios::app);
    fout << "====== ADD ======" << endl;
    fout << UserName << " " << VaccineName << " " << age << " " << LocationName << endl;
    fout << "=================" << endl << endl;
    fout.close();
    return true;
}

bool Manager::SEARCH_BP(string name) 
{
    // Error check 
    if (!this->bp || !this->bp->searchDataNode(name) || name.empty()) return false;
    this->bp->SearchRange(name, " "); // Exact match search
    return true;
}

bool Manager::SEARCH_BP(string start, string end) 
{
    // Error check 1. Bp Tree is empty or input data is not fully valid  
    if (!this->bp || start.empty() || end.empty()) return false;
    // Error check 2. There is no data in range [start, end]
    // Comparing with the first alphabet of user name
    int start_ascii = int(start.at(0));
    int end_ascii = int(end.at(0));
    int i;
    for (i = start_ascii; i <= end_ascii; i++) {
        string s; 
        s += char(i);
        if (this->bp->searchDataNode(s) != NULL) break;
    }
    if (i > end_ascii) return false; 
    this->bp->SearchRange(start, end); // Range search
    return true;
}

bool Manager::SEARCH_AVL(string name) {
    // Error 1. input data is not valid
    if (name.empty()) return false;
    VaccinationData* v = this->avl->Search(name);
    // Error 2. there is no searched data
    if (v == NULL) return false;
    fout.open("log.txt", ios::app);
    fout << "====== SEARCH_AVL ======" << endl;
    fout << v->GetUserName() << " ";
    fout << v->GetVaccineName() << " ";
    fout << v->GetTimes() << " ";
    fout << v->GetAge() << " ";
    fout << v->GetLocationName() << endl;
    fout << "========================" << endl << endl;
    fout.close();
    return true;
}

bool Manager::PRINT_BP() 
{
    if (!this->bp) return false;
    else {
        bp->Print();
        return true;
    }
}

bool Compare_A(VaccinationData* vac1, VaccinationData* vac2) {
    // Sort Type A: VaccineName (Ascending) -> Age (Ascending) -> UserName (Ascending)
    if (vac1->GetVaccineName() == vac2->GetVaccineName()) {
        if (vac1->GetAge() == vac2->GetAge())
            return vac1->GetUserName() < vac2->GetUserName();
        return vac1->GetAge() < vac2->GetAge();
    }
    return vac1->GetVaccineName() < vac2->GetVaccineName();
}

bool Compare_B(VaccinationData* vac1, VaccinationData* vac2) {
    // Sort Type B: LocationName (Ascending) -> Age (Descending) -> UserName (Ascending)
    if (vac1->GetLocationName() == vac2->GetLocationName()) {
        if (vac1->GetAge() == vac2->GetAge())
            return vac1->GetUserName() < vac2->GetUserName();
        return vac1->GetAge() > vac2->GetAge();
    }
    return vac1->GetLocationName() < vac2->GetLocationName();
}

bool Manager::VPRINT(string type_) {
    // Error 1. AVL Tree is empty
    if (Print_vector.empty()) return false;
    if (type_ == "A") sort(Print_vector.begin(), Print_vector.end(), Compare_A);
    else if (type_ == "B") sort(Print_vector.begin(), Print_vector.end(), Compare_B);
    // Error 2. Type option is not valid
    else return false; 

    fout.open("log.txt", ios::app);
    fout << "======= " << "VPRINT " << type_ << " =======" << endl;
    for (int i = 0; i < Print_vector.size(); i++) {
        fout << Print_vector[i]->GetUserName() << " ";
        fout << Print_vector[i]->GetVaccineName() << " ";
        fout << Print_vector[i]->GetTimes() << " ";
        fout << Print_vector[i]->GetAge() << " ";
        fout << Print_vector[i]->GetLocationName() << endl;
    }
    fout << "==========================" << endl << endl;
    fout.close();
    return true;
}

void Manager::EXIT() 
{
    delete this->bp;
    delete this->avl;
}

void Manager::printSuccess(const char* act)
{
    fout.open("log.txt", ofstream::app);
    fout << "======= " << act << " =======" << endl;
    fout << "Success" << endl;
    fout << "===================" << endl << endl;
    fout.close();
}

void Manager::printErrorCode(int n) 
{
	fout.open("log.txt", ofstream::app);
	fout << "====== ERROR ======" <<endl;
	fout << n << endl;
	fout << "===================" << endl << endl;
	fout.close();
}
