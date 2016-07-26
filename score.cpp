#include <cstdio>
#include <unordered_set>
using namespace std;

int main() {

    unordered_set<int> fs;
    int id;
    while( scanf("%d", &id)!=EOF )
        fs.insert(id);

    printf("score: %d\n", (int)fs.size());

    return 0;
}
