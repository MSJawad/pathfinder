#include "maze.h"
using namespace std;

int main(int argc, char * argv[]) {

  /* binary_heap <int> myheap; */

  /* myheap.add(7); */
  /* myheap.add(9); */
  /* myheap.add(18); */
  /* myheap.add(2); */
  /* myheap.add(10); */
  /* myheap.add(5); */

  /* myheap.add(26); */
  /* myheap.add(14); */
  /* myheap.add(91); */
  /* myheap.add(87); */
  /* myheap.add(24); */
  /* myheap.add(31); */
  /* myheap.add(83); */

  /* vector <int> store; */
  /* while (!myheap.is_Empty()) { */
  /*     int rv = myheap.remove_top(); */
  /*     store.push_back(rv); */
  /* } */
  /* for (unsigned int i = 0; i < store.size(); i++) { */
  /*     cout << store[i] << " "; */
  /* } */
  /* cout << endl; */

  Grid myGrid{67};
  myGrid.GenerateMaze();
  cout << myGrid;
  int val;
  cin >> val;
}
