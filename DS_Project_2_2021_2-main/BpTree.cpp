#include "BpTree.h"
bool BpTree::Insert(VaccinationData* newData)
{
	// 1. Insert the newData into B+ tree
	// When the B+ Tree is empty
	if (this->root == NULL) {
		BpTreeNode* new_root = new BpTreeDataNode;
		new_root->insertDataMap(newData->GetUserName(), newData);
		this->root = new_root;
	}
	// When the B+ tree is not empty
	else {
		// Error check 1. Already exist data
		if (this->searchDataNode(newData->GetUserName()) != NULL) return false; 
		else {
			// Go to the data node
			BpTreeNode* cursor = this->root;
			while (cursor->getMostLeftChild() != NULL) {
				cursor = cursor->getMostLeftChild();
			}
			for (; cursor->getNext() != NULL; cursor = cursor->getNext()) {
				auto itr = cursor->getDataMap()->end(); itr--;
				if (newData->GetUserName() < itr->first)
					break;
			}
			// Now, cursor is the data node to insert newData
			cursor->insertDataMap(newData->GetUserName(), newData);
			// ------ Finish inserting the newData ------	
			// 2. Check the tree, and split or not from leaf to root
			// If the cursor is the root of the tree
			if (cursor == this->root) {
				if (this->exceedDataNode(cursor)) this->splitDataNode(cursor);
			}
			// If the cursor is not the root of tree
			else {
				// Split at the data node
				if (this->exceedDataNode(cursor)) this->splitDataNode(cursor);
				// Go to parent, and Splitting the node while access at the root of the tree
				for (cursor = cursor->getParent(); cursor != NULL; cursor = cursor->getParent()) {
					if (this->exceedIndexNode(cursor)) this->splitIndexNode(cursor);
				}
			} // ------ Finish splitting the nodes ------
		}
	} 
	return true;
}

BpTreeNode * BpTree::searchDataNode(string n) 
{
	if (this->root == NULL) return NULL;
	BpTreeNode* cursor = this->root;
	
	// 1. Go to the data node
	while (cursor->getMostLeftChild() != NULL) {  
		cursor = cursor->getMostLeftChild(); 
	} 

	// 2. Searching "n" in the data node's keys
	// Case 1. When "n" is just one alphabet
	if (n.size() == 1) {
		for (; cursor != NULL; cursor = cursor->getNext()) {
			for (auto itr = cursor->getDataMap()->begin(); itr != cursor->getDataMap()->end(); itr++) {
				if (itr->first.at(0) == n.at(0)) return cursor;
			}
		}
	}
	// Case 2. When "n" is the full name
	else {
		for (; cursor != NULL; cursor = cursor->getNext()) {
			if (cursor->getDataMap()->find(n) != cursor->getDataMap()->end()) {
					return cursor; // Found 
			}
		}
	}
	return NULL; // Not Found
}
void BpTree::splitDataNode(BpTreeNode* pDataNode) 
{ 
	// 1. Make a new right data node
	BpTreeNode* rightDataNode = new BpTreeDataNode;
	auto itr = pDataNode->getDataMap()->end(); itr--;
	rightDataNode->insertDataMap(itr->first, itr->second);
	pDataNode->deleteMap(itr->first);
	itr = pDataNode->getDataMap()->end(); itr--;
	rightDataNode->insertDataMap(itr->first, itr->second);
	pDataNode->deleteMap(itr->first);
	
	// 2. Make a new parent node
	// 2.1. If the pDataNode is the root 
	if (pDataNode->getParent() == NULL) {
		BpTreeNode* newIndexNode = new BpTreeIndexNode;
		newIndexNode->insertIndexMap(rightDataNode->getDataMap()->begin()->first, rightDataNode); // set right child
		newIndexNode->setMostLeftChild(pDataNode); // set most left child
		rightDataNode->setParent(newIndexNode); // set parent of right child
		pDataNode->setParent(newIndexNode); // set parent of most left child 
		this->root = newIndexNode; // save to the root
	}
	// 2.2. If the pDataNode is not the root
	else {
		pDataNode->getParent()->insertIndexMap(rightDataNode->getDataMap()->begin()->first, rightDataNode); // set right child
		rightDataNode->setParent(pDataNode->getParent()); // set parent of right child
	}
	
	// 3. Connect doubly linked list between data nodes
	if (pDataNode->getNext() != NULL) {
		rightDataNode->setNext(pDataNode->getNext());
		pDataNode->getNext()->setPrev(rightDataNode);
	}
	pDataNode->setNext(rightDataNode);
	rightDataNode->setPrev(pDataNode);
}

void BpTree::splitIndexNode(BpTreeNode* pIndexNode) 
{
	// 1. Make a new right Index node
	BpTreeNode* rightIndexNode = new BpTreeIndexNode;
	auto itr = pIndexNode->getIndexMap()->end(); itr--;
	rightIndexNode->insertIndexMap(itr->first, itr->second);
	pIndexNode->deleteMap(itr->first);
	itr = pIndexNode->getIndexMap()->end(); itr--;
	pIndexNode->deleteMap(itr->first);

	// 2.Make a new parent node
	// 2.1. If the pIndexNode is the root
	if (pIndexNode->getParent() == NULL) {
		BpTreeNode* newRootNode = new BpTreeIndexNode;
		newRootNode->insertIndexMap(rightIndexNode->getIndexMap()->begin()->first, rightIndexNode); // set right child
		newRootNode->setMostLeftChild(pIndexNode); // set most left child
		rightIndexNode->setParent(newRootNode); // set parent of right child
		pIndexNode->setParent(newRootNode); // set parent of most left child 
		this->root = newRootNode; // save to the root
	}
	// 2.2. If the pIndexNode is not the root
	else {
		pIndexNode->getParent()->insertIndexMap(rightIndexNode->getIndexMap()->begin()->first, rightIndexNode); // set right child
		rightIndexNode->setParent(pIndexNode->getParent()); // set parent of right child
	}
}

bool BpTree::exceedDataNode(BpTreeNode* pDataNode) 
{
	// When the pDataNode is root, have maximum of 2 childs
	if (pDataNode == this->root) {  
		if (pDataNode->getDataMap()->size() > this->order-1) {
			return true;
		}
	}
	// When the pDataNode is not root, have maximum of 3 childs
	else {
		if (pDataNode->getDataMap()->size() > this->order) {
			return true;
		}
	}
	return false; // Not exceed
}

bool BpTree::exceedIndexNode(BpTreeNode* pIndexNode) 
{
	// When the pIndexNode is root, have maximum of 2 childs
	if (pIndexNode == this->root) {
		if (pIndexNode->getIndexMap()->size() > this->order-1) {
			return true;
		}
	}
	// When the pIndexNode is not root, have maximum of 3 childs
	else {
		if (pIndexNode->getIndexMap()->size() > this->order) {
			return true;
		}
	}
	return false; // Not exceed
}

void BpTree::SearchRange(string start, string end) 
{
	BpTreeNode* cursor = this->root;
	ofstream fout;
	fout.open("log.txt", ios::app);

	// Go to the data node for searching
	while (cursor->getMostLeftChild() != NULL) {
		cursor = cursor->getMostLeftChild();
	} // cursor => most left data node

	fout << "====== SEARCH_BP ======" << endl;
	// Search type 1. Exact match
	if (end == " ") {
		// Searching "start" in the data node's keys
		for (; cursor != NULL; cursor = cursor->getNext()) {
			for (auto i = cursor->getDataMap()->begin(); i != cursor->getDataMap()->end(); i++) {
				if (i->first == start) {
					fout << i->second->GetUserName() << " ";
					fout << i->second->GetVaccineName() << " ";
					fout << i->second->GetTimes() << " ";
					fout << i->second->GetAge() << " ";
					fout << i->second->GetLocationName() << endl;
					break;
				}
			}
		}
	}
	// Search type 2. Range search
	else {
		for (; cursor != NULL; cursor = cursor->getNext()) {
			for (auto i = cursor->getDataMap()->begin(); i != cursor->getDataMap()->end(); i++) {
				if (i->first.at(0) >= start.at(0) && i->first.at(0) <= end.at(0)) {
					fout << i->second->GetUserName() << " ";
					fout << i->second->GetVaccineName() << " ";
					fout << i->second->GetTimes() << " ";
					fout << i->second->GetAge() << " ";
					fout << i->second->GetLocationName() << endl;
				}
			}
		}
	}
	fout << "=======================" << endl << endl;
	fout.close();
}

void BpTree::Print() 
{
	// Print all data
	BpTreeNode* print_cursor = this->root;
	while (print_cursor->getMostLeftChild() != NULL) {
		print_cursor = print_cursor->getMostLeftChild();
	} // p is data node
	ofstream fout;
	fout.open("log.txt", ios::app);
	fout << "======== PRINT_BP =======" << endl;
	for (; print_cursor != NULL; print_cursor = print_cursor->getNext()) {
		for (auto itr = print_cursor->getDataMap()->begin(); itr != print_cursor->getDataMap()->end(); itr++)
		{
			fout << itr->second->GetUserName() << " ";
			fout << itr->second->GetVaccineName() << " ";
			fout << itr->second->GetTimes() << " ";
			fout << itr->second->GetAge() << " ";
			fout << itr->second->GetLocationName() << endl;
		}
	}
	fout << "=========================" << endl << endl;
	fout.close();
}
