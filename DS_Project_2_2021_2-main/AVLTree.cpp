#include "AVLTree.h"
bool AVLTree::Insert(VaccinationData* pVac)
{
	// When AVL Tree is empty
	if (this->root == NULL) {
		AVLNode* new_root = new AVLNode;
		new_root->setVacData(pVac);
		this->root = new_root;
		return true;
	}
	// When AVL Tree is not empty
	// 1. Locate insertion point
	AVLNode* a = this->root,     // most recent node with BF is +1 or -1 
		   * pa = NULL,      // parent of a
		   * p = this->root, // p moves through the tree
		   * pp = NULL,      // parent of p
		   * rootSub = NULL; // root of subtree

	while (p != NULL) {
		if (p->getBF() != 0) { // p is candidate of a
			a = p; 
			pa = pp;
		}
		if (pVac->GetUserName() < p->getVacData()->GetUserName()) {
			pp = p;
			p = p->getLeft(); // take left branch
		}
		else if (pVac->GetUserName() > p->getVacData()->GetUserName()) {
			pp = p;
			p = p->getRight(); // take right branch
		}
		else return false; // already in AVL Tree			
	}

	// 2. Insert and set BF of nodes
	AVLNode* y = new AVLNode;
	y->setVacData(pVac);
	if (pVac->GetUserName() < pp->getVacData()->GetUserName()) {
		pp->setLeft(y); // insert as left child
	}
	else pp->setRight(y); // insert as right child

	int d;
	AVLNode    * b, // child if a 
		   * c; // child of b
	
	if (pVac->GetUserName() > a->getVacData()->GetUserName()) {
		b = p = a->getRight();
		d = -1;
	}
	else {
		b = p = a->getLeft();
		d = 1;
	}
	while (p != y) {
		if (pVac->GetUserName() > p->getVacData()->GetUserName()) {
			p->setBF(-1); // height of right increases by 1
			p = p->getRight();
		}
		else {
			p->setBF(1); // height of left increase by 1
			p = p->getLeft();
		}
	}

	// 3. Rebalance the AVL Tree
	// 3-1. Tree is still balanced
	if (a->getBF() == 0 || a->getBF() + d == 0) {
		a->setBF(a->getBF() + d);
		return true;
	}
	// 3-2. Tree is unbalanced -> Determine rotation type
	if (d == 1) { // left imbalance
		if (b->getBF() == 1) { // Rotation type "LL"
			a->setLeft(b->getRight());
			b->setRight(a);
			a->setBF(0);
			b->setBF(0);
			rootSub = b; // b is new root of the subtree
		}
		else { // Rotation type "LR"
			c = b->getRight();
			b->setRight(c->getLeft());
			a->setLeft(c->getRight());
			c->setLeft(b);
			c->setRight(a);
			switch (c->getBF()) {
			case 0:
				b->setBF(0);
				a->setBF(0);
				break;
			case 1:
				a->setBF(-1);
				b->setBF(0);
				break;
			case 2:
				a->setBF(0);
				b->setBF(1);
				break;
			}
			c->setBF(0);
			rootSub = c; // c is new root of the subtree
		}
	} // end of left imbalance
	else { // right imbalance
		if(b->getBF() == -1) { // Rotation type "RR"
			a->setRight(b->getLeft());
			b->setLeft(a);
			a->setBF(0);
			b->setBF(0);
			rootSub = b; // b is new root of the subtree
		}
		else { // Rotation type "RL"
			c = b->getLeft();
			b->setLeft(c->getRight());
			a->setRight(c->getLeft());
			c->setRight(b);
			c->setLeft(a);
			switch (c->getBF()) {
			case 0:
				a->setBF(0);
				b->setBF(0);
				break;
			case 1:
				a->setBF(0);
				b->setBF(-1);
				break;
			case -1:
				a->setBF(1);
				b->setBF(0);
				break;
			}
			c->setBF(0);
			rootSub = c; // c is new root of the subtree
		} 
	} // end of right imbalance
	
	// 4. Update the subtree to AVL Tree
	if (pa == NULL) root = rootSub;
	else if (a == pa->getLeft())
		pa->setLeft(rootSub);
	else pa->setRight(rootSub);
	return true;
}

void AVLTree::GetVector(vector<VaccinationData*>& v)
{
	// If vector is not empty -> Clear and make new vector
	if (!v.empty()) v.clear();

	// Push all data in the AVL Tree into vector
	queue<AVLNode*> q[100];
	AVLNode* n = this->root;

	if (n == NULL) return; // If AVL Tree is empty, terminate the function 
	q->push(n); // Push a AVL Tree node into the Queue

	while (q->size() > 0) 
	{
		// Pop the first node in the queue and Push Vaccination Data of the node into the vector
		n = q->front();
		v.push_back(n->getVacData());
		q->pop();

		if (n != NULL)
		{
			// push the child nodes of 'n'
			if (n->getLeft() != NULL) q->push(n->getLeft());
			if (n->getRight() != NULL) q->push(n->getRight());
		}
		else break;
	}
}

VaccinationData* AVLTree::Search(string name)
{
	AVLNode* cursor = this->root;    

	while (cursor != NULL) {
		if (name < cursor->getVacData()->GetUserName()) {
			cursor = cursor->getLeft(); // take left branch
		}
		else if (name > cursor->getVacData()->GetUserName()) {
			cursor = cursor->getRight(); // take right branch
		}
		else return cursor->getVacData(); // Found the node in AVL Tree
	}
	if(cursor == NULL)return NULL; // Not Found
}

