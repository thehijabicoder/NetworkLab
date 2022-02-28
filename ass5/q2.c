#include <stdio.h>
#include <stdlib.h>
#define inf 1000000000

int isallvisited(int visited[], int n)
{
    for (int i = 0; i < n; i++)
    {
        if (visited[i] == 0)
            return 0;
    }
    return 1;
}

void lsr(int n, int cost[n][n], int src)
{
    int visited[n];
    int dist[n];
    int path[n][n];
    int index[n];
    printf("\nAt node %d\n", src + 1);

    for (int i = 0; i < n; i++)
    {
        visited[i] = 0;
        dist[i] = cost[src][i];
        path[i][0] = src + 1;
        index[i] = 1;
        for (int j = 1; j < n; j++)
        {
            path[i][j] = -1;
        }
    }

    visited[src] = 1;
    dist[src] = 0;
    path[src][index[src]++] = src + 1;

    while (!isallvisited(visited, n))
    {
        int min = inf, u;
        for (int j = 0; j < n; j++)
        {
            if (!visited[j] && dist[j] <= min)
            {
                min = dist[j];
                u = j;
            }
        }

        visited[u] = 1;
        path[u][index[u]++] = u + 1;

        for (int j = 0; j < n; j++)
        {
            if (!visited[j] && cost[u][j] != inf)
            {
                if (dist[u] + cost[u][j] < dist[j])
                {
                    dist[j] = dist[u] + cost[u][j];
                    index[j] = 0;
                    for (int k = 0; k < index[u]; k++)
                    {
                        path[j][index[j]++] = path[u][k];
                    }
                }
            }
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < index[i] - 1; j++)
        {
            printf("%d -> ", path[i][j]);
        }
        printf("%d", path[i][index[i] - 1]);
        for (int k = index[i]; k < n; k++)
        {
            printf("\t");
        }
        printf("%4d\n", dist[i]);
    }
}

int main()
{
    int n, m, s, d, w;
    scanf("%d %d", &n, &m);
    int cost[n][n];

    for (int i = 0; i < n; i++)
        for (int j = 0; j < i + 1; j++)
        {
            if (i != j)
            {
                cost[i][j] = inf;
                cost[j][i] = inf;
            }
            else
            {
                cost[i][j] = 0;
            }
        }

    for (int i = 0; i < m; i++)
    {
        scanf("%d %d %d", &s, &d, &w);
        s--;
        d--;
        cost[s][d] = w;
        cost[d][s] = w;
    }

    for (int i = 0; i < n; i++)
    {
        lsr(n, cost, i);
    }
}
