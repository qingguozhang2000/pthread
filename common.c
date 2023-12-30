#include "./common.h"

int line_count(FILE *fd)
{
    char ch;
    int lines = 0;

    while (!feof(fd))
    {
        ch = fgetc(fd);
        if (ch == '\n')
        {
            lines++;
        }
    }

    rewind(fd);
    return lines;
}

void load_tables()
{
    FILE *fd = fopen("table.txt", "r");
    if (fd == NULL)
    {
        perror("Unable to open table.txt");
    }

    max_party_size = 0;
    table_count = line_count(fd);
    tables = malloc(table_count * sizeof(struct Table));

    char line[10];
    for (int i = 0; i < table_count; i++)
    {
        char *s = fgets(line, 10, fd);
        if (!s)
        {
            break;
        }

        int capacity = atoi(s);

        tables[i].id = i;
        tables[i].capacity = capacity;
        tables[i].p_guest = NULL;

        if (max_party_size < capacity)
        {
            max_party_size = capacity;
        }
    }

    fclose(fd);
}

void load_names()
{
    FILE *fd = fopen("name.txt", "r");
    if (fd == NULL)
    {
        perror("Unable to open name.txt");
    }

    name_count = line_count(fd);
    names = malloc(name_count * sizeof(char *));

    char line[15];
    for (int i = 0; i < name_count; i++)
    {
        char *name = fgets(line, 15, fd);
        int n = strlen(name);
        names[i] = malloc(n);
        for (int j = 0; j < n; j++)
        {
            if (name[j] != '\n')
            {
                names[i][j] = name[j];
            }
        }
    }

    fclose(fd);
}

void load_config()
{
    load_tables();
    load_names();
}

void free_config()
{
    // free dynamically allocated memory
    free(tables);

    for (int i = 0; i < name_count; i++)
    {
        free(names[i]);
    }
    free(names);
}
