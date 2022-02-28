// Distance Vector Routing Algorithm - Bellman-Ford Algorithm
#include <stdio.h>
#include <stdlib.h>
#define inf 1000000000

void dvr(int n, int cost[n][n], int next_hop[n][n])
{
    for (int i = 0; i < n; i++)
    {
        printf("\nRouting table at node %d\n", i + 1);
        for (int j = 0; j < n; j++)
        {

            for (int k = 0; k < n; k++)
            {
                if (cost[i][j] > cost[i][k] + cost[k][j])
                {
                    cost[i][j] = cost[i][k] + cost[k][j];
                    next_hop[i][j] = next_hop[i][k];
                }
            }
            printf("%d\t%d\t%d\n", j + 1, next_hop[i][j] + 1, cost[i][j]);
        }
    }
}

int main()
{
    int n, m, s, d, w;
    scanf("%d %d", &n, &m);
    int cost[n][n];
    int next_hop[n][n];

    for (int i = 0; i < n; i++)
        for (int j = 0; j < i + 1; j++)
        {
            if (i != j)
            {
                cost[i][j] = inf;
                cost[j][i] = inf;
                next_hop[i][j] = -1;
            }
            else
            {
                cost[i][j] = 0;
                next_hop[i][j] = i;
            }
        }

    for (int i = 0; i < m; i++)
    {
        scanf("%d %d %d", &s, &d, &w);
        s--;
        d--;
        cost[s][d] = w;
        next_hop[s][d] = d;
        cost[d][s] = w;
        next_hop[d][s] = s;
    }

    dvr(n, cost, next_hop);
}