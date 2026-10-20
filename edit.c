#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "edit.h"

void edit_tag(char *filename, char *tag, char *new_value)
{
    FILE *fp;
    FILE *temp_fp;

    unsigned char header[10];
    unsigned char frame_header[10];

    unsigned int tag_size;
    unsigned int frame_size;
    unsigned int bytes_read = 0;

    char frame_id[5];
    int tag_found = 0;

    /* Open original MP3 */
    fp = fopen(filename, "rb");

    if (fp == NULL)
    {
        printf("ERROR: Unable to open %s\n", filename);
        return;
    }

    /* Read ID3 header */
    if (fread(header, 1, 10, fp) != 10)
    {
        printf("ERROR: Unable to read ID3 header\n");
        fclose(fp);
        return;
    }

    /* Validate ID3 */
    if (header[0] != 'I' ||
        header[1] != 'D' ||
        header[2] != '3')
    {
        printf("ERROR: ID3 tag not found\n");
        fclose(fp);
        return;
    }

    /* Support ID3v2.3 */
    if (header[3] != 3)
    {
        printf("ERROR: Only ID3v2.3 is supported\n");
        fclose(fp);
        return;
    }

    /* Calculate tag size */
    tag_size = ((unsigned int)(header[6] & 0x7F) << 21) |
               ((unsigned int)(header[7] & 0x7F) << 14) |
               ((unsigned int)(header[8] & 0x7F) << 7) |
               (unsigned int)(header[9] & 0x7F);

    /* Create temporary file */
    temp_fp = fopen("temp.mp3", "wb");

    if (temp_fp == NULL)
    {
        printf("ERROR: Unable to create temp.mp3\n");
        fclose(fp);
        return;
    }

    /* Copy ID3 header */
    fwrite(header, 1, 10, temp_fp);

    /* Read frames */
    while (bytes_read < tag_size)
    {
        /* Read frame header */
        if (fread(frame_header, 1, 10, fp) != 10)
        {
            break;
        }

        bytes_read += 10;

        /* Empty frame / padding */
        if (frame_header[0] == 0)
        {
            /*
             * Copy the 10 bytes already read.
             */
            fwrite(frame_header, 1, 10, temp_fp);

            /*
             * Copy remaining ID3 padding.
             */
            while (bytes_read < tag_size)
            {
                unsigned char ch;

                if (fread(&ch, 1, 1, fp) != 1)
                    break;

                fwrite(&ch, 1, 1, temp_fp);
                bytes_read++;
            }

            break;
        }

        /* Get frame ID */
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        /* Get frame size */
        frame_size = ((unsigned int)frame_header[4] << 24) |
                     ((unsigned int)frame_header[5] << 16) |
                     ((unsigned int)frame_header[6] << 8) |
                     (unsigned int)frame_header[7];

        /* Check selected frame */
        if ((strcmp(tag, "-t") == 0 &&
             strcmp(frame_id, "TIT2") == 0) ||

            (strcmp(tag, "-a") == 0 &&
             strcmp(frame_id, "TPE1") == 0) ||

            (strcmp(tag, "-A") == 0 &&
             strcmp(frame_id, "TALB") == 0) ||

            (strcmp(tag, "-y") == 0 &&
             strcmp(frame_id, "TYER") == 0) ||

            (strcmp(tag, "-g") == 0 &&
             strcmp(frame_id, "TCON") == 0))
        {
            unsigned int new_size;
            unsigned int remaining;

            /*
             * Text frame:
             * 1 byte encoding + new string
             */
            new_size = strlen(new_value) + 1;

            printf("\nFrame found : %s\n", frame_id);
            printf("Old frame size : %u\n", frame_size);
            printf("New frame size : %u\n", new_size);

            /*
             * New value must fit.
             */
            if (new_size > frame_size)
            {
                printf("ERROR: New value is too large\n");
                printf("Current version cannot resize the frame\n");

                fclose(fp);
                fclose(temp_fp);

                remove("temp.mp3");

                return;
            }

            /* Copy frame ID */
            fwrite(frame_header, 1, 4, temp_fp);

            /* Keep original frame size */
            fwrite(frame_header + 4, 1, 4, temp_fp);

            /* Keep original flags */
            fwrite(frame_header + 8, 1, 2, temp_fp);

            /* Encoding = 0 */
            fputc(0, temp_fp);

            /* Write new value */
            fwrite(new_value, 1, strlen(new_value), temp_fp);

            /* Remaining space */
            remaining = frame_size - new_size;

            /* Fill remaining space */
            for (unsigned int i = 0; i < remaining; i++)
            {
                fputc(0, temp_fp);
            }

            /* Skip old frame data */
            fseek(fp, frame_size, SEEK_CUR);

            bytes_read += frame_size;

            tag_found = 1;

            printf("Tag updated successfully\n");

            continue;
        }

        /*
         * Normal frame:
         * Copy header.
         */
        fwrite(frame_header, 1, 10, temp_fp);

        /*
         * Copy frame data.
         */
        unsigned char *data = malloc(frame_size);

        if (data == NULL)
        {
            printf("ERROR: Memory allocation failed\n");

            fclose(fp);
            fclose(temp_fp);

            remove("temp.mp3");

            return;
        }

        if (fread(data, 1, frame_size, fp) != frame_size)
        {
            printf("ERROR: Unable to read frame data\n");

            free(data);

            fclose(fp);
            fclose(temp_fp);

            remove("temp.mp3");

            return;
        }

        fwrite(data, 1, frame_size, temp_fp);

        free(data);

        bytes_read += frame_size;
    }

    /*
     * Copy remaining MP3 audio data.
     */
    unsigned char buffer[1024];
    size_t count;

    while ((count = fread(buffer, 1, sizeof(buffer), fp)) > 0)
    {
        fwrite(buffer, 1, count, temp_fp);
    }

    fclose(fp);
    fclose(temp_fp);

    /* Check whether requested tag was found */
    if (tag_found == 0)
    {
        printf("\nERROR: Requested tag not found\n");

        remove("temp.mp3");

        return;
    }

    /*
     * Replace original file with edited file.
     */
    if (remove(filename) != 0)
    {
        printf("ERROR: Unable to remove original file\n");
        return;
    }

    if (rename("temp.mp3", filename) != 0)
    {
        printf("ERROR: Unable to rename temporary file\n");
        return;
    }

    printf("\n=====================================\n");
    printf("       EDIT SUCCESSFUL\n");
    printf("=====================================\n");
    printf("Updated file : %s\n", filename);
}