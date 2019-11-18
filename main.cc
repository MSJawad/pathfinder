#include "maze.h"
using namespace std;
int main(int argc, char * argv[]) {

  binary_heap <int> myheap;

  myheap.add(7);
  myheap.add(9);
  myheap.add(18);
  myheap.add(2);
  cout << myheap << endl;

}
