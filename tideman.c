#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
void mergesort(int a[], pair b[], int temp1[], pair temp2[], int l, int r);
bool circle(int w, int l);
bool every_colum(int l);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            int pi = 0;
            int pj = 0;
            for (int n = 0; n < candidate_count; n++)
            {
                if (ranks[n] == i)
                {
                    pi = n;
                }
                if (ranks[n] == j)
                {
                    pj = n;
                }
            }
            if (pi < pj)
            {
                preferences[i][j]++;
            }
        }
    }

    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] != preferences[j][i] && j > i)
            {
                pair_count++;
                if (preferences[i][j] > preferences[j][i])
                {
                    pairs[pair_count - 1].winner = i;
                    pairs[pair_count - 1].loser = j;
                }
                else
                {
                    pairs[pair_count - 1].winner = j;
                    pairs[pair_count - 1].loser = i;
                }
            }
        }
    }
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
    int score[pair_count];
    for (int i = 0; i < pair_count; i++)
    {
        score[i] = preferences[pairs[i].winner][pairs[i].loser];
    }
    int temp1[pair_count];
    pair temp2[pair_count];
    mergesort(score, pairs, temp1, temp2, 0, pair_count - 1);
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    for (int i = 0; i < pair_count; i++)
    {
        int w = pairs[i].winner;
        int l = pairs[i].loser;
        if (i == pair_count - 1)
        {
            if (every_colum(l) == false)
            {
                locked[w][l] = true;
            }
        }
        else
        {
            if (circle(w, l) == false)
            {
                locked[w][l] = true;
            }
        }
    }
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    int count;
    for (int i = 0; i < candidate_count; i++)
    {
        count = 0;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i] == false)
            {
                count++;
            }
        }
        if (count == candidate_count)
        {
            printf("%s\n", candidates[i]);
        }
    }
}

void mergesort(int a[], pair b[], int temp1[], pair temp2[], int l, int r)
{
    if (l != r)
    {
        int mid = (l + r) / 2;
        mergesort(a, b, temp1, temp2, l, mid);
        mergesort(a, b, temp1, temp2, mid + 1, r);
        for (int i = l; i <= r; i++)
        {
            temp1[i] = a[i];
            temp2[i] = b[i];
        }
        int i = 0, j = 0, k = 0;
        for (i = l, j = mid + 1, k = l; i <= mid && j <= r && k <= r; k++)
        {
            if (temp1[i] > temp1[j])
            {
                a[k] = temp1[i];
                b[k] = temp2[i];
                i++;
            }
            else
            {
                a[k] = temp1[j];
                b[k] = temp2[j];
                j++;
            }
        }
        int m = i, n = j, s = k;
        for (; m <= mid; m++, s++)
        {
            a[s] = temp1[m];
            b[s] = temp2[m];
        }
        for (; n <= r; n++, s++)
        {
            a[s] = temp1[n];
            b[s] = temp2[n];
        }
    }
}

bool circle(int w, int l)
{
    for (int i = 0; i < pair_count; i++)
    {
        if (locked[i][w] == true && locked[l][i] == true)
        {
            return true;
        }
    }
    return false;
}

bool every_colum(int l)
{
    int count = 0;
    for (int j = 0; j < pair_count; j++)
    {
        if (j != l)
        {
            bool flag = false;
            for (int i = 0; i < pair_count; i++)
            {
                if (locked[i][j] == true)
                {
                    flag = true;
                }
            }
            if (flag == true)
            {
                count++;
            }
        }
    }
    if (count ==  pair_count - 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}