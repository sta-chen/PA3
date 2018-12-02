
/**
 *
 * twoDtree (pa3)
 * slight modification of a Kd tree of dimension 2.
 * twoDtree.cpp
 * This file will be used for grading.
 *
 */

#include "twoDtree.h"

/* node constructor given */
twoDtree::Node::Node(pair<int,int> ul, pair<int,int> lr, HSLAPixel a)
	:upLeft(ul),lowRight(lr),avg(a),LT(NULL),RB(NULL)
	{}

/* destructor given */
twoDtree::~twoDtree(){
	clear();
}

/* copy constructor given */
twoDtree::twoDtree(const twoDtree & other) {
	copy(other);
}


/* operator= given */
twoDtree & twoDtree::operator=(const twoDtree & rhs){
	if (this != &rhs) {
		clear();
		copy(rhs);
	}
	return *this;
}

/* twoDtree constructor */
twoDtree::twoDtree(PNG & imIn){ 
/* your code here */
	height = imIn.height();
	width  = imIn.width();
	stats s(imIn);
	pair<int, int> ul = make_pair(0, 0);
	pair<int, int> lr = make_pair(width - 1, height - 1);
	root = buildTree(s,ul,lr, true);
}

/* buildTree helper for twoDtree constructor */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {

/* your code here */
	HSLAPixel avg = s.getAvg(ul, lr);

	root = new Node(ul, lr, avg);

	buildTree(root, s, ul, lr, vert);

	return root;

}

void twoDtree::buildTree(Node* node, stats& s, pair<int,int> ul, pair<int,int> lr, bool vert) {
	if (ul.first == lr.first && ul.second == lr.second) {
		return;
	} else {
		pair<pair<int,int>, pair<int,int>> corners = findBestSplit(s, ul, lr, vert);

		node->LT = new Node(ul, corners.first, s.getAvg(ul, corners.first));
		node->RB = new Node(corners.second, lr, s.getAvg(corners.second, lr));

		buildTree(node->LT, s, ul, corners.first, !vert);
		buildTree(node->RB, s, corners.second, lr, !vert);

	}
}

pair<pair<int,int>, pair<int,int>> twoDtree::findBestSplit(stats& s, pair<int,int> ul, pair<int,int> lr, bool vert) {

	int ui = ul.first;
	int uj = ul.second;
	int li = lr.first;
	int lj = lr.second;

	double currentLowestEntropy = LONG_MAX;
	double currentEntropy;
	int smallestBottomArea = INT_MAX;
	pair<int,int> lrCorner;
	pair<int,int> ulCorner;
	double totalArea = s.rectArea(pair<int,int> (ui,uj), pair<int,int> (li,lj));

	if (ui == li) {
		vert = false;
	}

	if (uj == lj) {
		vert = true;
	}

	if (vert) {
		for (int i = 0; i < li - ui; i++) {
			int topArea = s.rectArea(pair<int,int> (ui,uj), pair<int,int> (ui + i, lj));
			int bottowmArea = s.rectArea(pair<int,int> (ui + 1 + i, uj), pair<int,int> (li,lj));

			currentEntropy = (topArea / totalArea) * s.entropy(pair<int,int> (ui, uj), pair<int,int> (ui + i, lj)) +
							(bottowmArea / totalArea) * s.entropy(pair<int,int> (ui + 1 + i, uj), pair<int,int> (li,lj));

			if (currentEntropy < currentLowestEntropy) {
				currentLowestEntropy = currentEntropy;
				lrCorner = pair<int,int> (ui + i, uj);
				ulCorner = pair<int,int> (ui + 1 + i, uj);
			}
			else if (currentEntropy == currentLowestEntropy && bottowmArea < smallestBottomArea) {

				currentLowestEntropy = currentEntropy;
				lrCorner = pair<int,int> (ui + i, uj);
				ulCorner = pair<int,int> (ui + 1 + i, uj);
				smallestBottomArea = bottowmArea;
			}
		}

		return pair<pair<int,int>, pair<int,int>> (lrCorner, ulCorner);
	} else {
		for (int j = 0; j < lj - uj; j++) {
			int topArea = s.rectArea(pair<int,int> (ui, uj), pair<int,int> (li, uj + j));
			int bottowmArea = s.rectArea(pair<int,int> (ui, uj + 1 + j), pair<int,int> (li,lj));

			currentEntropy = (topArea / totalArea) * s.entropy(pair<int,int> (ui, uj), pair<int,int> (li, uj + j)) +
							(bottowmArea / totalArea) * s.entropy(pair<int,int> (ui, uj + 1 + j), pair<int,int> (li,lj));
			if (currentEntropy < currentLowestEntropy) {
				currentLowestEntropy = currentEntropy;
				lrCorner = pair<int,int> (li,uj + j);
				ulCorner = pair<int,int> (ui, uj + 1 + j);
			}
			else if (currentEntropy == currentLowestEntropy && bottowmArea < smallestBottomArea) {
				currentLowestEntropy = currentEntropy;
				lrCorner = pair<int,int> (li,uj + j);
				ulCorner = pair<int,int> (ui, uj + 1 + j);
				smallestBottomArea = bottowmArea;
			}
		}

		return pair<pair<int,int>, pair<int,int>> (lrCorner, ulCorner);
	}
}

/* render your twoDtree into a png */
PNG twoDtree::render(){
/* your code here */

	if (root == NULL) {
		return *(new PNG());
	}

	PNG image(width, height);

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			*(image.getPixel(i,j)) = getPixelFromTree(i,j);
		}
	}

	return image;
}

HSLAPixel twoDtree::getPixelFromTree(int x, int y) const {
	return getPixelFromTree(root, x, y);
}

HSLAPixel twoDtree::getPixelFromTree(Node* node, int x, int y) const {
	if (node->LT == NULL && node->RB == NULL) {
		return node->avg;
	}

	else if (x <= node->LT->lowRight.first && y <= node->LT->lowRight.second && node->LT->upLeft.first <= x && node->LT->upLeft.second <= y) {
		return getPixelFromTree(node->LT, x, y);
	} else {
		return getPixelFromTree(node->RB, x, y);
	}
}


/* prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of 
 * the average pixel value contained in the root
 * of the subtree
 */
void twoDtree::prune(double tol){

/* your code here */
	prune(root, tol);

}

void twoDtree::prune(Node* node, double tol) {
	if (node->LT == NULL) {
		return;
	}

	if (isPrunable(node, node, tol)) {
		delete node->LT;
		delete node->RB;

		node->LT = NULL;
		node->RB = NULL;

		return;
	} else {
		prune(node->LT, tol);
		prune(node->RB, tol);
	}
}

bool twoDtree::isPrunable(Node* node1, Node* node2, double tol) const {
	if (node1->LT == NULL) {
		if (node1->avg.dist(node2->avg) <= tol) {
			return true;
		}

		return false;
	} else {
		return isPrunable(node1->LT, node2, tol) && isPrunable(node1->RB, node2, tol);
	}
}



/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
 /* your code here */
	delete root;
	clear(root);
}

void twoDtree::clear(Node* node) {
	if (node != NULL) {
		Node* temp = node;
		clear(node->LT);
		clear(node->RB);

		delete temp;
	}
}


/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree & orig){

/* your code here */
	root = new Node(orig.root->upLeft, orig.root->lowRight, orig.root->avg);
	height = orig.height;
	width = orig.width;
	copy(root, orig.root);

}

void twoDtree::copy(Node* dest, Node* other) {
	if (other != NULL) {
		if (other->LT != NULL) {
			dest->LT = new Node(other->LT->upLeft, other->LT->lowRight, other->LT->avg);
			dest->RB = new Node(other->RB->upLeft, other->RB->lowRight, other->RB->avg);
			copy(dest->LT, other->LT);
			copy(dest->RB, other->RB);
		}
	}
}



