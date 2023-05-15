#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

const int grid_width = 30;
const int grid_height = 30;

typedef struct {
    int i;
    char* src;
    char* dst;
} thread_args;


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

void *cells_handler(void *arguments){
    thread_args *args = (thread_args *) arguments;

    while(1) {
        args->dst[args->i] = is_alive(
                        (args->i)/grid_width, 
                        (args->i)%grid_height, 
                        args->src);  
        pause();
    }
}

void handler(int _) {}

void update_grid(char *src, char *dst)
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

    int n = grid_height * grid_width;

   signal(SIGUSR1, handler);

   pthread_t *threads = NULL;
   
   if (!threads){
        
        threads = calloc(n, sizeof(pthread_t));

        for (int i=0; i<n; i++){
            thread_args* args = calloc(1, sizeof(thread_args));
            args->src = src;
            args->dst = dst;
            args->i = i;

            pthread_create(&threads[i], NULL, cells_handler, (void*)args);
        }
   }

    for (int i=0; i<n; i++){
        pthread_kill(threads[i], SIGUSR1);
    }

}
