#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

const int grid_width = 30;
const int grid_height = 30;

char *create_grid()
{
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid)
{
    free(grid);
}

void draw_grid(char *grid)
{
    for (int i = 0; i < grid_height; ++i)
    {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j)
        {
            if (grid[i * grid_width + j])
            {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            }
            else
            {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid)
{
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid)
{

    int count = 0;
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width)
            {
                continue;
            }
            if (grid[grid_width * r + c])
            {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col])
    {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    }
    else
    {
        if (count == 3)
            return true;
        else
            return false;
    }
}

typedef struct {
    int start;
    int end;
    char* src;
    char* dst;
} thread_args;

void *cells_handler(void *arguments){
    thread_args *args = (thread_args *) arguments;

    while(1) {

        for (int i=args->start; i<=args->end; i++)
            args->dst[i] = is_alive(i/grid_width, i%grid_height, args->src);  
        
        pause();
    }
}

int min(int a, int b){
    if (a<b) return a;
    return b;
}

void handler(int _) {}

void update_grid(char *src, char *dst, int no_threads)
{   
    /*
    for (int i = 0; i < grid_height; ++i)
    {
        for (int j = 0; j < grid_width; ++j)
        {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
    */
    
   signal(SIGUSR1, handler);

   pthread_t *threads = NULL;
   
   if (!threads){
        
        int n = grid_height * grid_width;

        if (no_threads>n || no_threads<=0) no_threads = n;

        int block_width = n / no_threads; 
        if (n % block_width==0) block_width += 1;

        threads = calloc(no_threads, sizeof(pthread_t));

        for (int i=0; i<no_threads; i++){
            thread_args* args = calloc(1, sizeof(thread_args));
            args->src = src;
            args->dst = dst;
            
            args->start = i*block_width;
            args->end = min(args->start+block_width, n-1);

            //printf("%d %d %d", args->start, args->end, n);

            pthread_create(&threads[i], NULL, cells_handler, (void*)args);
        }
   }

    for (int i=0; i<no_threads; i++){
        pthread_kill(threads[i], SIGUSR1);
    }

}