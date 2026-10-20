/*

----------------Project Details--------------------

Project Name: MP3 Tag Reader and Editor

Name: Gagandeep C Banakar

Start Date: [19/08/2026]

End Date: [15/09/2026]

Language: C Programming

Platform: Linux / Windows

Project Description: MP3 Tag Reader and Editor is a C-based project used to read and edit ID3 metadata from MP3 audio files.

It allows users to view and modify metadata such as song title, artist name, album, year, genre, and comments using command-line operations.

It uses binary file handling techniques such as fread(), fwrite(), fseek(), and ftell() to read and manipulate ID3 frames.

The project uses structures, functions, pointers, dynamic memory allocation, and command-line arguments to manage the MP3 metadata.

It validates the MP3 file format, identifies ID3 frames, creates a temporary MP3 file during editing, and replaces the original file after successful modification.

*/


#include <stdio.h>
#include <string.h>
#include "view.h"
#include"edit.h"

int main(int argc, char *argv[])
{
    /* Check command line arguments */

    if (argc < 2)
    {
        printf("ERROR: Insufficient arguments\n");
        printf("\n");
        printf("Usage:\n");
        printf("./mp3_tag_reader -v <file.mp3>\n");
        printf("./mp3_tag_reader -e <file.mp3>\n");
        printf("./mp3_tag_reader -h\n");

        return 1;
    }

    /* VIEW operation */

    if (strcmp(argv[1], "-v") == 0)
    {
        if (argc != 3)
        {
            printf("ERROR: MP3 file name is missing\n");
            printf("Usage: ./mp3_tag_reader -v <file.mp3>\n");

            return 1;
        }
        view_tag(argv[2]);
    }
    /* EDIT operation */

    else if (strcmp(argv[1], "-e") == 0)
    {
        if (argc != 5)
        {
            printf("ERROR: Invalid arguments\n");
            printf("Usage: ./a.out -e <option> <new_value> <file.mp3>\n");

            return 1;
        }

        printf("Edit operation selected\n");
        printf("Edit option : %s\n", argv[2]);
        printf("New value   : %s\n", argv[3]);
        printf("Input file  : %s\n", argv[4]);

        edit_tag(argv[4], argv[2], argv[3]);
    }

    /* HELP operation */

    else if (strcmp(argv[1], "-h") == 0)
    {
        if (argc != 2)
        {
            printf("ERROR: Invalid arguments for help\n");
            printf("Usage: ./mp3_tag_reader -h\n");

            return 1;
        }

        printf("\n");
        printf("=====================================\n");
        printf("       MP3 TAG READER AND EDITOR\n");
        printf("=====================================\n");

        printf("\nUsage:\n");
        printf("  ./mp3_tag_reader -v <file.mp3>\n");
        printf("      View MP3 tag information\n");

        printf("\n");
        printf("  ./mp3_tag_reader -e <file.mp3>\n");
        printf("      Edit MP3 tag information\n");

        printf("\n");
        printf("  ./mp3_tag_reader -h\n");
        printf("      Display help information\n");

        printf("Following are some options to edit tags : \n\n");
        printf("-t : Modifies Title \n");
        printf("-A : Modifies Artist name \n");
        printf("-a : Modifies Album name \n");
        printf("-y : Modifies Year \n");
        printf("-m : Modifies Content type tag \n");
        printf("-c : Modifies Composer tag\n\n");

        printf("\n");
        printf("=====================================\n");
    }

    /* INVALID operation */

    else
    {
        printf("ERROR: Invalid option '%s'\n", argv[1]);

        printf("\nValid options:\n");
        printf("-v  View MP3 tags\n");
        printf("-e  Edit MP3 tags\n");
        printf("-h  Help\n");

        return 1;
    }

    return 0;
}