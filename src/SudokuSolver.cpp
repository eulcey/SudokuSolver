#include "SudokuSolver.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

namespace sudoku {

bool isValidBlock(const Mat&);
bool solved(const Mat&);

bool SudokuSolver::solve(Mat& res) {

  Node start(matrix);

  SudokuSolver::Node* node = &start;

  // apply constrains to start node; already in constructor
  node->constrainDomains();
  int steps = 0;
  while(!node->isSolution()) {
  // while(steps < 5 && !node->isSolution()) {
    cout << "=======NEXT STEP=======" << endl;
    ++steps;
    // get next node
    if(node->hasDesc()) {
      cout << "Get Desc:" << endl;
      node->getNextStep(*node);
      cout << node->getSudoku() << endl;
    } else {
      node = node->revertStep();
      if(!node) {
	cerr << "No Solution was found" << endl;
	return false;
      }
    }
    // then try to constrain domains
    node->constrainDomains();

    // TODO break, because not every fully implemented
    //break;
  }
  res = node->getSudoku();
  return true;
}

/*
 * Tests if the given sudoku
 * is a valid solution
 */
bool solved(const Mat& sudoku) {
  if(countNonZero(sudoku) != 81) {
    return false;
  }

  // test every 9x9 block
  for(size_t row = 0; row < 3; ++row) {
    for(size_t col = 0; col < 3; ++col) {
      Rect roi(row*3, col*3, 3, 3);
      if(!isValidBlock(sudoku(roi))) {
	return false;
      }
    }
  }

  // test every column
  for(size_t i = 0; i < 9; ++i) {
     if(!isValidBlock(sudoku.col(i))) {
      return false;
    }
  }

  // test every row
  for(size_t i = 0; i < 9; ++i) {
    if(!isValidBlock(sudoku.row(i))) {
      return false;
    }
  }
  return true;
}

/* Tests if the given block (3x3, 1x9 or 9x1)
 * is a valid sudoku block.
 * that means every digit from 1-9 exactly once
 */
bool isValidBlock(const Mat& block) {
  int count;
  Mat t;
  for(size_t nr = 1; nr <= 9; ++nr) {
    t = (block == nr);
    count = countNonZero(t);
    if(count != 1) {
      return false;
    }
  }

  return true;
}

SudokuSolver::Node::Node(const Mat& m) {
  int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
  for(size_t row = 0; row < m.rows; ++row) {
    for(size_t col = 0; col < m.cols; ++col) {
      char val = m.at<char>(row, col);
      vector<int> csp;
      if(val != 0) {
	csp.push_back((int)val);
      } else {
	csp.assign(arr, arr + 9);
      }
      numberCSP.push_back(csp);
    }
  }
  // TODO temp remove to test DFSAlg
  //  constrainDomains();
  
  /*
  for(size_t i = 0; i < 9; ++i) {
    triedNumbers[i] = 1;
  }
  */
}

SudokuSolver::Node::Node(vector<vector<int>> nrCSP, int nextCell)
  : numberCSP(nrCSP), actCell(nextCell) {
  vector<int> actCellDomain = numberCSP[actCell];
  int val = actCellDomain[0];
  actCellDomain.erase(remove(actCellDomain.begin(), actCellDomain.end(), val));
  for(auto it = actCellDomain.begin(); it < actCellDomain.end(); ++it) {
    triedNumbers[*it] = 0;
  }
  actCellDomain.clear();
  actCellDomain.push_back(val);
  numberCSP[actCell] = actCellDomain;
}

void SudokuSolver::Node::getNextStep(SudokuSolver::Node& child) {
  int smallestOpenSize = 10;
  int nextCell;
  for(size_t pos = 0; pos < numberCSP.size(); ++pos) {
    int nextSize = numberCSP[pos].size();
    if(nextSize != 1 && nextSize < smallestOpenSize) {
      smallestOpenSize = nextSize;
      nextCell = pos;
    }
  }
  cout << "Created next step for cell: " << nextCell << endl;
  child = Node(numberCSP, nextCell);
  child.parent = this;
}

bool SudokuSolver::Node::isSolution() {
  for(size_t i = 0; i < numberCSP.size(); ++i) {
    if(numberCSP[i].size() != 1) {
      return false;
    }
  }
  return true;
}

bool SudokuSolver::Node::hasDesc() {
  for(size_t i = 0; i < numberCSP.size(); ++i) {
    if(numberCSP[i].size() < 1) {
      cout << "csp below 1: " << i << endl;
      return false;
    }
  }
  return true;
  /*
  if(hasOpenNumbers()) {
    return true;
  } else {
    for(size_t i = 0; i < numberCSP.size(); ++i) {
      if(numberCSP[i].size() > 1) {
	return true;
      }
    } 
  }
  return false;
  */
}

SudokuSolver::Node* SudokuSolver::Node::revertStep() {
  if(this->actCell == -1) {
    cout << "trying to revert start" << endl;
  } else  {
    cout << "in reverting " << this->actCell << endl;
  }
  if(!hasOpenNumbers()) {
    cout << "Has no more open numbers, reverting to parent" << endl;
    cout << parent << endl;
    if(parent) {
      return parent->revertStep();
    } else {
      return 0;
    }
  }
  int nextNr;
  for(size_t i = 0; i < 9; ++i) {
    if(triedNumbers[i] == 0) {
      nextNr = i;
      break;
    }
  }
  triedNumbers[nextNr] = 1;
  
  vector<int> actCellDomain = numberCSP[actCell];
  actCellDomain.clear();
  actCellDomain.push_back(nextNr);
  numberCSP[actCell] = actCellDomain;
  // set next number to actCell
  cout << "Try next Number: " << nextNr << " for cell: " << actCell << endl;
  return this;
}

void SudokuSolver::Node::constrainDomains() {
  bool finished = true;
  vector<int> cell;
  do {
    finished = true;
    for(size_t row = 0; row < 9; ++row) {
      for(size_t col = 0; col < 9; ++col) {
	getCellDomain(row, col, cell);
	int domainSize = cell.size();
	if(constrainCell(cell, row, col)) {
	  finished = false;
	}

	setCellDomain(row, col, cell);
      }
    }
  } while(!finished);
}

bool SudokuSolver::Node::constrainCell(vector<int>& cell, int row, int col) {
  // cout << "Constraining (" << row << ", " << col << ") " << " " << cell.size() << endl;
  bool domainChanged = false;
  vector<int> other;
  // constrain according to row
  for(size_t oColl = 0; oColl < 9; ++oColl) {
    if(oColl != col) {
      getCellDomain(row, oColl, other);
      if(other.size() == 1) {
	std::vector<int>::iterator end = remove(cell.begin(), cell.end(), other.at(0));
	if(cell.end() > end) {
	  domainChanged = true;
	}
	cell.erase(end, cell.end());
      }
    }
  }
  // constrain according to col
  for(size_t oRow = 0; oRow < 9; ++oRow) {
    if(oRow != row) {
      getCellDomain(oRow, col, other);
      if(other.size() == 1) {
	std::vector<int>::iterator end = remove(cell.begin(), cell.end(), other.at(0));
	
	if(cell.end() > end) {
	  domainChanged = true;
	}
	
	cell.erase(end, cell.end());
      }
    }
  }
  // constrain according to block
  int blockRow = row/3;
  int blockCol = col/3;
  for(size_t dRow = 0; dRow < 3; ++dRow) {
    for(size_t dCol = 0; dCol < 3; ++dCol) {
      int oRow = (blockRow*3) + dRow;
      int oCol = (blockCol*3) + dCol;
      if(oRow != row && oCol != col) {
	getCellDomain(oRow, oCol, other);
	if(other.size() == 1) {
	  std::vector<int>::iterator end = remove(cell.begin(), cell.end(), other.at(0));
	  if(cell.end() > end) {
	    domainChanged = true;
	  }
	  cell.erase(end, cell.end());
	}
      }
    }
  }
  //cout << "ready" << endl;
  return domainChanged;
}

Mat SudokuSolver::Node::getSudoku() {
  Mat res;
  vector<int> cell;
  for(size_t row = 0; row < 9; ++row) {
    Mat rowMat;
    for(size_t col = 0; col < 9; ++col) {
      getCellDomain(row, col, cell);
      rowMat.push_back(cell.at(0));
    }
    res.push_back(rowMat.t());
  }

  return res;
}

}; // namespace sudoku
