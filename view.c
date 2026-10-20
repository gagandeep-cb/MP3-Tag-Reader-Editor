#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "view.h"

void view_tag(char *filename)
{
    FILE *fp;

    unsigned char header[10];
    unsigned char frame_header[10];

    unsigned int tag_size;
    unsigned int frame_size;
    unsigned int bytes_read = 0;

    char frame_id[5];
    char *data;

    /* Open MP3 file */
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("ERROR: Unable to open file %s\n", filename);
        return;
    }

    /* Read ID3 header */
    if (fread(header, 1, 10, fp) != 10)
    {
        printf("ERROR: Unable to read ID3 header\n");
        fclose(fp);
        return;
    }

    /* Check ID3 */
    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        printf("ERROR: ID3 tag not found\n");
        fclose(fp);
        return;
    }
 
    /*
     * Move file pointer to the beginning
     * of ID3 tag frame area.
     *
     * Header = 10 bytes
     */
    fseek(fp, 10, SEEK_SET);

    /* Calculate ID3 tag size */
    tag_size = ((header[6] & 0x7F) << 21) |
               ((header[7] & 0x7F) << 14) |
               ((header[8] & 0x7F) << 7) |
               (header[9] & 0x7F);

    printf("\n=====================================\n");
    printf("          MP3 TAG INFORMATION\n");
    printf("=====================================\n");

    printf("ID3 version : 2.%d.%d\n",header[3],header[4] );

    /* Read frames */
    while (bytes_read < tag_size)
    {
        /* Read frame header */
        if (fread(frame_header, 1, 10, fp) != 10)
        {
            break;
        }

        bytes_read += 10;

        /* Check empty frame */
        if (frame_header[0] == 0)
        {
            break;
        }

        /* Copy Frame ID */
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        /* Calculate Frame Size */
        frame_size = ((unsigned int)frame_header[4] << 24) |
                     ((unsigned int)frame_header[5] << 16) |
                     ((unsigned int)frame_header[6] << 8) |
                     (unsigned int)frame_header[7];

        /* Allocate memory */
        data = malloc(frame_size + 1);

        if (data == NULL)
        {
            printf("ERROR: Memory allocation failed\n");
            fclose(fp);
            return;
        }

        /* Read frame data */
        if (fread(data, 1, frame_size, fp) != frame_size)
        {
            free(data);
            break;
        }

        bytes_read += frame_size;

        data[frame_size] = '\0';

        /* Display required tags */

        if (strcmp(frame_id, "TIT2") == 0)
        {
            printf("Title   : %s\n", data + 1);
        }
        else if (strcmp(frame_id, "TPE1") == 0)
        {
            printf("Artist  : %s\n", data + 1);
        }
        else if (strcmp(frame_id, "TALB") == 0)
        {
            printf("Album   : %s\n", data + 1);
        }
        else if (strcmp(frame_id, "TYER") == 0)
        {
            printf("Year    : %s\n", data + 1);
        }
        else if (strcmp(frame_id, "TCON") == 0)
        {
            printf("Genre   : %s\n", data + 1);
        }
        else if (strcmp(frame_id, "COMM") == 0)
        {
            printf("Comment : %s\n", data + 1);
        }

        free(data);
    }

    printf("=====================================\n");

    fclose(fp);
}