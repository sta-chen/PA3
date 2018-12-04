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
    stats s = stats(imIn);
    this->root = buildTree(s,make_pair<int,int>(0,0),make_pair<int,int>(imIn.width()-1,imIn.height()-1),1);
    this->height = imIn.height();
    this->width = imIn.width();
}

#ifdef MEMORIZE
static inline unsigned long long pair_hash(pair<int,int> x, pair<int,int> y) {
	return (x.first*23333333333) + (x.second*131071) + (y.first*23333) + (y.second*31);
}
#endif

/* buildTree helper for twoDtree constructor */
twoDtree::Node * twoDtree::buildTree(stats & s, pair<int,int> ul, pair<int,int> lr, bool vert) {
// #ifdef DEBUG_BUILDTREE
// 	cout << "ul: (" << ul.first << ", " << ul.second << ") ";
// 	cout << "lr: (" << lr.first << ", " << lr.second << ") " << endl;
// #endif
    HSLAPixel ag = s.getAvg(ul,lr);
    Node * curr;
	// experimental
#ifdef MEMORIZE
	unsigned long long hash = pair_hash(ul,lr);
	auto it = this->memorized.find(hash);
	if(it!=this->memorized.end()) {
		return it->second;
	} else {
		curr = new Node(ul,lr,ag);
		this->memorized[hash] = curr;
	}
#else
	curr = new Node(ul,lr,ag);
#endif

    if(ul.first!=lr.first || ul.second!=lr.second){
        if(ul.first==lr.first){
            int pos = splitHorizontally(s,ul,lr);
            int m = ul.second;
            int n = ul.first;
            int i = lr.second;
            int j = lr.first;
            pair<int,int> lul = {n,m};
            pair<int,int> llt = {n,pos};
            pair<int,int> rul = {j,pos+1};
            pair<int,int> rlt = {j,i};
            curr->LT = buildTree(s,lul,llt,0);
            curr->RB = buildTree(s,rul,rlt,0);
        }
        else if(ul.second==lr.second){
            int pos = splitVertically(s,ul,lr);
            int m = ul.second;
            int n = ul.first;
            int i = lr.second;
            int j = lr.first;
            pair<int,int> lul = {n,m};
            pair<int,int> llt = {pos,m};
            pair<int,int> rul = {pos+1,i};
            pair<int,int> rlt = {j,i};
            curr->LT = buildTree(s,lul,llt,1);
            curr->RB = buildTree(s,rul,rlt,1);
        }
        else if(vert){
            int pos = splitVertically(s,ul,lr);
            int m = ul.second;
            int n = ul.first;
            int i = lr.second;
            int j = lr.first;
            pair<int,int> lul = {n,m};
            pair<int,int> llt = {pos,i};
            pair<int,int> rul = {pos+1,m};
            pair<int,int> rlt = {j,i};
            curr->LT = buildTree(s,lul,llt,0);
            curr->RB = buildTree(s,rul,rlt,0);
        }
        else if(!vert){
            int pos = splitHorizontally(s,ul,lr);
            int m = ul.second;
            int n = ul.first;
            int i = lr.second;
            int j = lr.first;
            pair<int,int> lul = {n,m};
            pair<int,int> llt = {j,pos};
            pair<int,int> rul = {n,pos+1};
            pair<int,int> rlt = {j,i};
            curr->LT = buildTree(s,lul,llt,1);
            curr->RB = buildTree(s,rul,rlt,1);
        }
    }
    if (ul.first==lr.first && ul.second==lr.second){
        curr->LT = NULL;
        curr->RB = NULL;
    }
    return curr;
}

/** splitVertically helper for buildTree */
int twoDtree::splitVertically(stats & s,pair<int, int> ul, pair<int, int> lr) {
    int m = ul.second;
    int n = ul.first;
    int i = lr.second;
    int j = lr.first;
    pair<int,int> lul = {n,m};  //first split left rec :upper left
    pair<int,int> llr = {n,i};  // left rec: lower right
    double firstLE = s.entropy(lul,llr);
    pair<int,int> rul = {n+1,m}; //first split right rec :upper left
    pair<int,int> rlr = {j,i};  // right rec: lower right
    double firstRE = s.entropy(rul,rlr);
    double totalPixels = s.rectArea(ul,lr);
    // weighted entropy!!!!
    double min = (firstLE*(s.rectArea(lul,llr)) + firstRE*(s.rectArea(rul,rlr)))/totalPixels;
    int Position = n; 	//this position is on the left side of the split line!!!!!
    for (int k = n; k < j; k++) {  //starts at (m,n), end at (m,j-1)
        int currPosition = k;
        pair<int,int> lTop = {n,m};
        pair<int,int> lBot = {k,i};
        double leftE = s.entropy(lTop,lBot);
        pair<int,int> rTop = {k+1,m};
        pair<int,int> rBot = {j,i};
        double rightE = s.entropy(rTop,rBot);
        double currE = (leftE*(s.rectArea(lTop,lBot))+rightE*(s.rectArea(rTop,rBot)))/totalPixels;
        if (currE<=min) {
            if (currE == min) {
                int preMinRight = (i-m+1)*(j-(Position+1)+1);
                int currRight = (i-m+1)*(j-(currPosition+1)+1);
                if (currRight<preMinRight) {
                    Position = currPosition;
                    min = currE;
                }
            } else {
                Position = currPosition;
                min = currE;
            }
        }
    }
    return Position;
}

int twoDtree::splitHorizontally(stats &s, pair<int, int> ul, pair<int, int> lr) {
    int m = ul.second;
    int n = ul.first;
    int i = lr.second;
    int j = lr.first;
    pair<int,int> tul = {n,m};  //first split top rec :upper left
    pair<int,int> tlr = {j,m};  // top rec: lower right
    double firstLE = s.entropy(tul,tlr);
    pair<int,int> bul = {n,m+1}; //first split bottom rec :upper left
    pair<int,int> blr = {j,i};  // bottom rec: lower right
    double firstRE = s.entropy(bul,blr);
    double totalPixels = s.rectArea(ul,lr);
    // weighted entropy!!!!
    double min = (firstLE*(s.rectArea(tul,tlr)) + firstRE*(s.rectArea(bul,blr)))/totalPixels;
    int Position = m; 	//this position is  above  the split line!!!!!
    for (int k = m; k < i; k++) {  //starts at (m,n), end at (m,j-1)
        int currPosition = k;
        pair<int,int> tLeft = {n,m};
        pair<int,int> tRight = {j,k};
        double topE = s.entropy(tLeft,tRight);
        pair<int,int> bLeft = {n,k+1};
        pair<int,int> bRight = {j,i};
        double bottomE = s.entropy(bLeft,bRight);
        double currE = (topE*(s.rectArea(tLeft,tRight))+bottomE*(s.rectArea(bLeft,bRight)))/totalPixels;
        if (currE<=min) {
            if (currE == min) {
                int preMinBottom = (j-n+1)*(i-(Position+1)+1);
                int currBottom = (j-n+1)*(i-(currPosition+1)+1);
                if (currBottom<preMinBottom) {
                    Position = currPosition;
                    min = currE;
                }
            } else {
                Position = currPosition;
                min = currE;
            }
        }
    }
    return Position;
}


/* render your twoDtree into a png */
PNG twoDtree::render(){
    PNG image(this->width,this->height); 
    //render helper function
    render(this->root,image);
    return image;
}

void twoDtree::render(Node* root, PNG & image){
    if(root->LT == NULL && root->RB == NULL){
        pair<int,int> ul = root->upLeft;
        pair<int,int> lr = root->lowRight;
        for (int i=ul.first;i<=lr.first;i++){
            for(int j=ul.second;j<=lr.second;j++){
                HSLAPixel * currPix = image.getPixel(i,j);
                HSLAPixel avgPix = root->avg;
                currPix->a = avgPix.a;
                currPix->h = avgPix.h;
                currPix->l = avgPix.l;
                currPix->s = avgPix.s;
            }
        }
    } else {
        if(root->LT != NULL) render(root->LT,image);
        if(root->RB != NULL) render(root->RB,image);
    }
}

/* prune function modifies tree by cutting off
 * subtrees whose leaves are all within tol of
 * the average pixel value contained in the root
 * of the subtree
 */
void twoDtree::prune(double tol){
    prune(root,tol);
}

/** helper prune*/
void twoDtree::prune(Node* curr, double tol) {
    if (curr != NULL) {
        HSLAPixel currAvg = curr->avg;
        if (isSatisified(curr,tol,currAvg)) {
            clear(curr->LT);
            curr->LT = NULL;
            clear(curr->RB);
            curr->RB = NULL;
        }
        prune(curr->LT,tol);
        prune(curr->RB,tol);
    }
}

/** whether all leaves of current node is within the tolerance */
bool twoDtree::isSatisified(Node * curr, double tol, HSLAPixel & comparePix) {
    bool within = true;
    if (curr != NULL) {
        if (curr->RB == NULL && curr->LT == NULL) {
            double dist = comparePix.dist(curr->avg);
			within = (dist <= tol) && within;
			/*
            if (dist > tol) {
                within = false;
            }
			*/
        }
        else {
            bool left = isSatisified(curr->LT,tol,comparePix);
            bool right = isSatisified(curr->RB,tol,comparePix);
			within = left && right && within;
			/*
            if (!left || !right) {
                within = false;
            }
			*/
        }
    }
    return within;
}
/** clear current tree*/
void twoDtree::clear(Node *curr) {
    if (curr != NULL) {
        clear(curr->LT);
        clear(curr->RB);
        delete curr;
    }
}

/* helper function for destructor and op= */
/* frees dynamic memory associated w the twoDtree */
void twoDtree::clear() {
    /* your code here */
    clear(root);
}


/* helper function for copy constructor and op= */
void twoDtree::copy(const twoDtree & orig){
    this->height = orig.height;
    this->width = orig.width;
    this->root = copy(orig.root);
}

twoDtree::Node * twoDtree::copy(Node * org) {
    Node * newNode = NULL;
    if (org != NULL) {
        newNode = new Node(org->upLeft,org->lowRight,org->avg);
        newNode->LT = copy(org->LT);
        newNode->RB = copy(org->RB);
    }
    return newNode;
}

