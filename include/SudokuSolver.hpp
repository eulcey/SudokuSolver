#ifndef SUDOKU_SOLVER_HPP
#define SUDOKU_SOLVER_HPP

#include <opencv2/core/mat.hpp>

namespace sudoku {
  
  class SudokuSolver {
  public:
    SudokuSolver(cv::Mat mat)
      : matrix(mat) {}
    
    bool solve(cv::Mat& solved);
    
  private:
    cv::Mat matrix;
    
  public:
    class Node {
    public:
      Node(const cv::Mat& m);
      /*
       * creates either a child for the next cell
       * or tries another value for the actual cell
       * if not all were tried yet
       */
      void getNextStep(Node& child);
      /*
       * returns true if node's sudoku is solved
       */
      bool isSolution();
      /*
       * returns true if node has children with still possible not tried
       * numbers, or has itself not tried numbers
       */
      bool hasDesc();
      /*
       * returns the parent node and adds the tried number
       * to its list
       */
      Node* revertStep();
      /*
       * constrains the domain of every cell
       * according to the sudoku rules with the other cells
       */
      void constrainDomains();
      /*
       * returns the equivalent sudoku of the node
       * if every domain size <= 1
       */
      cv::Mat getSudoku();
      /*
       * Constrains the domain of the given cell
       * returns true if the cell domain could be shortened
       */
      bool constrainCell(std::vector<int>& cell, int row, int col);
    private:
      Node(std::vector<std::vector<int>> nrCSP, int nextCell);
      void getCellDomain(int row, int col, std::vector<int>& cell) {
	cell = numberCSP[9*row + col];
      }
      void setCellDomain(int row, int col, std::vector<int> cell) {
	numberCSP[9*row + col] = cell;
      }
      bool hasOpenNumbers() {
	for(size_t i = 0; i < 9; ++i) {
	  if(triedNumbers[i] == 0)
	    return true;
	}
	return false;
      }
      Node* parent = 0;
      std::vector<std::vector<int>> numberCSP;
      int actCell = -1;
      //std::vector<int> triedNumbers;
      char triedNumbers[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    };
  };
}; // namespace sudoku

#endif // SUDOKU_SOLVER_HPP
