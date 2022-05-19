/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <stdlib.h>
#include "easprintf.h"
#include "attachments.h"
#include "string_helpers.h"

// Pages
#include "../static/attachments.chtml"
#include "../static/attachment_image.chtml"
#include "../static/attachment_gifv.chtml"
#include "../static/attachment_video.chtml"
#include "../static/attachment_link.chtml"
#include "../static/attachment_audio.chtml"

struct attachments_args
{
    struct mstdnt_attachment* atts;
    mstdnt_bool sensitive;
};

int try_upload_media(struct mstdnt_storage** storage,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_attachment** attachments,
                     char*** media_ids)
{
    size_t size = keyfile(ssn->post.files).array_size;
    if (!FILES_READY(ssn))
        return 1;

    if (media_ids)
        *media_ids = malloc(sizeof(char*) * size);

    *attachments = malloc(sizeof(struct mstdnt_attachment) * size);
    *storage = calloc(1, sizeof(struct mstdnt_storage) * size);

    for (int i = 0; i < size; ++i)
    {
        struct file_content* content = keyfile(ssn->post.files).content + i;
        struct mstdnt_upload_media_args args = {
            .file = {
                .file = content->content,
                .filename = content->filename,
                .filesize = content->content_size,
                .filetype = content->filetype,
            },
            .thumbnail = NULL,
            .description = "Treebird image"
        };
        
        if (mastodont_upload_media(api,
                                   &args,
                                   *storage + i,
                                   *attachments + i))
        {
            // EPICFAIL
            for (size_t j = 0; j < i; ++j)
            {
                if (media_ids) free((*media_ids)[j]);
                mastodont_storage_cleanup(*storage + j);
            }

            if (media_ids)
            {
                free(*media_ids);
                *media_ids = NULL;
            }
            
            free(*attachments);
            *attachments = NULL;
            free(*storage);
            *storage = NULL;
            return 1;
        }

        if (media_ids)
        {
            (*media_ids)[i] = malloc(strlen((*attachments)[i].id)+1);
            strcpy((*media_ids)[i], (*attachments)[i].id);
        }
    }
    
    return 0;
}

void cleanup_media_storages(struct session* ssn, struct mstdnt_storage* storage)
{
    if (!FILES_READY(ssn)) return;
    for (size_t i = 0; i < keyfile(ssn->post.files).array_size; ++i)
        mastodont_storage_cleanup(storage + i);
    free(storage);
}

void cleanup_media_ids(struct session* ssn, char** media_ids)
{
    if (!FILES_READY(ssn)) return;
    if (!media_ids) return;
    for (size_t i = 0; i < keyfile(ssn->post.files).array_size; ++i)
        free(media_ids[i]);
    free(media_ids);
}

char* construct_attachment(mstdnt_bool sensitive, struct mstdnt_attachment* att, int* str_size)
{
    char* att_html;
    size_t s;
    const char* attachment_str;
    if (!att) return NULL;

    switch (att->type)
    {
    case MSTDNT_ATTACHMENT_IMAGE:
        attachment_str = data_attachment_image_html; break;
    case MSTDNT_ATTACHMENT_GIFV:
        attachment_str = data_attachment_gifv_html; break;
    case MSTDNT_ATTACHMENT_VIDEO:
        attachment_str = data_attachment_video_html; break;
    case MSTDNT_ATTACHMENT_AUDIO:
        attachment_str = data_attachment_audio_html; break;
    case MSTDNT_ATTACHMENT_UNKNOWN: // Fall through
    default:
        attachment_str = data_attachment_link_html; break;
    }

    // Images/visible content displays sensitive placeholder after
    if (att->type == MSTDNT_ATTACHMENT_IMAGE ||
        att->type == MSTDNT_ATTACHMENT_GIFV ||
        att->type == MSTDNT_ATTACHMENT_VIDEO)
    {
        s = easprintf(&att_html, attachment_str,
                      att->url,
                      sensitive ? "<div class=\"sensitive-contain sensitive\"></div>" : "");
    }
    else {
        s = easprintf(&att_html, attachment_str,
                      sensitive ? "sensitive" : "",
                      att->url);
    }
    
    if (str_size) *str_size = s;
    return att_html;
}

static char* construct_attachments_voidwrap(void* passed, size_t index, int* res)
{
    struct attachments_args* args = passed;
    return construct_attachment(args->sensitive, args->atts + index, res);
}

char* construct_attachments(mstdnt_bool sensitive, struct mstdnt_attachment* atts, size_t atts_len, size_t* str_size)
{
    size_t elements_size;
    struct attachments_args args = { atts, sensitive };
    char* elements = construct_func_strings(construct_attachments_voidwrap, &args, atts_len, &elements_size);
    char* att_view;

    size_t s = easprintf(&att_view, data_attachments_html, elements);
    if (str_size) *str_size = s;
    // Cleanup
    free(elements);
    return att_view;
}
