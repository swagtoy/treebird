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
#include "base_page.h"
#include "helpers.h"
#include "easprintf.h"
#include "attachments.h"
#include "string_helpers.h"


struct attachments_args
{
    struct session* ssn;
    struct mstdnt_attachment* atts;
    mstdnt_bool sensitive;
};

int try_upload_media(struct mstdnt_storage** storage,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_attachment** attachments,
                     char*** media_ids)
{
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
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
            .description = NULL,
        };
        
        if (mastodont_upload_media(api,
                                   &m_args,
                                   &args,
                                   *storage + i,
                                   *attachments + i))
        {
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

HV* perlify_attachment(struct mstdnt_attachment* const attachment)
{
    if (!attachment) return NULL;
    HV* attach_hv = newHV();
    hvstores_str(attach_hv, "id", attachment->id);
    hvstores_int(attach_hv, "type", attachment->type);
    hvstores_str(attach_hv, "url", attachment->url);
    hvstores_str(attach_hv, "preview_url", attachment->preview_url);
    hvstores_str(attach_hv, "remote_url", attachment->remote_url);
    hvstores_str(attach_hv, "hash", attachment->hash);
    hvstores_str(attach_hv, "description", attachment->description);
    hvstores_str(attach_hv, "blurhash", attachment->blurhash);
    return attach_hv;
}

AV* perlify_attachments(struct mstdnt_attachment* const attachments, size_t len)
{
    if (!(attachments && len)) return NULL;
    AV* av = newAV();
    av_extend(av, len-1);

    for (int i = 0; i < len; ++i)
    {
        av_store(av, i, newRV_inc((SV*)perlify_attachment(attachments + i)));
    }

    return av;
}

void api_attachment_create(PATH_ARGS)
{
    struct mstdnt_storage *att_storage = NULL;
    struct mstdnt_attachment* attachments = NULL;
    char* string;
    char** media_ids = NULL;

    cJSON* root = cJSON_CreateObject();

    // TODO If multiple attachments are submitted, this uploads all of them.
    //      I don't think we want that, but it's just a minor issue and won't happen on TreebirdFE.
    try_upload_media(&att_storage, ssn, api, &attachments, &media_ids);

    if (media_ids)
        cJSON_AddStringToObject(root, "id", media_ids[0]);
    
    if (media_ids)
    {
        string = cJSON_Print(root);
        send_result(req, NULL, "application/json", string, 0);
        free(string);
    }
    else
        send_result(req, NULL, "application/json", "{\"status\":\"Couldn't\"}", 0);

    // Cleanup media stuff
    cleanup_media_storages(ssn, att_storage);
    cleanup_media_ids(ssn, media_ids);
    free(attachments);
    cJSON_Delete(root);
}
